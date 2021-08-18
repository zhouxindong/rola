#ifndef ROLA_EASY_SOCKET_UDP_SOCKET_H
#define ROLA_EASY_SOCKET_UDP_SOCKET_H

#include "datagram_socket.hpp"
#include "inet_address.hpp"

namespace rola
{
	/** UDP datagram socket type for IPv4 */
	using udp_socket = datagram_socket_tmpl<inet_address>;
} // namespace rola

#endif // !ROLA_EASY_SOCKET_UDP_SOCKET_H
