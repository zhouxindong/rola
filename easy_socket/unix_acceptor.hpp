#ifndef ROLA_EASY_SOCKET_UNIX_ACCEPTOR_HPP
#define ROLA_EASY_SOCKET_UNIX_ACCEPTOR_HPP

#ifdef unix

#include "acceptor.hpp"
#include "unix_stream_socket.h"

namespace rola
{
	/// Class for creating a Unix-domain server.
/// Objects of this class bind and listen on Unix-domain ports for
/// connections. Normally, a server thread creates one of these and blocks
/// on the call to accept incoming connections. The call to accept creates
/// and returns a @ref unix_stream_socket which can then be used for the
/// actual communications.

	class unix_acceptor : public acceptor
	{
		/** The base class */
		using base = acceptor;

		// Non-copyable
		unix_acceptor(const unix_acceptor&) = delete;
		unix_acceptor& operator=(const unix_acceptor&) = delete;

	public:
		/**
		 * Creates an unconnected acceptor.
		 */
		unix_acceptor() {}
		/**
		 * Creates a acceptor and starts it listening on the specified address.
		 * @param addr The TCP address on which to listen.
		 * @param queSize The listener queue size.
		 */
		unix_acceptor(const unix_address& addr, int queSize = DFLT_QUE_SIZE) {
			open(addr, queSize);
		}
		/**
		 * Gets the local address to which we are bound.
		 * @return The local address to which we are bound.
		 */
		unix_address address() const { return unix_address(base::address()); }
		/**
		 * Base open call also work.
		 */
		using base::open;
		/**
		 * Opens the acceptor socket and binds it to the specified address.
		 * @param addr The address to which this server should be bound.
		 * @param queSize The listener queue size.
		 * @return @em true on success, @em false on error
		 */
		bool open(const unix_address& addr, int queSize = DFLT_QUE_SIZE) {
			return base::open(addr, queSize);
		}
		/**
		 * Accepts an incoming UNIX connection and gets the address of the
		 * client.
		 * @return A unix_socket to the client.
		 */
		unix_socket accept() { return unix_socket(base::accept()); }
	};

} // namespace rola

#endif // unix


#endif // !ROLA_EASY_SOCKET_UNIX_ACCEPTOR_HPP
