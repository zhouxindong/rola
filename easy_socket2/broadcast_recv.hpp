#ifndef ROLA_EASY_SOCKET2_BROADCAST_RECV_HPP
#define ROLA_EASY_SOCKET2_BROADCAST_RECV_HPP

#include "socket2.hpp"

namespace rola
{
	class broadcast_recv : public socket2
	{
	public:
		broadcast_recv(const char* port)
		{
			socket2::set_address(nullptr, port, &b_addr_);
			pre_build();
		}

		broadcast_recv(short port)
		{
			socket2::set_address(nullptr, port, &b_addr_);
			pre_build();
		}

		broadcast_recv(const broadcast_recv&)				= delete;
		broadcast_recv(broadcast_recv&&)					= delete;
		broadcast_recv& operator=(const broadcast_recv&)	= delete;
		broadcast_recv& operator=(broadcast_recv&&)			= delete;

	public:
		result_t read(void* buf, iolen_t n) noexcept
		{
			return detail::recv(this->handle(), buf, n);
		}

	private:
		void pre_build()
		{
			create_socket(SOCK_DGRAM);
			//set_reuse();
			bindto(&b_addr_);
		}
	private:
		sockaddr_in b_addr_;
	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_BROADCAST_RECV_HPP
