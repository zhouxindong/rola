#ifndef ROLA_EASY_ACTOR_BLOCK_QUEUE_HPP
#define ROLA_EASY_ACTOR_BLOCK_QUEUE_HPP

#include <iostream>
#include <queue>
#include <atomic>

#include "condition.hpp"

namespace rola
{
	namespace actor
	{
		template <typename Type, typename LockType>
		class Block_queue
		{
		private:
			LockType lock_;
			Condition condition_;
			std::atomic<bool> waited_;
			std::queue<Type> queue_;

		public:
			Block_queue()
				: waited_(false)
			{}

			void push(Type& ele)
			{
				{
					std::unique_lock<LockType> lock(lock_);
					queue_.push(ele);
				}
				if (waited_)
				{
					condition_.notify_all();
				}
			}

			template <typename... _Types>
			void push(_Types&&... _Args)
			{
				{
					std::unique_lock<LockType> lock(lock_);
					queue_.emplace(std::forward<_Types>(_Args)...);
				}
				if (waited_)
					condition_.notify_all();
			}

			Type pop()
			{
				do
				{
					{
						std::unique_lock<LockType> lock(lock_);
						if (!queue_.empty())
						{
							auto& ele = queue_.front();
							queue_.pop();
							return ele;
						}
					}
					
					condition_.wait([this]()
						{
							waited_ = empty();
							return !waited_;
						});
				} while (true);
			}

			bool empty()
			{
				std::unique_lock<LockType> lock(lock_);
				return queue_.empty();
			}

			void clear()
			{
				std::unique_lock<LockType> lock(lock_);
				std::queue<Type> empty;
				queue_.swap(empty);
			}
		};
	} // namespace actor
} // namespace rola

#endif // !ROLA_EASY_ACTOR_BLOCK_QUEUE_HPP
