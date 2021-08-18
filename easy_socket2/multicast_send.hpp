#ifndef ROLA_EASY_SOCKET2_MULTICAST_SEND_HPP
#define ROLA_EASY_SOCKET2_MULTICAST_SEND_HPP

#include "socket2.hpp"

namespace rola
{
	class multicast_send : public socket2
	{
	public:
		multicast_send(const char* mul_ip, const char* mul_port, int ttl = 64)
		{
			socket2::set_address(mul_ip, mul_port, &mul_addr_);
			pre_build(ttl);
		}

		multicast_send(const char* mul_ip, short port, int ttl = 64)
		{
			socket2::set_address(mul_ip, port, &mul_addr_);
			pre_build(ttl);
		}

		multicast_send(const multicast_send&)				= delete;
		multicast_send(multicast_send&&)					= delete;
		multicast_send& operator=(const multicast_send&)	= delete;
		multicast_send& operator=(multicast_send&&)			= delete;

	public:
		result_t write(const void* buf, iolen_t n) noexcept
		{
			return detail::send_to(this->handle(), buf, n, &mul_addr_);
		}

		result_t write(const std::string& str) noexcept
		{
			return detail::send_to(this->handle(), str, &mul_addr_);
		}

	private:
		void pre_build(int ttl)
		{
			create_socket(SOCK_DGRAM);

			if (!set_option(IPPROTO_IP, IP_MULTICAST_TTL, ttl))
				throw socket_exception(socket_exception_source::TTL);
		}

	private:
		sockaddr_in mul_addr_;
	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_MULTICAST_SEND_HPP
