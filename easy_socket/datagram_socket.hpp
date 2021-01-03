#ifndef ROLA_EASY_SOCKET_DATAGRAM_SOCKET_HPP
#define ROLA_EASY_SOCKET_DATAGRAM_SOCKET_HPP

#include "socket.hpp"
#include "exception.hpp"
#include <algorithm>

namespace rola
{
	/**
 * Base class for datagram sockets.
 *
 * Datagram sockets are normally connectionless, where each packet is
 * individually routed and delivered.
 */
	class datagram_socket : public socket
	{
		/** The base class   */
		using base = socket;

		// Non-copyable
		datagram_socket(const datagram_socket&) = delete;
		datagram_socket& operator=(const datagram_socket&) = delete;

	protected:
		static socket_t create_handle(int domain) {
			return socket_t(::socket(domain, COMM_TYPE, 0));
		}

	public:
		/** The socket 'type' for communications semantics. */
		static constexpr int COMM_TYPE = SOCK_DGRAM;

		/**
		 * Creates an uninitialized datagram socket.
		 */
		datagram_socket() {}
		/**
		 * Creates a datagram socket from an existing OS socket handle and
		 * claims ownership of the handle.
		 * @param handle A socket handle from the operating system.
		 */
		explicit datagram_socket(socket_t handle) : base(handle) {}
		/**
		 * Creates a UDP socket and binds it to the address.
		 * @param addr The address to bind.
		 */
		explicit datagram_socket(const sock_address& addr)
		{
			auto domain = addr.family();
			socket_t h = create_handle(domain);

			if (check_socket_bool(h)) {
				reset(h);
				bind(addr);
			}
		}
		/**
		 * Move constructor.
		 * @param other The other socket to move to this one
		 */
		datagram_socket(datagram_socket&& other) noexcept
			: base(std::move(other)) {}
		/**
		 * Move assignment.
		 * @param rhs The other socket to move into this one.
		 * @return A reference to this object.
		 */
		datagram_socket& operator=(datagram_socket&& rhs) noexcept {
			base::operator=(std::move(rhs));
			return *this;
		}
		/**
		 * Connects the socket to the remote address.
		 * In the case of datagram sockets, this does not create an actual
		 * connection, but rather specifies the address to which datagrams are
		 * sent by default and the only address from which packets are
		 * received.
		 * @param addr The address on which to "connect".
		 * @return @em true on success, @em false on failure
		 */
		bool connect(const sock_address& addr) {
			return check_ret_bool(::connect(handle(), addr.sockaddr_ptr(),
				addr.size()));
		}

		// ----- I/O -----

