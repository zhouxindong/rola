#ifndef ROLA_TCP_CONNECT_HPP
#define ROLA_TCP_CONNECT_HPP

#include <thread>
#include <chrono>
#include <type_traits>

#include "easy_socket2/socket2.hpp"
#include "easy_socket2/connected_socket.hpp"
#include "easy_socket2/stream_parser.hpp"

namespace rola
{
	template <typename StreamParser, size_t BufSize = 4096, 
		typename = std::enable_if_t<std::is_base_of<stream_parser, StreamParser>::value>>
	class tcp_send_connect
	{
	public:
		// connect process
		void operator()(sockaddr_in peer, connected_socket sock)
		{
			sock.set_non_blocking();
			running_ = true;

			result_t len = 0;
			char buf[BufSize];

			while (running_)
			{
				try
				{
					len = sock.read(buf, BufSize);
					if (len == 0) // EOF
						break;

					if (len > 0)
					{
						parser_->push(buf, len);
					}
					else
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
				}
				catch (...)
				{
					break;
				}
			}
		}

	public:
		template <typename ... Args>
		tcp_send_connect(std::function<void(std::vector<char>)> f, Args&& ... args)
		{
			parser_ = new StreamParser(std::forward<Args>(args)...);
			parser_->matched_signal().connect(f);
		}

		tcp_send_connect(const tcp_send_connect& other)
			: parser_{ other.parser_->clone() }
		{
		}

		tcp_send_connect(tcp_send_connect&& other) noexcept
			: parser_{ other.parser_ }
		{
			other.parser_ = nullptr;
		}

		tcp_send_connect& operator=(const tcp_send_connect& other)
		{
			if (this == &other)
				return *this;

			delete parser_;
			parser_ = other.parser_->clone();
			return *this;
		}

		tcp_send_connect& operator=(tcp_send_connect&& other)
		{
			if (this == &other)
				return *this;

			delete parser_;
			parser_ = other.parser_;
			other.parser_ = nullptr;
			return *this;
		}

		~tcp_send_connect()
		{
			running_ = false;

			if (parser_ != nullptr)
				delete parser_;
		}

	public:
		void stop()
		{
			running_ = false;
		}

	private:
		stream_parser* parser_{ nullptr };
		bool running_{ false };
	};
} // namespace rola

#endif // ROLA_TCP_CONNECT_HPP
