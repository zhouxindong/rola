#ifndef ROLA_EASY_SOCKET_SOCKET_HPP
#define ROLA_EASY_SOCKET_SOCKET_HPP

#include "sock_address.hpp"
#include "exception.hpp"
#include <chrono>
#include <string>
#include <tuple>
#include <algorithm>
#include <cstring>
#include <fcntl.h>
#define ignore_result(x) if (x) {}
namespace rola
{
	/////////////////////////////////////////////////////////////////////////////

#if !defined(SOCKPP_SOCKET_T_DEFINED)
	typedef int socket_t;				///< The OS socket handle
	const socket_t INVALID_SOCKET = -1;	///< Invalid socket descriptor
#define SOCKPP_SOCKET_T_DEFINED
#endif


	inline timeval to_timeval(const std::chrono::microseconds& dur)
	{
		using namespace std::chrono;
		const seconds sec = duration_cast<seconds>(dur);

		timeval tv;
#if defined(_WIN32)
		tv.tv_sec = long(sec.count());
#else
		tv.tv_sec = time_t(sec.count());
#endif
		tv.tv_usec = suseconds_t(duration_cast<microseconds>(dur - sec).count());
		return tv;
	}

	template<class Rep, class Period>
	timeval to_timeval(const std::chrono::duration<Rep, Period>& dur) {
		return to_timeval(std::chrono::duration_cast<std::chrono::microseconds>(dur));
	}

	/////////////////////////////////////////////////////////////////////////////

	/**
	 * Base class for socket objects.
	 *
	 * This class wraps an OS socket handle and maintains strict ownership
	 * semantics. If a socket object has a valid, open socket, then it owns the
	 * handle and will close it when the object is destroyed.
	 *
	 * Objects of this class are not copyable, but they are moveable.
	 */
	class socket
	{
		/** The OS integer socket handle */
		socket_t handle_;
		/** Cache of the last error (errno) */
		mutable int lastErr_;
		/**
		 * The OS-specific function to close a socket handle/
		 * @param h The OS socket handle.
		 * @return @em true if the sock is closed, @em false on error.
		 */
		bool close(socket_t h)
		{
#if defined(_WIN32)
			return ::closesocket(h) >= 0;
#else
			return ::close(h) >= 0;
#endif
		}

		// Non-copyable.
		socket(const socket&) = delete;
		socket& operator=(const socket&) = delete;

	protected:
		/**
		 * Closes the socket without checking for errors or updating the last
		 * error.
		 * This is used in internal open/connect type functions that fail after
		 * creating the socket, but want to preserve the previous failure
		 * condition.
		 * Assumes that the socket handle is valid.
		 * @return Always returns @em false.
		 */
		bool close_on_err() {
			close(release());
			return false;
		}
		/**
		 * OS-specific means to retrieve the last error from an operation.
		 * This should be called after a failed system call to set the
		 * lastErr_ member variable. Normally this would be called from
		 * @ref check_ret.
		 */
		static int get_last_error()
		{
#if defined(_WIN32)
			return ::WSAGetLastError();
#else
			int err = errno;
			return err;
#endif
		}
		/**
		 * Cache the last system error code into this object.
		 * This should be called after a failed system call to store the error
		 * value.
		 */
		void set_last_error() {
			lastErr_ = get_last_error();
		}
		/**
		 * Checks the value and if less than zero, sets last error.
		 * @tparam T A signed integer type of any size
		 * @param ret The return value from a library or system call.
		 * @return Returns the value sent to it, `ret`.
		 */
		template <typename T>
		T check_ret(T ret) const {
			lastErr_ = (ret < 0) ? get_last_error() : 0;
			return ret;
		}
		/**
		 * Checks the value and if less than zero, sets last error.
		 * @tparam T A signed integer type of any size
		 * @param ret The return value from a library or system call.
		 * @return @em true if the value is a typical system success value (>=0)
		 *  	   or @em false is is an error (<0)
		 */
		template <typename T>
		bool check_ret_bool(T ret) const {
			lastErr_ = (ret < 0) ? get_last_error() : 0;
			return ret >= 0;
		}
		/**
		 * Checks the value and if it is not a valid socket, sets last error.
		 * This is specifically required for Windows which uses an unsigned type
		 * for its SOCKET.
		 * @param ret The return value from a library or system call that returns
		 *  		  a socket, such as socket() or accept().
		 * @return Returns the value sent to it, `ret`.
		 */
		socket_t check_socket(socket_t ret) const {
			lastErr_ = (ret == INVALID_SOCKET) ? get_last_error() : 0;
			return ret;
		}
		/**
		 * Checks the value and if it is INVALID_SOCKET, sets last error.
		 * This is specifically required for Windows which uses an unsigned type
		 * for its SOCKET.
		 * @param ret The return value from a library or system call that returns
		 *  		  a socket such as socket() or accept().
		 * @return @em true if the value is a valid socket (not INVALID_SOCKET)
		 *  	   or @em false is is an error (INVALID_SOCKET)
		 */
		bool check_socket_bool(socket_t ret) const {
			lastErr_ = (ret == INVALID_SOCKET) ? get_last_error() : 0;
			return ret != INVALID_SOCKET;
		}

