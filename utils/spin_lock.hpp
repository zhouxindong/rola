#ifndef ROLA_EASY_ACTOR_SPIN_LOCK_HPP
#define ROLA_EASY_ACTOR_SPIN_LOCK_HPP

#include <atomic>
#include <thread>

namespace rola
{
	namespace actor
	{
		class Spin_lock
		{
		private:
			std::atomic<bool> flag_;

		public:
			Spin_lock()
				: flag_(false) {}

			void lock()
			{
				bool expect = false;
				while (!flag_.compare_exchange_weak(expect, true))
				{
					expect = false;
				}
			}
			void unlock()
			{
				flag_.store(false);
			}
		};
	} // namespace actor
} // namespace rola

#endif // !ROLA_EASY_ACTOR_SPIN_LOCK_HPP
