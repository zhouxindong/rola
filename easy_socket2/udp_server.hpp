#ifndef ROLA_EASY_SOCKET2_UDP_SERVER_HPP
#define ROLA_EASY_SOCKET2_UDP_SERVER_HPP

#include <functional>

#include "socket2.hpp"
#include "unconnected_socket.hpp"

namespace rola
{
	class udp_server : public socket2
	{
	public:
		udp_server(const char* hname, const char* sname)
		{
			socket2::set_address(hname, sname, &local_);
			pre_build();
		}

		udp_server(const char* hname, short port)
		{
			socket2::set_address(hname, port, &local_);
			pre_build();
		}

		udp_server(const char* sname)
		{
			socket2::set_address(nullptr, sname, &local_);
			pre_build();
		}

		udp_server(short port)
		{
			socket2::set_address(nullptr, port, &local_);
			pre_build();
		}

		udp_server(const udp_server&)				= delete;
		udp_server& operator=(const udp_server&)	= delete;
		udp_server(udp_server&&)					= delete;
		udp_server& operator=(udp_server&&)			= delete;

	public:
		void start(std::function<void(unconnected_socket)> f)
		{
			f(unconnected_socket{ clone()/*.handle()*/ });
		}

	private:
		void pre_build()
		{
			create_socket(SOCK_DGRAM);
			//set_reuse();
			bindto(&local_);
		}

	private:
		sockaddr_in local_;
		bool running_{ true };
	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_UDP_SERVER_HPP