	public:
		/**
		 * Creates an unconnected (invalid) socket
		 */
		socket() : handle_(INVALID_SOCKET), lastErr_(0) {}
		/**
		 * Creates a socket from an existing OS socket handle.
		 * The object takes ownership of the handle and will close it when
		 * destroyed.
		 * @param h An OS socket handle.
		 */
		explicit socket(socket_t h) : handle_(h), lastErr_(0) {}
		/**
		 * Move constructor.
		 * This takes ownership of the underlying handle in sock.
		 * @param sock An rvalue reference to a socket object.
		 */
		socket(socket&& sock) noexcept
			: handle_(sock.handle_), lastErr_(sock.lastErr_) {
			sock.handle_ = INVALID_SOCKET;
		}
		/**
		 * Destructor closes the socket.
		 */
		virtual ~socket() { close(); }
		/**
		 * Initializes the socket (sockpp) library.
		 * This is only required for Win32. On platforms that use a standard
		 * socket implementation this is an empty call.
		 */
		static void initialize()
		{
#if defined(_WIN32)
			WSADATA wsadata;
			::WSAStartup(MAKEWORD(2, 0), &wsadata);
#else
			// Don't signal on socket write errors.
			::signal(SIGPIPE, SIG_IGN);
#endif
		}
		/**
		 * Shuts down the socket library.
		 * This is only required for Win32. On platforms that use a standard
		 * socket implementation this is an empty call.
		 */
		static void destroy()
		{
#if defined(_WIN32)
			::WSACleanup();
#endif
		}
		/**
		 * Creates a socket with the specified communications characterics.
		 * Not that this is not normally how a socket is creates in the sockpp
		 * library. Applications would typically create a connector (client) or
		 * acceptor (server) socket which would take care of the details.
		 *
		 * This is included for completeness or for creating different types of
		 * sockets than are supported by the library.
		 *
		 * @param domain The communications domain for the sockets (i.e. the
		 *  			 address family)
		 * @param type The communication semantics for the sockets (SOCK_STREAM,
		 *  		   SOCK_DGRAM, etc).
		 * @param protocol The particular protocol to be used with the sockets
		 *
		 * @return A socket with the requested communications characteristics.
		 */
		static socket create(int domain, int type, int protocol = 0)
		{
			socket sock(::socket(domain, type, protocol));
			if (!sock)
				sock.clear(get_last_error());
			return sock;
		}
		/**
		 * Determines if the socket is open (valid).
		 * @return @em true if the socket is open, @em false otherwise.
		 */
		bool is_open() const { return handle_ != INVALID_SOCKET; }
		/**
		 * Determines if the socket is closed or in an error state.
		 * @return @em true if the socket is closed or in an error state, @em
		 *  	   false otherwise.
		 */
		bool operator!() const {
			return handle_ == INVALID_SOCKET || lastErr_ != 0;
		}
		/**
		 * Determines if the socket is open and in an error-free state.
		 * @return @em true if the socket is open and in an error-free state,
		 *  	   @em false otherwise.
		 */
		explicit operator bool() const {
			return handle_ != INVALID_SOCKET && lastErr_ == 0;
		}
		/**
		 * Get the underlying OS socket handle.
		 * @return The underlying OS socket handle.
		 */
		socket_t handle() const { return handle_; }
		/**
		 * Gets the network family of the address to which the socket is bound.
		 * @return The network family of the address (AF_INET, etc) to which the
		 *  	   socket is bound. If the socket is not bound, or the address
		 *  	   is not known, returns AF_UNSPEC.
		 */
		virtual sa_family_t family() const {
			return address().family();
		}
		/**
		 * Creates a new socket that refers to this one.
		 * This creates a new object with an independent lifetime, but refers
		 * back to this same socket. On most systems, this duplicates the file
		 * handle using the dup() call.
		 * A typical use of this is to have separate threads for reading and
		 * writing the socket. One thread would get the original socket and the
		 * other would get the cloned one.
		 * @return A new socket object that refers to the same socket as this
		 *  	   one.
		 */
		socket clone() const
		{
			socket_t h = INVALID_SOCKET;
#if defined(_WIN32)
			WSAPROTOCOL_INFO protInfo;
			if (::WSADuplicateSocket(handle_, ::GetCurrentProcessId(), &protInfo) == 0)
				h = ::WSASocket(AF_INET, SOCK_STREAM, 0, &protInfo, 0, WSA_FLAG_OVERLAPPED);
			// TODO: Set lastErr_ on failure
#else
			h = ::dup(handle_);
#endif
			return socket(h);
		}
		/**
		 * Creates a pair of connected sockets.
		 *
		 * Whether this will work at all is highly system and domain dependent.
		 * Currently it is only known to work for Unix-domain sockets on *nix
		 * systems.
		 *
		 * Note that applications would normally call this from a derived socket
		 * class which would return the properly type-cast sockets to match the
		 * `domain` and `type`.
		 *
		 * @param domain The communications domain for the sockets (i.e. the
		 *  			 address family)
		 * @param type The communication semantics for the sockets (SOCK_STREAM,
		 *  		   SOCK_DGRAM, etc).
		 * @param protocol The particular protocol to be used with the sockets
		 *
		 * @return A std::tuple of sockets. On error both sockets will be in an
		 *  	   error state with the
		 */
		static std::tuple<socket, socket> pair(int domain, int type, int protocol = 0)
		{
			socket sock0, sock1;

#if !defined(_WIN32)
			int sv[2];
			int ret = ::socketpair(domain, type, protocol, sv);

			if (ret == 0) {
				sock0.reset(sv[0]);
				sock1.reset(sv[1]);
			}
			else {
				int err = get_last_error();
				sock0.clear(err);
				sock1.clear(err);
			}
#else
			sock0.clear(ENOTSUP);
			sock1.clear(ENOTSUP);
#endif
			// TODO: Should we set an "unsupported" error on Windows?

			return std::make_tuple<socket, socket>(std::move(sock0), std::move(sock1));
		}

