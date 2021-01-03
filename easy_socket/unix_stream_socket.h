#ifndef ROLA_EASY_SOCKET_UNIX_STREAM_SOCKET_H
#define ROLA_EASY_SOCKET_UNIX_STREAM_SOCKET_H

#ifdef unix

#include "stream_socket.hpp"
#include "unix_address.hpp"

namespace rola
{
	/** Streaming Unix-domain socket */
	using unix_stream_socket = stream_socket_tmpl<unix_address>;

	/** Streaming Unix-domain socket (same as a `unix_stream_socket` */
	using unix_socket = unix_stream_socket;
} // namespace rola

#endif // unix

#endif // !ROLA_EASY_SOCKET_UNIX_STREAM_SOCKET_H
