#ifndef ROLA_EASY_ACTOR_THREAD_POOL_HPP
#define ROLA_EASY_ACTOR_THREAD_POOL_HPP

#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <memory>

namespace rola
{
	namespace actor
	{
		class Thread
		{
		public:
			Thread()
				: Thread(nullptr)
			{}

			Thread(std::function<void()> callback)
				: started_(false)
				, run_(callback)
				, thread_(std::bind(&Thread::entry_point, this))
			{}

			virtual ~Thread()
			{}

		public:
			void start()
			{
				std::unique_lock<std::mutex> lock(mutex_);
				started_ = true;
				condition_.notify_one();
			}

			virtual void stop() {}

			bool started()
			{
				return started_;
			}

			void join()
			{
				thread_.join();
			}

			void detach()
			{
				thread_.detach();
			}

			bool is_run_in_this_thread()
			{
				return current_thread_id() == id_;
			}

			std::thread::id thread_id()
			{
				return id_;
			}

			void register_process(std::function<void()> func)
			{
				run_ = func;
			}

		public:
			static void yield_current_thread()
			{
				std::this_thread::yield();
			}

			static std::thread::id current_thread_id()
			{
				return std::this_thread::get_id();
			}

			static void sleep_ms(uint64_t ms)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(ms));
			}

		protected:
			bool started_;
			std::mutex mutex_;
			std::condition_variable condition_;

		private:
			std::function<void()> run_;
			std::thread::id id_;
			std::thread thread_;

			void entry_point()
			{
				id_ = current_thread_id();
				{
					std::unique_lock<std::mutex> lock(mutex_);
					condition_.wait(lock, [this] {return started_; });
				}
				if (run_)
					run_();
			}
		};

		class Thread_pool
		{
		public:
			Thread_pool() {}
			virtual ~Thread_pool() {}

		public:
			void register_process(std::function<void()> process)
			{
				processor_ = process;
			}
			void start(uint32_t cnt)
			{
				for (uint32_t i = 0; i < cnt; ++i)
				{
					std::shared_ptr<Thread> = std::make_shared<Thread>(
						std::bind(&Thread_pool::run, this)
						);
					threads_.push_back(ptr);
					ptr->start();
				}
			}
			void join()
			{
				for (auto thread : threads_)
				{
					thread->join();
				}
			}

		private:
			std::function<void()> processor_;
			std::vector<std::shared_ptr<Thread>> threads_;

			void run()
			{
				while (true)
				{
					processor_();
				}
			}
		};
	} // namespace actor
} // namespace rola

#endif // !ROLA_EASY_ACTOR_THREAD_POOL_HPP
