#ifndef ROLA_EASY_SOCKET_UNIX_DGRAM_SOCKET_H
#define ROLA_EASY_SOCKET_UNIX_DGRAM_SOCKET_H

#ifdef unix

#include "datagram_socket.hpp"
#include "unix_address.hpp"

namespace rola
{
	/** Unix-domain datagram socket */
	using unix_datagram_socket = datagram_socket_tmpl<unix_address>;

	/** Unix-domain datagram socket (same as `unix_datagram_socket`) */
	using unix_dgram_socket = unix_datagram_socket;
} // namespace rola

#endif // unix

#endif // !ROLA_EASY_SOCKET_UNIX_DGRAM_SOCKET_H
