#ifndef ROLA_EASY_SOCKET_UNIX_ADDRESS_HPP
#define ROLA_EASY_SOCKET_UNIX_ADDRESS_HPP

#ifdef unix

#include "platform.h"
#include "sock_address.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <sys/un.h>
#include <stdexcept>

namespace rola
{
	/**
	 * Class that represents a UNIX domain address.
	 * This inherits from the UNIX form of a socket address, @em sockaddr_un.
	 */
	class unix_address : public sock_address
	{
		/** The underlying C struct for unix-domain addresses  */
		sockaddr_un addr_;

		/** The size of the underlying address struct, in bytes */
		static constexpr size_t SZ = sizeof(sockaddr_un);

	public:
		/** The address family for this type of address */
		static constexpr sa_family_t ADDRESS_FAMILY()
		{
			return AF_UNIX;
		}

		// TODO: This only applies to Linux
		static constexpr size_t MAX_PATH_NAME()
		{
			return 108;
		}

		/**
		 * Constructs an empty address.
		 * The address is initialized to all zeroes.
		 */
		unix_address() : addr_() {}
		/**
		 * Constructs an address for the specified path.
		 * @param path The
		 */
		unix_address(const std::string& path)
		{
			addr_.sun_family = ADDRESS_FAMILY();
			::strncpy(addr_.sun_path, path.c_str(), MAX_PATH_NAME());
		}
		/**
		 * Constructs the address by copying the specified structure.
		 * @param addr The generic address
		 * @throws std::invalid_argument if the address is not a UNIX-domain
		 *            address (i.e. family is not AF_UNIX)
		 */
		explicit unix_address(const sockaddr& addr)
		{
			auto domain = addr.sa_family;
			if (domain != AF_UNIX)
				throw std::invalid_argument("Not a UNIX-domain address");

			// TODO: We should check the path, or at least see that it has
			// proper NUL termination.
			std::memcpy(&addr_, &addr, sizeof(sockaddr));
		}
		/**
		 * Constructs the address by copying the specified structure.
		 * @param addr The other address
		 */
		unix_address(const sock_address& addr) {
			std::memcpy(&addr_, addr.sockaddr_ptr(), SZ);
		}
		/**
		 * Constructs the address by copying the specified structure.
		 * @param addr The other address
		 * @throws std::invalid_argument if the address is not properly
		 *            initialized as a UNIX-domain address (i.e. family is not
		 *            AF_UNIX)
		 */
		unix_address(const sockaddr_un& addr) : addr_(addr) {}
		/**
		 * Constructs the address by copying the specified address.
		 * @param addr The other address
		 */
		unix_address(const unix_address& addr) : addr_(addr.addr_) {}
		/**
		 * Checks if the address is set to some value.
		 * This doesn't attempt to determine if the address is valid, simply
		 * that it's not all zero.
		 * @return @em true if the address has been set, @em false otherwise.
		 */
		bool is_set() const { return addr_.sun_path[0] != '\0'; }
		/**
		 * Gets the path to which this address refers.
		 * @return The path to which this address refers.
		 */
		std::string path() const { return std::string(addr_.sun_path); }
		/**
		 * Gets the size of the address structure.
		 * Note: In this implementation, this should return sizeof(this) but
		 * more convenient in some places, and the implementation might change
		 * in the future, so it might be more compatible with future revisions
		 * to use this call.
		 * @return The size of the address structure.
		 */
		socklen_t size() const override { return socklen_t(SZ); }

		// TODO: Do we need a:
		//   create(path)
		// to mimic the inet_address behavior?

		/**
		 * Gets a pointer to this object cast to a const @em sockaddr.
		 * @return A pointer to this object cast to a const @em sockaddr.
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
		 * Gets a const pointer to this object cast to a @em sockaddr_un.
		 * @return const sockaddr_un pointer to this object.
		 */
		const sockaddr_un* sockaddr_un_ptr() const { return &addr_; }
		/**
		 * Gets a pointer to this object cast to a @em sockaddr_un.
		 * @return sockaddr_un pointer to this object.
		 */
		sockaddr_un* sockaddr_un_ptr() { return &addr_; }
		/**
		 * Gets a printable string for the address.
		 * @return A string representation of the address in the form
		 *  	   "unix:<path>"
		 */
		std::string to_string() const {
			return std::string("unix:") + std::string(addr_.sun_path);
		}
	};

	// --------------------------------------------------------------------------

	/**
	 * Stream inserter for the address.
	 * @param os The output stream
	 * @param addr The address
	 * @return A reference to the output stream.
	 */
	inline std::ostream& operator<<(std::ostream& os, const unix_address& addr)
	{
		os << "unix:" << addr.path();
		return os;
	}
} // namespace rola

#endif // unix




#endif // !ROLA_EASY_SOCKET_UNIX_ADDRESS_HPP
