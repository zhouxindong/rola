#ifndef ROLA_EASY_SOCKET_INET6_ADDRESS_HPP
#define ROLA_EASY_SOCKET_INET6_ADDRESS_HPP

#include "platform.h"
#include "sock_address.hpp"
#include <iostream>
#include <string>
#include <cstring>

namespace rola
{
	/**
 * Class that represents an internet (IPv6) address. This inherits from the
 * IP-specific form of a socket address, @em sockaddr_in.
 */
	class inet6_address : public sock_address
	{
		/** The underlying C IPv6 struct  */
		sockaddr_in6 addr_;

		/** The size of the underlying address struct, in bytes */
		static constexpr size_t SZ = sizeof(sockaddr_in6);

	public:
		/** The address family for this type of address */
		static constexpr sa_family_t ADDRESS_FAMILY = AF_INET6;

		/**
		 * Constructs an empty address.
		 * The address is initialized to all zeroes.
		 */
		inet6_address() : addr_() {}
		/**
		 * Constructs an address for any iface using the specified port.
		 * This is a convenient way for a server to specify an address that will
		 * bind to all interfaces.
		 * @param port The port number in native/host byte order.
		 */
		explicit inet6_address(in_port_t port) {
			const in6_addr ANY IN6ADDR_ANY_INIT;
			create(ANY, port);
		}
		/**
		 * Constructs an address using the name of the host and the specified
		 * port. This attempts to resolve the host name to an address.
		 *
		 * @param saddr The name of the host.
		 * @param port The port number in native/host byte order.
		 */
		inet6_address(const std::string& saddr, in_port_t port) {
			create(saddr, port);
		}
		/**
		 * Constructs the address by copying the specified structure.
		 * TODO: Do we actually need a conversion from something that's
		 * cast to a sockaddr, but is really a sockaddr_in6?
		 * @param addr The other address
		 */
		inet6_address(const sockaddr_storage& addr) {
			std::memcpy(&addr_, &addr, SZ);
		}
		/**
		 * Constructs the address by copying the specified structure.
		 * @param addr The other address
		 */
		inet6_address(const sock_address& addr) {
			std::memcpy(&addr_, addr.sockaddr_ptr(), SZ);
		}
		/**
		 * Constructs the address by copying the specified structure.
		 * @param addr The other address
		 */
		inet6_address(const sockaddr_in6& addr) {
			std::memcpy(&addr_, &addr, SZ);
		}
		/**
		 * Constructs the address by copying the specified address.
		 * @param addr The other address
		 */
		inet6_address(const inet6_address& addr) : addr_(addr.addr_) {}
		/**
		 * Creates an address on the loopback (localhost) interface.
		 * @param port The port number (in native/host byte order).
		 * @return The full address on the loopback interface.
		 */
		static inet6_address loopback(in_port_t port) {
			const in6_addr LOOPBACK IN6ADDR_LOOPBACK_INIT;
			inet6_address addr;
			addr.create(LOOPBACK, port);
			return addr;
		}
		/**
		 * Checks if the address is set to some value.
		 * This doesn't attempt to determine if the address is valid, simply
		 * that it's not all zero.
		 * @return bool
		 */
		bool is_set() const
		{
			static const auto EMPTY_ADDR = sockaddr_in6{};
			return std::memcmp(&addr_, &EMPTY_ADDR, SZ) != 0;
		}
		/**
		 * Attempts to resolve the host name into a 32-bit internet address.
		 * @param saddr The string host name.
		 * @return The internet address in network byte order.
		 */
		static in6_addr resolve_name(const std::string& saddr)
		{
#if !defined(_WIN32)
			in6_addr ia;
			if (::inet_pton(ADDRESS_FAMILY, saddr.c_str(), &ia) == 1)
				return ia;
#endif

			addrinfo* res, hints = addrinfo{};
			hints.ai_family = ADDRESS_FAMILY;
			hints.ai_socktype = SOCK_STREAM;

			int gai_err = ::getaddrinfo(saddr.c_str(), NULL, &hints, &res);

#if !defined(_WIN32)
			if (gai_err == EAI_SYSTEM)
				throw sys_error();
#endif

			if (gai_err != 0)
				throw getaddrinfo_error(gai_err, saddr);


			auto ipv6 = reinterpret_cast<sockaddr_in6*>(res->ai_addr);
			auto addr = ipv6->sin6_addr;
			freeaddrinfo(res);
			return addr;
		}
		/**
		 * Creates the socket address using the specified host address and port
		 * number.
		 * @param addr The host address (16-byte IPv6 address).
		 * @param port The host port number.
		 */
		void create(const in6_addr& addr, in_port_t port)
		{
			addr_ = sockaddr_in6{};
			addr_.sin6_family = AF_INET6;
			addr_.sin6_flowinfo = 0;
			addr_.sin6_addr = addr;
			addr_.sin6_port = htons(port);
		}
		/**
		 * Creates the socket address using the specified host name and port
		 * number.
		 * @param saddr The string host name.
		 * @param port The port number in native/host byte order.
		 */
		void create(const std::string& saddr, in_port_t port)
		{
			addr_ = sockaddr_in6{};
			addr_.sin6_family = AF_INET6;
			addr_.sin6_flowinfo = 0;
			addr_.sin6_addr = resolve_name(saddr.c_str());
			addr_.sin6_port = htons(port);
		}
		/**
		 * Gets 128-bit IPv6 address.
		 * The address is usually stored in network byte order.
		 * @return The IPv6 address.
		 */
		in6_addr address() const { return addr_.sin6_addr; }
		/**
		 * Gets a byte of the 128-bit IPv6 Address.
		 * Note that the address is normally stored in network byte
		 * order.
		 * @param i The byte to read (0-7)
	{
		return addr_ != sockaddr_in6{};
		 * @return The specified byte in the 128-bit IPv6 Address
		 */
		uint8_t operator[](int i) const {
			return addr_.sin6_addr.s6_addr[i];
		}
		/**
		 * Gets the port number.
		 * @return The port number in native/host byte order.
		 */
		in_port_t port() const { return ntohs(addr_.sin6_port); }
		/**
		 * Gets the size of this structure.
		 * This is equivalent to sizeof(this) but more convenient in some
		 * places.
		 * @return The size of this structure.
		 */
		socklen_t size() const override { return socklen_t(SZ); }
		/**
		 * Gets a pointer to this object cast to a @em sockaddr.
		 * @return A pointer to this object cast to a @em sockaddr.
		 */
		const sockaddr* sockaddr_ptr() const override {
			return reinterpret_cast<const sockaddr*>(&addr_);
		}
		/**
		 * Gets a pointer to this object cast to a @em sockaddr.
		 * @return A pointer to this object cast to a @em sockaddr.
		 */
		sockaddr* sockaddr_ptr() override {
			return reinterpret_cast<sockaddr*>(&addr_);
		}
		/**
		 * Gets a const pointer to this object cast to a @em
		 * sockaddr_in6.
		 * @return const sockaddr_in6 pointer to this object.
		 */
		const sockaddr_in6* sockaddr_in6_ptr() const { return &addr_; }
		/**
		 * Gets a pointer to this object cast to a @em sockaddr_in6.
		 * @return sockaddr_in6 pointer to this object.
		 */
		sockaddr_in6* sockaddr_in6_ptr() { return &addr_; }
		/**
		 * Gets a printable string for the address.
		 * This gets the address in the printable form "[addr]:port"
		 * using inet_ntop(). It does not attempt to find the host name
		 * using a lookup.
		 * @return A string representation of the address in the form
		 *  	   '[address]:port'
		 */
		std::string to_string() const
		{
			char buf[INET6_ADDRSTRLEN];
			auto str = inet_ntop(AF_INET6, (void*)&(addr_.sin6_addr),
				buf, INET6_ADDRSTRLEN);
			return std::string("[") + std::string(str ? str : "<unknown>")
				+ "]:" + std::to_string(unsigned(port()));
		}
	};

	// --------------------------------------------------------------------------

	/**
	 * Stream inserter for the address.
	 * This uses the simple dot notation of the address as returned from
	 * inet_ntop(). It does not attempt a host lookup.
	 * @param os The output stream
	 * @param addr The address
	 * @return A reference to the output stream.
	 */
	inline std::ostream& operator<<(std::ostream& os, const inet6_address& addr)
	{
		char buf[INET6_ADDRSTRLEN];
		auto str = inet_ntop(AF_INET6, (void*)&(addr.sockaddr_in6_ptr()->sin6_addr),
			buf, INET6_ADDRSTRLEN);
		os << "[" << (str ? str : "<unknown>") << "]:" << unsigned(addr.port());
		return os;
	}
} // namespace rola

#endif // !ROLA_EASY_SOCKET_INET6_ADDRESS_HPP
