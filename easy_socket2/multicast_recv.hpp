#ifndef ROLA_EASY_SOCKET2_MULTICAST_RECV_HPP
#define ROLA_EASY_SOCKET2_MULTICAST_RECV_HPP

#include "socket2.hpp"

namespace rola
{
	class multicast_recv : public socket2
	{
	public:
		multicast_recv(const char* mul_ip, const char* port, const char* local_ip = nullptr)
		{
			socket2::set_address(local_ip, port, &addr_);
			pre_build(mul_ip, local_ip);
		}

		multicast_recv(const char* mul_ip, short port, const char* local_ip = nullptr)
		{
			socket2::set_address(local_ip, port, &addr_);
			pre_build(mul_ip, local_ip);
		}

		multicast_recv(const multicast_recv&)				= delete;
		multicast_recv(multicast_recv&&)					= delete;
		multicast_recv& operator=(const multicast_recv&)	= delete;
		multicast_recv& operator=(multicast_recv&&)			= delete;

	public:
		result_t read(void* buf, iolen_t n) noexcept
		{
			return detail::recv(this->handle(), buf, n);
		}

	private:
		void pre_build(const char* mul_ip, const char* local_ip)
		{
			create_socket(SOCK_DGRAM);
			//set_reuse();
			bindto(&addr_);

#if defined(_WIN32)
			join_addr_.imr_multiaddr.S_un.S_addr = inet_addr(mul_ip);
			if (local_ip)
				join_addr_.imr_interface.S_un.S_addr = inet_addr(local_ip);
			else
				join_addr_.imr_interface.S_un.S_addr = htonl(INADDR_ANY);
#else
			join_addr_.imr_multiaddr.s_addr = inet_addr(mul_ip);
			if (local_ip)
				join_addr_.imr_interface.s_addr = inet_addr(local_ip);
			else
				join_addr_.imr_interface.s_addr = htonl(INADDR_ANY);
#endif			
			if (!set_option(IPPROTO_IP, IP_ADD_MEMBERSHIP, join_addr_))
				throw socket_exception(socket_exception_source::AddMembership);
		}

	private:
		sockaddr_in addr_;
		ip_mreq join_addr_;
	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_MULTICAST_RECV_HPP
