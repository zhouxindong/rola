#ifndef ROLA_EASY_SOCKET_UNIX_CONNECTOR_H
#define ROLA_EASY_SOCKET_UNIX_CONNECTOR_H

#ifdef unix

#include "connector.hpp"
#include "unix_stream_socket.h"

namespace rola
{
	/** Unix-domain active connector socket. */
	using unix_connector = connector_tmpl<unix_socket, unix_address>;
} // namespace rola

#endif // unix


#endif // !ROLA_EASY_SOCKET_UNIX_CONNECTOR_H
