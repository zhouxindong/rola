#ifndef ROLA_EASY_SOCKET_INET_ADDRESS_HPP
#define ROLA_EASY_SOCKET_INET_ADDRESS_HPP

#include "sock_address.hpp"
#include "exception.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <sstream>

#if (defined _WIN32) && (defined __GNUC__)
#pragma message("QT MinGW required: win32:LIBS += -lws2_32")
#endif

namespace rola
{
	namespace detail
	{
		inline std::vector<std::string> split(const std::string& text, const std::unordered_set<char>& tag)
		{
			std::vector<std::string> ret_vec;
			std::string sub_str;

			for (size_t i = 0; i < text.length(); i++)
			{
				if (tag.find(text[i]) != tag.cend())
				{
					if (!sub_str.empty())
					{
						ret_vec.push_back(sub_str);
						sub_str.clear();
					}
				}
				else
				{
					sub_str.push_back(text[i]);
				}
			}

			if (!sub_str.empty())
			{
				ret_vec.push_back(sub_str);
			}

			return ret_vec;
		}

		inline std::string valid_ip(const std::string& ip_str)
		{
			auto v = split(ip_str, { '.' });
			if (v.size() < 4)
				return std::string{};

			std::ostringstream oss;
			for (auto i = 0; i < 4; ++i)
			{
				if (i != 0)
					oss << '.';
				oss << std::atoi(v[i].c_str());
			}
			return oss.str();
		}
	} // namespace detail

	/**
	 * Class that represents an internet (IPv4) address.
	 * This inherits from the IP-specific form of a socket address,
	 * @em sockaddr_in.
	 */
	class inet_address : public sock_address
	{
		/** The underlying C struct for IPv4 addresses */
		sockaddr_in addr_;

		/** The size of the underlying address struct, in bytes */
		static constexpr size_t SZ = sizeof(sockaddr_in);

	public:
		/** The address family for this type of address */
		static constexpr sa_family_t ADDRESS_FAMILY = AF_INET;