		/**
		 * Clears the error flag for the object.
		 * @param val The value to set the flag, normally zero.
		 */
		void clear(int val = 0) { lastErr_ = val; }
		/**
		 * Releases ownership of the underlying socket object.
		 * @return The OS socket handle.
		 */
		socket_t release() {
			socket_t h = handle_;
			handle_ = INVALID_SOCKET;
			return h;
		}
		/**
		 * Replaces the underlying managed socket object.
		 * @param h The new socket handle to manage.
		 */
		void reset(socket_t h = INVALID_SOCKET)
		{
			socket_t oh = handle_;
			handle_ = h;
			if (oh != INVALID_SOCKET)
				close(oh);
			clear();
		}
		/**
		 * Move assignment.
		 * This assigns ownership of the socket from the other object to this
		 * one.
		 * @return A reference to this object.
		 */
		socket& operator=(socket&& sock) noexcept {
			// Give our handle to the other to close.
			std::swap(handle_, sock.handle_);
			lastErr_ = sock.lastErr_;
			return *this;
		}
		/**
		 * Binds the socket to the specified address.
		 * @param addr The address to which we get bound.
		 * @return @em true on success, @em false on error
		 */
		bool bind(const sock_address& addr)
		{
			return check_ret_bool(::bind(handle_, addr.sockaddr_ptr(), addr.size()));
		}
		/**
		 * Gets the local address to which the socket is bound.
		 * @return The local address to which the socket is bound.
		 */
		sock_address_any address() const
		{
			auto addrStore = sockaddr_storage{};
			socklen_t len = sizeof(sockaddr_storage);

			if (!check_ret_bool(::getsockname(handle_,
				reinterpret_cast<sockaddr*>(&addrStore), &len)))
				return sock_address_any{};

			return sock_address_any(addrStore, len);
		}
		/**
		 * Gets the address of the remote peer, if this socket is connected.
		 * @return The address of the remote peer, if this socket is connected.
		 */
		sock_address_any peer_address() const
		{
			auto addrStore = sockaddr_storage{};
			socklen_t len = sizeof(sockaddr_storage);

			if (!check_ret_bool(::getpeername(handle_,
				reinterpret_cast<sockaddr*>(&addrStore), &len)))
				return sock_address_any{};

			return sock_address_any(addrStore, len);
		}
		/**
		 * Gets the value of a socket option.
		 *
		 * This is a thin wrapper for the system `getsockopt`.
		 *
		 * @param level The protocol level at which the option resides, such as
		 *              SOL_SOCKET.
		 * @param optname The option passed directly to the protocol module.
		 * @param optval The buffer for the value to retrieve
		 * @param optlen Initially contains the lenth of the buffer, and on return
		 *               contains the length of the value retrieved.
		 *
		 * @return bool @em true if the value was retrieved, @em false if an error
		 *         occurred.
		 */
		bool get_option(int level, int optname, void* optval, socklen_t* optlen) const
		{
#if defined(_WIN32)
			if (optval && optlen) {
				int len = static_cast<int>(*optlen);
				if (check_ret_bool(::getsockopt(handle_, level, optname,
					static_cast<char*>(optval), &len))) {
					*optlen = static_cast<socklen_t>(len);
					return true;
				}
			}
			return false;
#else
			return check_ret_bool(::getsockopt(handle_, level, optname, optval, optlen));
#endif
		}
		/**
		 * Gets the value of a socket option.
		 *
		 * @param level The protocol level at which the option resides, such as
		 *              SOL_SOCKET.
		 * @param optname The option passed directly to the protocol module.
		 * @param val The value to retrieve
		 * @return bool @em true if the value was retrieved, @em false if an error
		 *         occurred.
		 */
		template <typename T>
		bool get_option(int level, int optname, T* val) const {
			socklen_t len = sizeof(T);
			return get_option(level, optname, (void*)val, &len);
		}
		/**
		 * Sets the value of a socket option.
		 *
		 * This is a thin wrapper for the system `setsockopt`.
		 *
		 * @param level The protocol level at which the option resides, such as
		 *              SOL_SOCKET.
		 * @param optname The option passed directly to the protocol module.
		 * @param optval The buffer with the value to set.
		 * @param optlen Contains the lenth of the value buffer.
		 *
		 * @return bool @em true if the value was set, @em false if an error
		 *         occurred.
		 */
		bool set_option(int level, int optname, const void* optval, socklen_t optlen)
		{
#if defined(_WIN32)
			return check_ret_bool(::setsockopt(handle_, level, optname,
				static_cast<const char*>(optval),
				static_cast<int>(optlen)));
#else
			return check_ret_bool(::setsockopt(handle_, level, optname, optval, optlen));
#endif
		}
		/**
		 * Sets the value of a socket option.
		 *
		 * @param level The protocol level at which the option resides, such as
		 *              SOL_SOCKET.
		 * @param optname The option passed directly to the protocol module.
		 * @param val The value to set.
		 *
		 * @return bool @em true if the value was set, @em false if an error
		 *         occurred.
		 */
		template <typename T>
		bool set_option(int level, int optname, const T& val) {
			return set_option(level, optname, (void*)&val, sizeof(T));
		}
		/**
		 * Places the socket into or out of non-blocking mode.
		 * When in non-blocking mode, a call that is not immediately ready to
		 * complete (read, write, accept, etc) will return immediately with the
		 * error EWOULDBLOCK.
		 * @param on Whether to turn non-blocking mode on or off.
		 * @return @em true on success, @em false on failure.
		 */
		bool set_non_blocking(bool on = true)
		{
#if defined(_WIN32)
			unsigned long mode = on ? 1 : 0;
			return check_ret_bool(::ioctlsocket(handle_, FIONBIO, &mode));
#else
			/**
			 * TODO: Consider a generic function:
			 *   bool set_flag(int flag, bool on=true);
			 * Used like:
			 *   set_flag(O_NONBLOCK, on);
			 */
			int flags = ::fcntl(handle_, F_GETFL, 0);

			if (flags == -1) {
				set_last_error();
				return false;
			}
			flags = on ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);

