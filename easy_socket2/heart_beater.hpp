#ifndef ROLA_EASY_SOCKET2_HEART_SERVER_HPP
#define ROLA_EASY_SOCKET2_HEART_SERVER_HPP

#include <string>
#include <unordered_set>
#include <memory>
#include <mutex>

#include "easy_socket2/platform2.h"
#include "easy_socket2/socket_init.hpp"
#include "easy_socket2/tcp_server.hpp"

namespace rola
{
	class endpoint
	{
	public:
		endpoint(sockaddr_in addr, socket_t s)
		{
			conn_s_ = std::make_shared<connected_socket>(s);
			conn_s_->set_non_blocking();

			char buf[INET_ADDRSTRLEN];
			ip_ = ::inet_ntop(AF_INET, (void*)&(addr.sin_addr), buf, INET_ADDRSTRLEN);
			port_ = ::ntohs(addr.sin_port);
		}

	public:
		const std::string ip() const
		{
			return ip_;
		}

		const short port() const
		{
			return port_;
		}

	private:
		std::shared_ptr<connected_socket> conn_s_;

	private:
		std::string ip_;
		short port_;
	};

	inline bool operator==(const endpoint& ep1, const endpoint& ep2)
	{
		return ep1.ip() == ep2.ip() && ep1.port() == ep2.port();
	}
} // namespace rola

namespace std
{
	template <>
	struct hash<rola::endpoint>
	{
		std::size_t operator()(const rola::endpoint& value) const
		{
			return hash<std::string>{}(value.ip()) ^ hash<short>{}(value.port()); // call operator()
		}
	};
} // namespace std

namespace rola
{
	class heart_server
	{
	public:
		heart_server(const char* ip, short port)
			: server_{ ip, port }
		{
			start_server_();
		}

		heart_server(short port)
			: server_{ port }
		{
			start_server_();
		}

		~heart_server()
		{
			server_.stop();
			if (listen_thread_.joinable())
				listen_thread_.join();
		}

	private:
		void start_server_()
		{
			server_.accept_signal().connect([this](sockaddr_in addr, socket_t s) {
				std::lock_guard<std::mutex> lg(m_);
				endpoints_.emplace(addr, s);
				});
			listen_thread_ = std::thread(&tcp_server::start_signal, &server_, false);
		}

		void beat_()
		{
			beat_thread_ = std::thread([this]() {
				long long count = 0;

				});
		}

	private:
		socket_init sock_init_;
		tcp_server server_;
		std::thread listen_thread_;
		std::thread beat_thread_;
		std::mutex m_;

	private:
		std::unordered_set<endpoint> endpoints_;
	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_HEART_SERVER_HPP
