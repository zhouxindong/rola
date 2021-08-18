#ifndef ROLA_STLEX_INTERRUPTIBLE_THREAD_HPP
#define ROLA_STLEX_INTERRUPTIBLE_THREAD_HPP

#include <functional>
#include <future>
#include <mutex>
#include <type_traits>
#include <chrono>

namespace rola
{
	class Interruption_flag;

	extern thread_local Interruption_flag interrupt_flag;

    class Interruption_flag
    {
    public:
        void set()
        {
            flag_.store(true, std::memory_order::memory_order_relaxed);

            std::lock_guard<std::mutex> lck(mtx_);
            if (cnd_)
                cnd_->notify_all();

            if (cnda_)
                cnda_->notify_all();
        }

        bool is_set() const
        {
            return flag_.load(std::memory_order::memory_order_relaxed);
        }

        struct cnd_guard
        {
            cnd_guard(std::condition_variable& cnd)
            {
                interrupt_flag.set_cnd(cnd);
            }

            cnd_guard(std::condition_variable_any& cnd)
            {
                interrupt_flag.set_cnd(cnd);
            }

            ~cnd_guard()
            {
                interrupt_flag.clear_cnd();
            }
        };
    private:
        void set_cnd(std::condition_variable& cnd)
        {
            std::lock_guard<std::mutex> lck(mtx_);
            cnd_ = &cnd;
        }

        void set_cnd(std::condition_variable_any& cnd)
        {
            std::lock_guard<std::mutex> lck(mtx_);
            cnda_ = &cnd;
        }

        void clear_cnd()
        {
            std::lock_guard<std::mutex> lck(mtx_);
            cnd_ = nullptr;
            cnda_ = nullptr;
        }

        template<typename Lock, typename Pred>
        friend void interruptible_wait(std::condition_variable_any& cnd, Lock& lck, Pred pred);

        std::atomic<bool> flag_ = false;

        mutable std::mutex mtx_;
        std::condition_variable* cnd_ = nullptr;
        std::condition_variable_any* cnda_ = nullptr;
    };

    static thread_local Interruption_flag interrupt_flag;


    class Interruptible_thread : public std::thread
    {
    public:
        template<typename Fn, typename... Args,
            typename = std::enable_if_t<!std::is_same_v<std::decay_t<Fn>, Interruptible_thread>>>
            Interruptible_thread(Fn&& fn, Args&&... args)
        {
            std::promise<Interruption_flag*> pro;
            auto fut = pro.get_future();

            static_cast<std::thread&>(*this) = std::thread([&] {
                auto t = std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...);
                pro.set_value(&interrupt_flag);

                try
                {
                    t();
                }
                catch (Thread_interrupted&)
                {
                }
                });

            flag_ = fut.get();
        }

        void interrupt()
        {
            if (!joinable())
                throw(std::runtime_error("no running thread"));

            flag_->set();
        }

    private:
        struct Thread_interrupted : public std::exception {};

        friend void interruption_point();

        Interruption_flag* flag_ = nullptr;
    };

    inline void interruption_point()
    {
        if (interrupt_flag.is_set())
            throw Interruptible_thread::Thread_interrupted();
    }

    template<typename Pred>
    void interruptible_wait(std::condition_variable& cnd, std::unique_lock<std::mutex>& lck, Pred pred,
        std::chrono::milliseconds delay = std::chrono::milliseconds(1))
    {
        interruption_point();
        Interruption_flag::cnd_guard guard(cnd);

        while (!pred())
        {
            interruption_point();
            cnd.wait_for(lck, delay);
        }

        interruption_point();
    }

    template<typename Lock, typename Pred>
    void interruptible_wait(std::condition_variable_any& cnd, Lock& lck, Pred pred)
    {
        struct Custom_lock
        {
            Custom_lock(std::mutex& mtx, Lock& lck) :
                mtx_(mtx), lck_(lck)
            {
                mtx_.lock();
            }

            ~Custom_lock()
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

            std::mutex& mtx_;
            Lock& lck_;
        };

        interruption_point();
        Interruption_flag::cnd_guard guard(cnd);

        Custom_lock c_lck(interrupt_flag.mtx_, lck);
        interruption_point();

        cnd.wait(c_lck, [&] { return interrupt_flag.is_set() || pred(); });
        interruption_point();
    }
} // namespace rola

#endif // !ROLA_STLEX_INTERRUPTIBLE_THREAD_HPP
