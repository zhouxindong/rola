#ifndef ROLA_EASY_SOCKET_CONNECTOR_HPP
#define ROLA_EASY_SOCKET_CONNECTOR_HPP

#include "stream_socket.hpp"
#include "sock_address.hpp"

namespace rola
{
	/**
 * Class to create a client stream connection.
 * This is a base class for creating active, streaming sockets that initiate
 * connections to a server. It can be used to derive classes that implement
 * TCP on IPv4 or IPv6.
 */
	class connector : public stream_socket
	{
		/** The base class */
		using base = stream_socket;

		// Non-copyable
		connector(const connector&) = delete;
		connector& operator=(const connector&) = delete;

	public:
		/**
		 * Creates an unconnected connector.
		 */
		connector() {}
		/**
		 * Creates the connector and attempts to connect to the specified
		 * address.
		 * @param addr The remote server address.
		 */
		connector(const sock_address& addr) { connect(addr); }
		/**
		 * Move constructor.
		 * Creates a connector by moving the other connector to this one.
		 * @param conn Another connector.
		 */
		connector(connector&& conn) : base(std::move(conn)) {}
		/**
		 * Move assignment.
		 * @param rhs The other connector to move into this one.
		 * @return A reference to this object.
		 */
		connector& operator=(connector&& rhs) {
			base::operator=(std::move(rhs));
			return *this;
		}
		/**
		 * Determines if the socket connected to a remote host.
		 * Note that this is not a reliable determination if the socket is
		 * currently connected, but rather that an initial connection was
		 * established.
		 * @return @em true If the socket connected to a remote host,
		 *  	   @em false if not.
		 */
		bool is_connected() const { return is_open(); }
		/**
		 * Attempts to connect to the specified server.
		 * If the socket is currently connected, this will close the current
		 * connection and open the new one.
		 * @param addr The remote server address.
		 * @return @em true on success, @em false on error
		 */
		bool connect(const sock_address& addr)
		{
			sa_family_t domain = addr.family();
			socket_t h = create_handle(domain);

			if (!check_ret_bool(h))
				return false;

			// This will close the old connection, if any.
			reset(h);

			if (!check_ret_bool(::connect(h, addr.sockaddr_ptr(), addr.size())))
				return close_on_err();

			return true;
		}
	};

	/////////////////////////////////////////////////////////////////////////////

	/**
	 * Class to create a client TCP connection.
	 */
	template <typename STREAM_SOCK, typename ADDR = typename STREAM_SOCK::addr_t>
	class connector_tmpl : public connector
	{
		/** The base class */
		using base = connector;

		// Non-copyable
		connector_tmpl(const connector_tmpl&) = delete;
		connector_tmpl& operator=(const connector_tmpl&) = delete;

	public:
		/** The type of streaming socket from the acceptor. */
		using stream_sock_t = STREAM_SOCK;
		/** The type of address for the connector. */
		using addr_t = ADDR;

		/**
		 * Creates an unconnected connector.
		 */
		connector_tmpl() {}
		/**
		 * Creates the connector and attempts to connect to the specified
		 * address.
		 * @param addr The remote server address.
		 */
		connector_tmpl(const addr_t& addr) : base(addr) {}
		/**
		 * Move assignment.
		 * @param rhs The other connector to move into this one.
		 * @return A reference to this object.
		 */
		connector_tmpl& operator=(connector_tmpl&& rhs) {
			base::operator=(std::move(rhs));
			return *this;
		}
		/**
		 * Gets the local address to which the socket is bound.
		 * @return The local address to which the socket is bound.
		 * @throw sys_error on error
		 */
		addr_t address() const { return addr_t(base::address()); }
		/**
		 * Gets the address of the remote peer, if this socket is connected.
		 * @return The address of the remote peer, if this socket is connected.
		 * @throw sys_error on error
		 */
		addr_t peer_address() const { return addr_t(base::peer_address()); }
		/**
		 * Binds the socket to the specified address.
		 * This call is optional for a client connector, though it is rarely
		 * used.
		 * @param addr The address to which we get bound.
		 * @return @em true on success, @em false on error
		 */
		bool bind(const addr_t& addr) { return base::bind(addr); }
		/**
		 * Attempts to connects to the specified server.
		 * If the socket is currently connected, this will close the current
		 * connection and open the new one.
		 * @param addr The remote server address.
		 * @return @em true on success, @em false on error
		 */
		bool connect(const addr_t& addr) { return base::connect(addr); }
	};
} // namespace rola

#endif // !ROLA_EASY_SOCKET_CONNECTOR_HPP
