#ifndef ROLA_EASY_SOCKET2_BROADCAST_SEND_HPP
#define ROLA_EASY_SOCKET2_BROADCAST_SEND_HPP

#include "socket2.hpp"

namespace rola
{
	class broadcast_send : public socket2
	{
	public:
		broadcast_send(const char* port)
		{
			socket2::set_address("255.255.255.255", port, &broad_ip_);
		}

		broadcast_send(short port)
		{
			socket2::set_address("255.255.255.255", port, &broad_ip_);
		}

		broadcast_send(const broadcast_send&)				= delete;
		broadcast_send(broadcast_send&&)					= delete;
		broadcast_send& operator=(const broadcast_send&)	= delete;
		broadcast_send& operator=(broadcast_send&&)			= delete;

	public:
		result_t write(const void* buf, iolen_t n) noexcept
		{
			return detail::send_to(this->handle(), buf, n, &broad_ip_);
		}

	private:
		void pre_build()
		{
			create_socket(SOCK_DGRAM);

			int so_brd = 1;
			if (!set_option(SOL_SOCKET, SO_BROADCAST, so_brd))
				throw socket_exception(socket_exception_source::Broadcast);
		}

	private:
		sockaddr_in broad_ip_;
	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_BROADCAST_SEND_HPP
