#ifndef ROLA_EASY_SOCKET_TCP6_SOCKET_H
#define ROLA_EASY_SOCKET_TCP6_SOCKET_H

#include "stream_socket.hpp"
#include "inet6_address.hpp"

namespace rola
{
	/** IPv6 streaming TCP socket */
	using tcp6_socket = stream_socket_tmpl<inet6_address>;
} // namespace rola

#endif // !ROLA_EASY_SOCKET_TCP6_SOCKET_H
