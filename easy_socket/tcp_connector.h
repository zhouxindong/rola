#ifndef ROLA_EASY_SOCKET_TCP_CONNECTOR_H
#define ROLA_EASY_SOCKET_TCP_CONNECTOR_H

#include "connector.hpp"
#include "tcp_socket.h"

namespace rola
{
	/** IPv4 active, connector (client) socket. */
	using tcp_connector = connector_tmpl<tcp_socket>;
} // namespace rola

#endif // !ROLA_EASY_SOCKET_TCP_CONNECTOR_H