		/**
		 * Constructs an empty address.
		 * The address is initialized to all zeros.
		 */
		inet_address() : addr_() {}
		/**
		 * Constructs an address for any i face using the specified port.
		 * This is a convenient way for a server to specify an address that will
		 * bind to all interfaces.
		 * @param port The port number in native/host byte order.
		 */
		explicit inet_address(in_port_t port) {
			create(in_addr_t(INADDR_ANY), port);
		}
		/**
		 * Constructs an address for the specified host using the specified
		 * port.
		 * @param addr The 32-bit host address in native/host byte order.
		 * @param port The port number in native/host byte order.
		 */
		inet_address(uint32_t addr, in_port_t port) { create(addr, port); }
		/**
		 * Constructs an address using the name of the host and the specified
		 * port. This attempts to resolve the host name to an address.
		 *
		 * @param saddr The name of the host.
		 * @param port The port number in native/host byte order.
		 */
		inet_address(const std::string& saddr, in_port_t port) {
			std::string valid_ip = detail::valid_ip(saddr);
			create(valid_ip, port);
		}
		/**
		 * Constructs the address by copying the specified structure.
		 * @param addr The other address
		 */
		inet_address(const sockaddr& addr) {
			std::memcpy(&addr_, &addr, SZ);
		}
		/**
		 * Constructs the address by copying the specified structure.
		 * @param addr The other address
		 */
		inet_address(const sock_address& addr) {
			std::memcpy(&addr_, addr.sockaddr_ptr(), SZ);
		}
		/**
		 * Constructs the address by copying the specified structure.
		 * @param addr The other address
		 */
		inet_address(const sockaddr_in& addr) : addr_(addr) {}
		/**
		 * Constructs the address by copying the specified address.
		 * @param addr The other address
		 */
		inet_address(const inet_address& addr) : addr_(addr.addr_) {}
		/**
		 * Checks if the address is set to some value.
		 * This doesn't attempt to determine if the address is valid, simply
		 * that it's not all zero.
		 * @return bool
		 */
		bool is_set() const
		{
			static const auto EMPTY_ADDR = sockaddr_in{};
			return std::memcmp(&addr_, &EMPTY_ADDR, SZ) != 0;
		}
		/**
		 * Attempts to resolve the host name into a 32-bit internet address.
		 * @param saddr The string host name.
		 * @return The internet address in network byte order.
		 * @throw sys_error, getaddrinfo_error
		 */
		static in_addr_t resolve_name(const std::string& saddr)
		{
#if !defined(_WIN32)
			in_addr ia;
			if (::inet_pton(ADDRESS_FAMILY, saddr.c_str(), &ia) == 1)
				return ia.s_addr;
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


			auto ipv4 = reinterpret_cast<sockaddr_in*>(res->ai_addr);
			auto addr = ipv4->sin_addr.s_addr;
			freeaddrinfo(res);
			return addr;
		}
		/**
		 * Creates the socket address using the specified host address and port
		 * number.
		 * @param addr The host address.
		 * @param port The host port number.
		 */
		void create(in_addr_t addr, in_port_t port)
		{
			addr_ = sockaddr_in{};
			addr_.sin_family = AF_INET;
			addr_.sin_addr.s_addr = htonl(addr);
			addr_.sin_port = htons(port);
		}
		/**
		 * Creates the socket address using the specified host name and port
		 * number.
		 * @param saddr The string host name.
		 * @param port The port number in native/host byte order.
		 * @throw sys_error, getaddrinfo_error
		 */
		void create(const std::string& saddr, in_port_t port)
		{
			addr_ = sockaddr_in{};
			addr_.sin_family = AF_INET;
			addr_.sin_addr.s_addr = resolve_name(saddr.c_str());
			addr_.sin_port = htons(port);
		}
		/**
		 * Gets the 32-bit internet address.
		 * @return The internet address in the local host's byte order.
		 */
		in_addr_t address() const { return ntohl(addr_.sin_addr.s_addr); }
		/**
		 * Gets a byte of the 32-bit Internet Address
		 * @param i The byte to read (0-3)
		 * @return The specified byte in the 32-bit Internet Address
		 */
		uint8_t operator[](int i) const {
			in_addr_t addr = address();
			return ((const uint8_t*)&addr)[i];
		}
		/**
		 * Gets the port number.
		 * @return The port number in native/host byte order.
		 */
		in_port_t port() const { return ntohs(addr_.sin_port); }
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
		 * Gets a const pointer to this object cast to a @em sockaddr_in.
		 * @return const sockaddr_in pointer to this object.
		 */
		const sockaddr_in* sockaddr_in_ptr() const {
			return static_cast<const sockaddr_in*>(&addr_);
		}
		/**
		 * Gets a pointer to this object cast to a @em sockaddr_in.
		 * @return sockaddr_in pointer to this object.
		 */
		sockaddr_in* sockaddr_in_ptr() {
			return static_cast<sockaddr_in*>(&addr_);
		}
		/**
		 * Gets a printable string for the address.
		 * This gets the simple dot notation of the address as returned from
		 * inet_ntop(). It does not attempt a host lookup.
		 * @return A string representation of the address in the form
		 *  	   'address:port'
		 */
                std::string to_string() const
		{
			char buf[INET_ADDRSTRLEN];
#if (defined _WIN32) && (defined __GNUC__)
                        return std::string("<unknown>") + ":" + std::to_string(unsigned(port()));
#else
                        auto str = inet_ntop(AF_INET, (void*)&(addr_.sin_addr), buf, INET_ADDRSTRLEN);
			return std::string(str ? str : "<unknown>")
                                + ":" + std::to_string(unsigned(port()));
#endif
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
	inline std::ostream& operator<<(std::ostream& os, const inet_address& addr)
	{
		char buf[INET_ADDRSTRLEN];
#if (defined _WIN32) && (defined (__GNUC__))
                os << "<unknown>" << ":" << unsigned(addr.port());
#else
		auto str = inet_ntop(AF_INET, (void*)&(addr.sockaddr_in_ptr()->sin_addr),
			buf, INET_ADDRSTRLEN);
		os << (str ? str : "<unknown>") << ":" << unsigned(addr.port());
#endif
		return os;
	}
} // namespace rola

#endif // !ROLA_EASY_SOCKET_INET_ADDRESS_HPP
