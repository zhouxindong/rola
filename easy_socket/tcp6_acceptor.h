#ifndef ROLA_EASY_SOCKET_TCP6_ACCEPTOR_H
#define ROLA_EASY_SOCKET_TCP6_ACCEPTOR_H

#include "acceptor.hpp"
#include "tcp6_socket.h"

namespace rola
{
	/// Class for creating a TCP v6 server.
	/// Objects of this class bind and listen on TCP ports for incoming
	/// connections. Normally, a server thread creates one of these and blocks
	/// on the call to accept incoming connections. The call to accept creates
	/// and returns a @ref tcp6_socket which can then be used for the actual
	/// communications.

	using tcp6_acceptor = acceptor_tmpl<tcp6_socket>;
} // namespace rola

#endif // ROLA_EASY_SOCKET_TCP6_ACCEPTOR_H
