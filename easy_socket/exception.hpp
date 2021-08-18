#ifndef ROLA_EASY_SOCKET_EXCEPTION_HPP
#define ROLA_EASY_SOCKET_EXCEPTION_HPP

#include <cerrno>
#include <stdexcept>
#include <string>
#include <cstring>

#include "platform.h"

// Used to explicitly ignore the returned value of a function call.
#define ignore_result(x) if (x) {}

//#pragma push_macro("FormatMessage")
//#pragma push_macro("gai_strerror")
//#undef FormatMessage
//#undef gai_strerror
#if defined(_WIN32)
#define gai_strerror_ gai_strerrorA
#else
#define gai_strerror_ gai_strerror
#endif

namespace rola
{
	/**
 * System error.
 * These are errors that are resulted from system socket calls. The error
 * codes are platform 'errno' values (or similar), and the messages are
 * typically derived from the system.
 */
	class sys_error : public std::runtime_error
	{
		/** The system error number (errno) */
		int errno_;

	public:
		/**
		 * Creates an error using the current system 'errno' value.
		 */
		sys_error() : sys_error(errno) {}
		/**
		 * Constructs an error with the specified system errno.
		 * @param err The error number. This is the system errno value.
		 */
		explicit sys_error(int err)
			: runtime_error(error_str(err)), errno_(err)
		{
		}
		/**
		 * Get the error number.
		 * @return The system error number.
		 */
		int error() const { return errno_; }
		/**
		 * Gets a string describing the specified error.
		 * This is typically the returned message from the system strerror().
		 * @param err The system error number.
		 * @return A string describing the specified error.
		 */
		static std::string error_str(int err)
		{
			char buf[1024];
			buf[0] = '\x0';

#if defined(_WIN32)
			::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				buf, sizeof(buf), NULL);
#else
#ifdef _GNU_SOURCE
			auto s = strerror_r(err, buf, sizeof(buf));
			return s ? std::string(s) : std::string();
#else
			ignore_result(strerror_r(err, buf, sizeof(buf)));
#endif
#endif
			return std::string(buf);
		}
	};

	/**
	 * Errors from getaddrinfo.
	 * These are errors relating to DNS lookup, returned from the getaddrinfo system call.
	 * Their codes are declared in <netdb.h>.
	 */
	class getaddrinfo_error : public std::runtime_error
	{
		/** The error number, as returned by getaddrinfo. */
		int error_;
		/** The hostname being resolved. */
		std::string hostname_;

	public:
		/**
		 * Constructs an error with the specified getaddrinfo error code.
		 * @param err The error number, as returned by getaddrinfo.
		 * @param hostname The DNS name being resolved that triggered the error.
		 */
		getaddrinfo_error(int err, const std::string& hostname)
			: runtime_error(gai_strerror_(err)), error_(err), hostname_(hostname)
		{}
		/**
		 * Get the error number.
		 * @return The error number returned by getaddrinfo.
		 */
		int error() const { return error_; }
		/**
		 * Get the hostname that triggered the error.
		 * @return The hostname that getaddrinfo failed to resolve.
		 */
		const std::string& hostname() const { return hostname_; }
	};
} // namespace rola

//#pragma pop_macro("gai_strerror")
//#pragma pop_macro("FormatMessage")

#endif // !ROLA_EASY_SOCKET_EXCEPTION_HPP
