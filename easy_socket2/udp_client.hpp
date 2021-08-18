#ifndef ROLA_EASY_SOCKET2_UDP_CLIENT_HPP
#define ROLA_EASY_SOCKET2_UDP_CLIENT_HPP

#include <functional>

#include "socket2.hpp"
#include "connected_socket.hpp"
#include "unconnected_socket.hpp"

namespace rola
{
	class udp_client : public socket2
	{
		enum class connect_category
		{
			connected,
			unconnected,
		};

	public:
		udp_client(const char* hname, const char* sname)
			: category_{ connect_category::connected }
		{
			socket2::set_address(hname, sname, &peer_);
			pre_build();
		}

		udp_client(const char* hname, short port)
			: category_{ connect_category::connected }
		{
			socket2::set_address(hname, port, &peer_);
			pre_build();
		}

		udp_client()
			: category_{ connect_category::unconnected }
		{
			create_socket(SOCK_DGRAM);
		}

		udp_client(const udp_client&)				= delete;
		udp_client(udp_client&&)					= delete;
		udp_client& operator=(const udp_client&)	= delete;
		udp_client& operator=(udp_client&&)			= delete;

	public:
		void conn_proc(std::function<void(connected_socket)> f)
		{
			if (category_ != connect_category::connected)
				throw socket_exception(socket_exception_source::Category);

			f(connected_socket{ clone()/*.handle()*/ });
		}

		void unconn_proc(std::function<void(unconnected_socket)> f)
		{
			if (category_ != connect_category::unconnected)
				throw socket_exception(socket_exception_source::Category);

			f(unconnected_socket{ clone()/*.handle() */});
		}

	private:
		void pre_build()
		{
			create_socket(SOCK_DGRAM);

			if (::connect(handle(), reinterpret_cast<const sockaddr*>(&peer_), sizeof(peer_)) != 0)
				throw socket_exception(socket_exception_source::Connect);
		}

	private:
		sockaddr_in peer_;
		connect_category category_;
	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_UDP_CLIENT_HPP
