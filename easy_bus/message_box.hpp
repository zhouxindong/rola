#ifndef ROLA_EASY_BUS_MESSAGE_BOX_HPP
#define ROLA_EASY_BUS_MESSAGE_BOX_HPP

#include <easy_socket/udp_socket.h>

#include <memory>
#include <thread>

#include "message.hpp"
#include "message_dispatcher.hpp"

namespace rola
{
	static constexpr size_t BUF_SIZE{ 4096 };
	static constexpr int MAX_SLEEP_MS{ 50 };

	// receive message box
	// the core of message bus app!
	class Message_box
	{
	public:
		Message_box(std::string const& ip, in_port_t port)
		{
			ready_ = sock_recv_.bind(inet_address(ip, port));
			if (ready_)
				sock_recv_.set_non_blocking(true);
		}

		~Message_box() noexcept
		{
			stop();
		}

	public:
		bool start()
		{
			if (!ready_)
				return false;

			t_ = std::thread([this]() {
				ssize_t recv_len;
				char buf[BUF_SIZE];
				int interval = 0;

				while (this->running_)
				{
					recv_len = this->sock_recv_.recv(buf, BUF_SIZE);
					if (recv_len > 0)
					{
						std::string msg(buf, recv_len);
						this->msg_q_.push(rebuild_message(msg));
						interval = 0;
					}
					else
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(++interval));
						if (interval > MAX_SLEEP_MS)
							interval = 0;
					}
				}
				});

			return true;
		}

		void stop()
		{
			running_ = false;
			if (t_.joinable())
				t_.join();
		}

		template <typename Message>
		ssize_t send(Message const& msg, std::string const& ip, in_port_t port)
		{
			if (!sock_send_)
				return 0;

			auto str = serialize_message(msg);
			return sock_send_.send_to(str, inet_address(ip, port));
		}

		Dispatcher wait() // create a dispatcher to handle message
		{
			return Dispatcher(&msg_q_);
		}

		bool can_send() const
		{
			return static_cast<bool>(sock_send_);
		}

		bool can_receive() const
		{
			return ready_;
		}

	private:
		Message_queue msg_q_;
		std::thread t_;

		socket_initializer sock_init_;
		udp_socket sock_recv_;
		udp_socket sock_send_;

		bool ready_{ false };
		bool running_{ true };
	};
} // namespace rola
#endif // !ROLA_EASY_BUS_MESSAGE_BOX_HPP