		/**
		 * Sends a message to the socket at the specified address.
		 * @param buf The data to send.
		 * @param n The number of bytes in the data buffer.
		 * @param flags The flags. See send(2).
		 * @param addr The remote destination of the data.
		 * @return the number of bytes sent on success or, @em -1 on failure.
		 */
		ssize_t send_to(const void* buf, size_t n, int flags, const sock_address& addr) {
#if defined(_WIN32)
			return check_ret(::sendto(handle(), reinterpret_cast<const char*>(buf), int(n),
				flags, addr.sockaddr_ptr(), addr.size()));
#else
			return check_ret(::sendto(handle(), buf, n, flags,
				addr.sockaddr_ptr(), addr.size()));
#endif
		}
		/**
		 * Sends a string to the socket at the specified address.
		 * @param s The string to send.
		 * @param flags The flags. See send(2).
		 * @param addr The remote destination of the data.
		 * @return the number of bytes sent on success or, @em -1 on failure.
		 */
		ssize_t send_to(const std::string& s, int flags, const sock_address& addr) {
			return send_to(s.data(), s.length(), flags, addr);
		}
		/**
		 * Sends a message to another socket.
		 * @param buf The data to send.
		 * @param n The number of bytes in the data buffer.
		 * @param addr The remote destination of the data.
		 * @return the number of bytes sent on success or, @em -1 on failure.
		 */
		ssize_t send_to(const void* buf, size_t n, const sock_address& addr) {
			return send_to(buf, n, 0, addr);
		}
		/**
		 * Sends a string to another socket.
		 * @param s The string to send.
		 * @param addr The remote destination of the data.
		 * @return the number of bytes sent on success or, @em -1 on failure.
		 */
		ssize_t send_to(const std::string& s, const sock_address& addr) {
			return send_to(s.data(), s.length(), 0, addr);
		}
		/**
		 * Sends a message to the socket at the default address.
		 * The socket should be connected before calling this.
		 * @param buf The date to send.
		 * @param n The number of bytes in the data buffer.
		 * @param flags The option bit flags. See send(2).
		 * @return @em zero on success, @em -1 on failure.
		 */
		ssize_t send(const void* buf, size_t n, int flags = 0) {
#if defined(_WIN32)
			return check_ret(::send(handle(), reinterpret_cast<const char*>(buf),
				int(n), flags));
#else
			return check_ret(::send(handle(), buf, n, flags));
#endif
		}
		/**
		 * Sends a string to the socket at the default address.
		 * The socket should be connected before calling this
		 * @param s The string to send.
		 * @param flags The option bit flags. See send(2).
		 * @return @em zero on success, @em -1 on failure.
		 */
		ssize_t send(const std::string& s, int flags = 0) {
			return send(s.data(), s.length(), flags);
		}
		/**
		 * Receives a message on the socket.
		 * @param buf Buffer to get the incoming data.
		 * @param n The number of bytes to read.
		 * @param flags The option bit flags. See send(2).
		 * @param srcAddr Receives the address of the peer that sent the
		 *  			   message
		 * @return The number of bytes read or @em -1 on error.
		 */
		ssize_t recv_from(void* buf, size_t n, int flags, sock_address* srcAddr = nullptr)
		{
			sockaddr* p = srcAddr ? srcAddr->sockaddr_ptr() : nullptr;
			socklen_t len = srcAddr ? srcAddr->size() : 0;

			// TODO: Check returned length
#if defined(_WIN32)
			return check_ret(::recvfrom(handle(), reinterpret_cast<char*>(buf),
				int(n), flags, p, &len));
#else
			return check_ret(::recvfrom(handle(), buf, n, flags, p, &len));
#endif
		}
		/**
		 * Receives a message on the socket.
		 * @param buf Buffer to get the incoming data.
		 * @param n The number of bytes to read.
		 * @param srcAddr Receives the address of the peer that sent the
		 *  			   message
		 * @return The number of bytes read or @em -1 on error.
		 */
		ssize_t recv_from(void* buf, size_t n, sock_address* srcAddr = nullptr) {
			return recv_from(buf, n, 0, srcAddr);
		}
		/**
		 * Receives a message on the socket.
		 * @param buf Buffer to get the incoming data.
		 * @param n The number of bytes to read.
		 * @param flags The option bit flags. See send(2).
		 * @return The number of bytes read or @em -1 on error.
		 */
		ssize_t recv(void* buf, size_t n, int flags = 0) {
#if defined(_WIN32)
			return check_ret(::recv(handle(), reinterpret_cast<char*>(buf),
				int(n), flags));
#else
			return check_ret(::recv(handle(), buf, n, flags));
#endif
		}
	};

	/////////////////////////////////////////////////////////////////////////////

	/**
	 * Base class for datagram sockets.
	 *
	 * Datagram sockets are normally connectionless, where each packet is
	 * individually routed and delivered.
	 */
	template <typename ADDR>
	class datagram_socket_tmpl : public datagram_socket
	{
		/** The base class */
		using base = datagram_socket;

	public:
		/** The address family for this type of address */
		static constexpr sa_family_t ADDRESS_FAMILY = ADDR::ADDRESS_FAMILY;
		/** The type of address for the socket. */
		using addr_t = ADDR;

		/**
		 * Creates an unbound datagram socket.
		 * This can be used as a client or later bound as a server socket.
		 */
		datagram_socket_tmpl() : base(create_handle(ADDRESS_FAMILY)) {}
		/**
		 * Creates a datagram socket from an existing OS socket handle and
		 * claims ownership of the handle.
		 * @param handle A socket handle from the operating system.
		 */
		datagram_socket_tmpl(socket_t handle) : base(handle) {}
		/**
		 * Creates a UDP socket and binds it to the address.
		 * @param addr The address to bind.
		 */
		datagram_socket_tmpl(const ADDR& addr) : base(addr) {}
		/**
		 * Move constructor.
		 * @param other The other socket to move to this one
		 */
		datagram_socket_tmpl(datagram_socket_tmpl&& other)
			: base(std::move(other)) {}
		/**
		 * Move assignment.
		 * @param rhs The other socket to move into this one.
		 * @return A reference to this object.
		 */
		datagram_socket_tmpl& operator=(datagram_socket_tmpl&& rhs) {
			base::operator=(std::move(rhs));
			return *this;
		}

