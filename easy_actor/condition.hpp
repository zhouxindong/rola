#ifndef ROLA_EASY_ACTOR_CONDITION_HPP
#define ROLA_EASY_ACTOR_CONDITION_HPP

#include <mutex>
#include <condition_variable>
#include <functional>

namespace rola
{
	namespace actor
	{
		class Condition
		{
		public:
			void notify_all()
			{
				std::unique_lock<std::mutex> lock(mutex_);
				condition_.notify_all();
			}

			void notify()
			{
				std::unique_lock<std::mutex> lock(mutex_);
				condition_.notify_one();
			}

			void wait(std::function<bool()> callback)
			{
				std::unique_lock<std::mutex> lock(mutex_);
				condition_.wait(lock, callback);
			}

		private:
			std::mutex mutex_;
			std::condition_variable condition_;
		};
	} // namespace actor
} // namespace rola

#endif // !ROLA_EASY_ACTOR_CONDITION_HPP
