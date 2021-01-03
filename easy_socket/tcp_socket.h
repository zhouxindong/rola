#ifndef ROLA_EASY_SOCKET_TCP_SOCKET_HPP
#define ROLA_EASY_SOCKET_TCP_SOCKET_HPP

#include "stream_socket.hpp"
#include "inet_address.hpp"

namespace rola
{
	/** IPv4 streaming TCP socket */
	using tcp_socket = stream_socket_tmpl<inet_address>;
} // namespace rola

#endif // !ROLA_EASY_SOCKET_TCP_SOCKET_HPP
