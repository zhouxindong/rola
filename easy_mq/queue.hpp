#ifndef ROLA_EASY_MQ_QUEUE_HPP
#define ROLA_EASY_MQ_QUEUE_HPP

#include "message.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <map>

namespace rola
{
	namespace mq
	{
		static constexpr int MSG_TIMEOUT = -1;

		class Queue
		{
		public:
			Queue();
			~Queue();

			void put(Message&& msg);

			/// <summary>
			/// Get message from the head of the queue.
			/// Blocks until at least one message is available in the queue, or until timeout happens.
			/// If get() returns due to timeout, the returned Msg will have Msg ID MSG_TIMEOUT.
			/// </summary>
			/// <param name="timeout_ms">How many ms to wait for message until timeout happens.
			/// 0 = wait indefinitely.
			/// </param>
			/// <returns></returns>
			std::unique_ptr<Message> get(int timeout_ms = 0);

			/// <summary>
			/// Make a request.
			/// Call will block until response is given with respondTo().
			/// </summary>
			/// <param name="msg">Request message. Is put to the queue so it can be retrieved from it with get().</param>
			/// <returns></returns>
			std::unique_ptr<Message> request(Message&& msg);

			/// <summary>
			/// Respond to a request previously made with request().
			/// </summary>
			/// <param name="req_uid">Msg UID of the request message.</param>
			/// <param name="response_msg">Response message. The requester will receive it as the return value of request().</param>
			void respond_to(uid_t req_uid, Message&& response_msg);
		private:
			class Queue_p;
			std::unique_ptr<Queue_p> impl_;
		};

		class Queue_p
		{
		public:
			Queue_p() = default;

			void put(Message&& msg)
			{
				{
					std::lock_guard<std::mutex> lock(q_mutex_);
				}
			}
		private:
			std::queue<std::unique_ptr<Message>> queue_;
			std::mutex q_mutex_;
			std::condition_variable q_cond_;
			std::map<uid_t, std::unique_ptr<Queue>> response_map_;
			std::mutex response_map_mutex_;
		};
	} // namespace mq;
} // namespace rola

#endif // !ROLA_EASY_MQ_QUEUE_HPP
