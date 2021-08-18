#ifndef ROLA_EASY_SOCKET2_TCP_CLIENT_HPP
#define ROLA_EASY_SOCKET2_TCP_CLIENT_HPP

#include <functional>

#include "socket2.hpp"
#include "connected_socket.hpp"

namespace rola
{
	class tcp_client : public socket2
	{
	public:
		tcp_client(const char* hname, const char* sname)
		{
			socket2::set_address(hname, sname, &peer_);
			pre_build();
		}

		tcp_client(const char* hname, short port)
		{
			socket2::set_address(hname, port, &peer_);
			pre_build();
		}

		tcp_client(const char* hname, short port, std::function<void(tcp_client&)> f)
		{
			socket2::set_address(hname, port, &peer_);
			pre_build(f);
		}

		tcp_client(const tcp_client&)				= delete;
		tcp_client(tcp_client&&)					= delete;
		tcp_client& operator=(const tcp_client&)	= delete;
		tcp_client& operator=(tcp_client&&)			= delete;

	public:
		void conn_proc(std::function<void(sockaddr_in peer, connected_socket)> f)
		{
			f(peer_, connected_socket{ clone()/*.handle()*/ });
		}

	private:
		void pre_build()
		{
			create_socket(SOCK_STREAM);

			if (::connect(handle(), reinterpret_cast<const sockaddr*>(&peer_), sizeof(peer_)) != 0)
				throw socket_exception(socket_exception_source::Connect);
		}

		void pre_build(std::function<void(tcp_client&)> f)
		{
			create_socket(SOCK_STREAM);
			f(*this);

			if (::connect(handle(), reinterpret_cast<const sockaddr*>(&peer_), sizeof(peer_)) != 0)
				throw socket_exception(socket_exception_source::Connect);
		}

	private:
		sockaddr_in peer_;
	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_TCP_CLIENT_HPP
