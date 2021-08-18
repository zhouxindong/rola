#ifndef ROLA_STLEX_THREAD_POOL_HPP
#define ROLA_STLEX_THREAD_POOL_HPP

#include <atomic>
#include "thread_safe_queue.hpp"
#include <functional>
#include "thread.hpp"

namespace rola
{
	class Simple_thread_pool
	{
		std::atomic_bool done;
		Thread_safe_queue<std::function<void()> > work_queue;
		std::vector<std::thread> threads;
		stlex::Join_threads joiner;

		void worker_thread()
		{
			while (!done)
			{
				std::function<void()> task;
				if (work_queue.try_pop(task))
				{
					task();
				}
				else
				{
					std::this_thread::yield();
				}
			}
		}
	public:
		Simple_thread_pool() :
			done(false), joiner(threads)
		{
			unsigned const thread_count = std::thread::hardware_concurrency();
			try
			{
				for (unsigned i = 0; i < thread_count; ++i)
				{
					threads.push_back(
						std::thread(&Simple_thread_pool::worker_thread, this));
				}
			}
			catch (...)
			{
				done = true;
				throw;
			}
		}

		~Simple_thread_pool()
		{
			done = true;
		}

		template<typename FunctionType>
		void submit(FunctionType f)
		{
			work_queue.push(std::function<void()>(f));
		}
	};


    class Moveonly_function_wrap
    {
        struct impl_base {
            virtual void call() = 0;
            virtual ~impl_base() {}
        };

        std::unique_ptr<impl_base> impl;

        template<typename F>
        struct impl_type : impl_base
        {
            F f;
            impl_type(F&& f_) : f(std::move(f_)) {}
            void call() { f(); }
        };

    public:
        template<typename F>
        Moveonly_function_wrap(F&& f) :
            impl(new impl_type<F>(std::move(f)))
        {}

        void call() { impl->call(); }

		void operator()()
		{
			call();
		}

        Moveonly_function_wrap(Moveonly_function_wrap&& other) :
            impl(std::move(other.impl))
        {}

        Moveonly_function_wrap& operator=(Moveonly_function_wrap&& other)
        {
            impl = std::move(other.impl);
            return *this;
        }

		Moveonly_function_wrap() = default;
        Moveonly_function_wrap(const Moveonly_function_wrap&) = delete;
        Moveonly_function_wrap(Moveonly_function_wrap&) = delete;
        Moveonly_function_wrap& operator=(const Moveonly_function_wrap&) = delete;
    };

    class Thread_pool
    {
        std::atomic_bool done;
        Thread_safe_queue<Moveonly_function_wrap> work_queue;
        std::vector<std::thread> threads;
        stlex::Join_threads joiner;

        void worker_thread()
        {
            while (!done)
            {
                Moveonly_function_wrap task;
                if (work_queue.try_pop(task))
                {
                    task();
                }
                else
                {
                    std::this_thread::yield();
                }
            }
        }
    public:
        Thread_pool() :
            done(false), joiner(threads)
        {
            unsigned const thread_count = std::thread::hardware_concurrency();
            try
            {
                for (unsigned i = 0; i < thread_count; ++i)
                {
                    threads.push_back(
                        std::thread(&Thread_pool::worker_thread, this));
                }
            }
            catch (...)
            {
                done = true;
                throw;
            }
        }

        ~Thread_pool()
        {
            done = true;
        }

        template<typename FunctionType>
        std::future<typename std::result_of_t<FunctionType()>> submit(FunctionType f)
        {
            typedef std::result_of_t<FunctionType()> result_type;
            std::packaged_task<result_type()> task(std::move(f));
            std::future<result_type> res(task.get_future());
            work_queue.push(std::move(task));
            return res;
        }
    };
} // namespace rola

#endif // !ROLA_STLEX_THREAD_POOL_HPP
