#ifndef ROLA_EASY_SOCKET_UDP6_SOCKET_H
#define ROLA_EASY_SOCKET_UDP6_SOCKET_H

#include "datagram_socket.hpp"
#include "inet6_address.hpp"

namespace rola
{
	/** UDP datagram socket type for IPv6 */
	using udp6_socket = datagram_socket_tmpl<inet6_address>;
} // namespace rola

#endif // !ROLA_EASY_SOCKET_UDP6_SOCKET_H
