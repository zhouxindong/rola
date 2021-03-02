#ifndef ROLA_EASY_BUS_MESSAGE_BOX_HPP
#define ROLA_EASY_BUS_MESSAGE_BOX_HPP

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

#include "message.hpp"
#include <easy_socket/udp_socket.h>

namespace rola
{
	// message queue
	class Message_queue
	{
		std::mutex m_;
		std::condition_variable cv_;
		std::queue<SMB_shp> q_;

	public:
		void push(SMB_shp sh)
		{
			std::lock_guard<std::mutex> lg(m_);
			q_.push(sh);
			cv_.notify_all();
		}

		template <typename Message>
		void push(Message const& msg)
		{
			push(std::make_shared<Serializable_message<Message>>(msg));
		}

		SMB_shp wait_and_pop()
		{
			std::unique_lock<std::mutex> ul(m_);
			cv_.wait(ul, [&] {return !q_.empty(); });
			auto m = q_.front();
			q_.pop();
			return m;
		}
	};

	class Message_box
	{

	};
} // namespace rola
#endif // !ROLA_EASY_BUS_MESSAGE_BOX_HPP