			if (::fcntl(handle_, F_SETFL, flags) == -1) {
				set_last_error();
				return false;
			}
			return true;
#endif
		}
		/**
		 * Gets a string describing the specified error.
		 * This is typically the returned message from the system strerror().
		 * @param errNum The error number.
		 * @return A string describing the specified error.
		 */
		static std::string error_str(int errNum)
		{
			return sys_error::error_str(errNum);
		}
		/**
		 * Gets the code for the last errror.
		 * This is typically the code from the underlying OS operation.
		 * @return The code for the last errror.
		 */
		int last_error() const { return lastErr_; }
		/**
		 * Gets a string describing the last errror.
		 * This is typically the returned message from the system strerror().
		 * @return A string describing the last errror.
		 */
		std::string last_error_str() const {
			return error_str(lastErr_);
		}
		/**
		 * Shuts down all or part of the full-duplex connection.
		 * @param how Which part of the connection should be shut:
		 *  	@li SHUT_RD   (0) Further reads disallowed.
		 *  	@li SHUT_WR   (1) Further writes disallowed
		 *  	@li SHUT_RDWR (2) Further reads and writes disallowed.
		 * @return @em true on success, @em false on error.
		 */
		bool shutdown(int how = SHUT_RDWR)
		{
			return check_ret_bool(::shutdown(handle_, how));
		}
		/**
		 * Closes the socket.
		 * After closing the socket, the handle is @em invalid, and can not be
		 * used again until reassigned.
		 * @return @em true if the sock is closed, @em false on error.
		 */
		bool close()
		{
			if (handle_ != INVALID_SOCKET) {
				if (!close(release())) {
					set_last_error();
					return false;
				}
			}
			return true;
		}
	};

	/////////////////////////////////////////////////////////////////////////////

	/**
	 * RAII class to initialize and then shut down the library.
	 * A single object of this class can be declared before any other classes in
	 * the library are used. The lifetime of the object should span the use of
	 * the other classes in the library, so declaring an object at the top of
	 * main() is usually the best choice.
	 * This is only required on some platforms, particularly Windows, but is
	 * harmless on other platforms. On some, such as POSIX, the initializer sets
	 * optional parameters for the library, and the destructor does nothing.
	 */
	class socket_initializer
	{
	public:
		socket_initializer() { socket::initialize(); }
		~socket_initializer() { socket::destroy(); }
	};
} // namespace rola

#endif // !ROLA_EASY_SOCKET_SOCKET_HPP