		/**
		 * Creates a pair of connected stream sockets.
		 *
		 * Whether this will work at all is highly system and domain dependent.
		 * Currently it is only known to work for Unix-domain sockets on *nix
		 * systems.
		 *
		 * @param protocol The protocol to be used with the socket. (Normally 0)
		 *
		 * @return A std::tuple of stream sockets. On error both sockets will be
		 *  	   in an error state with the last error set.
		 */
		static std::tuple<datagram_socket_tmpl, datagram_socket_tmpl> pair(int protocol = 0) {
			auto pr = base::pair(addr_t::ADDRESS_FAMILY, COMM_TYPE, protocol);
			return std::make_tuple<datagram_socket_tmpl, datagram_socket_tmpl>(
				datagram_socket_tmpl{ std::get<0>(pr).release() },
				datagram_socket_tmpl{ std::get<1>(pr).release() });
		}
		/**
		 * Binds the socket to the local address.
		 * Datagram sockets can bind to a local address/adapter to filter which
		 * incoming packets to receive.
		 * @param addr The address on which to bind.
		 * @return @em true on success, @em false on failure
		 */
		bool bind(const ADDR& addr) { return base::bind(addr); }
		/**
		 * Connects the socket to the remote address.
		 * In the case of datagram sockets, this does not create an actual
		 * connection, but rather specifies the address to which datagrams are
		 * sent by default and the only address from which packets are
		 * received.
		 * @param addr The address on which to "connect".
		 * @return @em true on success, @em false on failure
		 */
		bool connect(const ADDR& addr) { return base::connect(addr); }

		// ----- I/O -----

		/**
		 * Sends a message to the socket at the specified address.
		 * @param buf The data to send.
		 * @param n The number of bytes in the data buffer.
		 * @param flags The option bit flags. See send(2).
		 * @param addr The remote destination of the data.
		 * @return the number of bytes sent on success or, @em -1 on failure.
		 */
		ssize_t send_to(const void* buf, size_t n, int flags, const ADDR& addr) {
			return base::send_to(buf, n, flags, addr);
		}
		/**
		 * Sends a string to the socket at the specified address.
		 * @param s The string to send.
		 * @param flags The flags. See send(2).
		 * @param addr The remote destination of the data.
		 * @return the number of bytes sent on success or, @em -1 on failure.
		 */
		ssize_t send_to(const std::string& s, int flags, const ADDR& addr) {
			return base::send_to(s, flags, addr);
		}
		/**
		 * Sends a message to another socket.
		 * @param buf The data to send.
		 * @param n The number of bytes in the data buffer.
		 * @param addr The remote destination of the data.
		 * @return the number of bytes sent on success or, @em -1 on failure.
		 */
		ssize_t send_to(const void* buf, size_t n, const ADDR& addr) {
			return base::send_to(buf, n, 0, addr);
		}
		/**
		 * Sends a string to another socket.
		 * @param s The string to send.
		 * @param addr The remote destination of the data.
		 * @return the number of bytes sent on success or, @em -1 on failure.
		 */
		ssize_t send_to(const std::string& s, const ADDR& addr) {
			return base::send_to(s, addr);
		}
		/**
		 * Receives a message on the socket.
		 * @param buf Buffer to get the incoming data.
		 * @param n The number of bytes to read.
		 * @param flags The option bit flags. See send(2).
		 * @param srcAddr Receives the address of the peer that sent
		 *  			the message
		 * @return The number of bytes read or @em -1 on error.
		 */
		ssize_t recv_from(void* buf, size_t n, int flags, ADDR* srcAddr) {
			return base::recv_from(buf, n, flags, srcAddr);
		}
		/**
		 * Receives a message on the socket.
		 * @param buf Buffer to get the incoming data.
		 * @param n The number of bytes to read.
		 * @param srcAddr Receives the address of the peer that sent
		 *  			the message
		 * @return The number of bytes read or @em -1 on error.
		 */
		ssize_t recv_from(void* buf, size_t n, ADDR* srcAddr = nullptr) {
			return base::recv_from(buf, n, srcAddr);
		}
	};
} // namespace rola

#endif // !ROLA_EASY_SOCKET_DATAGRAM_SOCKET_HPP
