#ifndef ROLA_STLEX_INTERRUPTIBLE_THREAD_HPP
#define ROLA_STLEX_INTERRUPTIBLE_THREAD_HPP

#include <functional>
#include <future>
#include <mutex>

namespace rola
{
	class interruption_flag;

	extern thread_local interruption_flag int_flag;

    class interruption_flag
    {
    public:
        void set()
        {
            flag_.store(true, memory_order::memory_order_relaxed);

            std::lock_guard<mutex> lck(mtx_);
            if (cnd_)
                cnd_->notify_all();

            if (cnda_)
                cnda_->notify_all();
        }

        bool is_set() const
        {
            return flag_.load(memory_order::memory_order_relaxed);
        }

        struct cnd_guard
        {
            cnd_guard(condition_variable& cnd)
            {
                int_flag.set_cnd(cnd);
            }

            cnd_guard(condition_variable_any& cnd)
            {
                int_flag.set_cnd(cnd);
            }

            ~cnd_guard()
            {
                int_flag.clear_cnd();
            }
        };
    private:
        void set_cnd(condition_variable& cnd)
        {
            std::lock_guard<mutex> lck(mtx_);
            cnd_ = &cnd;
        }

        void set_cnd(condition_variable_any& cnd)
        {
            std::lock_guard<mutex> lck(mtx_);
            cnda_ = &cnd;
        }

        void clear_cnd()
        {
            std::lock_guard<mutex> lck(mtx_);
            cnd_ = nullptr;
            cnda_ = nullptr;
        }

        template<typename Lock, typename Pred>
        friend void interruptible_wait(condition_variable_any& cnd, Lock& lck, Pred pred);

        atomic<bool> flag_ = false;

        mutable mutex mtx_;
        condition_variable* cnd_ = nullptr;
        condition_variable_any* cnda_ = nullptr;
    };

    static thread_local interruption_flag int_flag;


    class interruptible_thread : public thread
    {
    public:
        template<typename Fn, typename... Args,
            typename = enable_if_t<!is_same_v<decay_t<Fn>, interruptible_thread>>>
            interruptible_thread(Fn&& fn, Args&&... args)
        {
            promise<interruption_flag*> pro;
            auto fut = pro.get_future();

            static_cast<thread&>(*this) = thread([&] {
                auto t = bind(forward<Fn>(fn), forward<Args>(args)...);
                pro.set_value(&int_flag);

                try
                {
                    t();
                }
                catch (thread_interrupted&)
                {
                }
                });

            flag_ = fut.get();
        }

        void interrupt()
        {
            if (!joinable())
                throw(runtime_error("no running thread"));

            flag_->set();
        }

    private:
        struct thread_interrupted : public exception {};

        friend void interruption_point();

        interruption_flag* flag_ = nullptr;
    };

    inline void interruption_point()
    {
        if (int_flag.is_set())
            throw interruptible_thread::thread_interrupted();
    }

    template<typename Pred>
    void interruptible_wait(condition_variable& cnd, unique_lock<mutex>& lck, Pred pred,
        chrono::milliseconds delay = chrono::milliseconds(1))
    {
        interruption_point();
        interruption_flag::cnd_guard guard(cnd);

        while (!pred())
        {
            interruption_point();
            cnd.wait_for(lck, delay);
        }

        interruption_point();
    }

    template<typename Lock, typename Pred>
    void interruptible_wait(condition_variable_any& cnd, Lock& lck, Pred pred)
    {
        struct custom_lock
        {
            custom_lock(mutex& mtx, Lock& lck) :
                mtx_(mtx), lck_(lck)
            {
                mtx_.lock();
            }

            ~custom_lock()
            {
                mtx_.unlock();
            }

            void lock()
            {
                std::lock(mtx_, lck_);
            }

            void unlock()
            {
                mtx_.unlock();
                lck_.unlock();
            }

            mutex& mtx_;
            Lock& lck_;
        };

        interruption_point();
        interruption_flag::cnd_guard guard(cnd);

        custom_lock c_lck(int_flag.mtx_, lck);
        interruption_point();

        cnd.wait(c_lck, [&] { return int_flag.is_set() || pred(); });
        interruption_point();
    }
} // namespace rola

#endif // !ROLA_STLEX_INTERRUPTIBLE_THREAD_HPP
