#ifndef ROLA_EASY_SOCKET2_SOCKET2_HPP
#define ROLA_EASY_SOCKET2_SOCKET2_HPP

#include "platform2.h"
#include "socket_exception.hpp"
#include <fcntl.h>
#include <fstream>

namespace rola
{
	/// <summary>
	/// class socket2
	/// owner a socket and release it
	/// </summary>
	class socket2
	{
	public:
		socket2() noexcept
			: handle_{ INVALID_SOCKET }
		{}

		explicit socket2(socket_t h) noexcept
			: handle_(h)
		{}

		socket2(const socket2&) = delete;
		socket2& operator=(const socket2&) = delete;

		socket2(socket2&& rhs) noexcept
			: handle_{ rhs.handle_ }
		{
			rhs.handle_ = INVALID_SOCKET;
		}

		socket2& operator=(socket2&& rhs) noexcept
		{
			handle_ = rhs.handle_;
			rhs.handle_ = INVALID_SOCKET;
			return *this;
		}

		~socket2() noexcept
		{
			close();
		}

	public:
		explicit operator bool() const
		{
			return handle_ != INVALID_SOCKET;
		}

		bool get_option(int level, int optname, void* optval, socklen_t* optlen) const
		{
#if defined(_WIN32)
			if (optval && optlen) {
				int len = static_cast<int>(*optlen);
				if ((::getsockopt(handle_, level, optname,
					static_cast<char*>(optval), &len)) >= 0) {
					*optlen = static_cast<socklen_t>(len);
					return true;
				}
			}
			return false;
#else
			return (::getsockopt(handle_, level, optname, optval, optlen) >= 0);
#endif
		}

		template <typename T>
		bool get_option(int level, int optname, T* val) const {
			socklen_t len = sizeof(T);
			return get_option(level, optname, (void*)val, &len);
		}

		bool set_option(int level, int optname, const void* optval, socklen_t optlen)
		{
#if defined(_WIN32)
			return (::setsockopt(handle_, level, optname,
				static_cast<const char*>(optval),
				static_cast<int>(optlen)) >= 0);
#else
			return (::setsockopt(handle_, level, optname, optval, optlen) >= 0);
#endif
		}

		template <typename T>
		bool set_option(int level, int optname, const T& val) {
			return set_option(level, optname, (void*)&val, sizeof(T));
		}

	public:
		bool set_non_blocking(bool on = true)
		{
#if defined(_WIN32)
			unsigned long mode = on ? 1 : 0;
			return ::ioctlsocket(handle_, FIONBIO, &mode);
#else
			int flags = ::fcntl(handle_, F_GETFL, 0);

			if (flags == -1) {
				return false;
			}
			flags = on ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);

			if (::fcntl(handle_, F_SETFL, flags) == -1) {
				return false;
			}
			return true;
#endif
		}

		bool set_read_timeout(const std::chrono::milliseconds& to)
		{
			auto tv =
#if defined(_WIN32)
				DWORD(to.count());
#else
				to_timeval(to);
#endif
			return set_option(SOL_SOCKET, SO_RCVTIMEO, tv);
		}

		template<class Rep, class Period>
		bool set_read_timeout(const std::chrono::duration<Rep, Period>& to) {
			return set_read_timeout(std::chrono::duration_cast<std::chrono::milliseconds>(to));
		}

		bool set_write_timeout(const std::chrono::milliseconds& to)
		{
			auto tv =
#if defined(_WIN32)
				DWORD(to.count());
#else
				to_timeval(to);
#endif

			return set_option(SOL_SOCKET, SO_SNDTIMEO, tv);
		}

		template<class Rep, class Period>
		bool set_write_timeout(const std::chrono::duration<Rep, Period>& to) {
			return set_write_timeout(std::chrono::duration_cast<std::chrono::milliseconds>(to));
		}

	public:
		static void set_address(const char* hname, const char* sname, sockaddr_in* sap)
		{
			::memset(sap, 0, sizeof(sockaddr_in));
			sap->sin_family = AF_INET;
			if (hname != nullptr)
			{
				if (::inet_pton(AF_INET, hname, &sap->sin_addr) != 1)
					throw socket_exception(socket_exception_source::Address);
			}
			else
#ifdef _WIN32
				sap->sin_addr.S_un.S_addr = ::htonl(INADDR_ANY);
#else
				sap->sin_addr.s_addr = ::htonl(INADDR_ANY);
#endif
			char* endptr;
			short port = static_cast<short>(::strtol(sname, &endptr, 0));
			if (*endptr == '\0')
				sap->sin_port = ::htons(port);
			else
				throw socket_exception(socket_exception_source::Port);
		}

		static void set_address(const char* hname, short port, sockaddr_in* sap)
		{
			::memset(sap, 0, sizeof(sockaddr_in));
			sap->sin_family = AF_INET;
			if (hname != nullptr)
			{
				if (::inet_pton(AF_INET, hname, &sap->sin_addr) != 1)
					throw socket_exception(socket_exception_source::Address);
			}
			else
#ifdef _WIN32
				sap->sin_addr.S_un.S_addr = ::htonl(INADDR_ANY);
#else
				sap->sin_addr.s_addr = ::htonl(INADDR_ANY);
#endif				
			sap->sin_port = ::htons(port);
		}

		static socket_t clone(socket_t s)
		{
			socket_t h = INVALID_SOCKET;
#if defined(_WIN32)
			WSAPROTOCOL_INFO protInfo;
			if (::WSADuplicateSocket(s, ::GetCurrentProcessId(), &protInfo) == 0)
				h = ::WSASocket(AF_INET, SOCK_STREAM, 0, &protInfo, 0, WSA_FLAG_OVERLAPPED);
#else
			h = ::dup(s);
#endif
			return h;
		}

		socket_t clone() const
		{
			return clone(handle_);
		}

		bool shutdown()
		{
			return ::shutdown(handle_, SHUT_RDWR) == 0;
		}

		bool shutdown_read()
		{
			return ::shutdown(handle_, SHUT_RD) == 0;
		}

		bool shutdown_write()
		{
			return ::shutdown(handle_, SHUT_WR) == 0;
		}

	public:
		socket_t& handle() noexcept
		{
			return handle_;
		}

		const socket_t& handle() const noexcept
		{
			return handle_;
		}

	private:
		socket_t handle_;

	public:
		void create_socket(int type)
		{
			handle_ = ::socket(AF_INET, type, 0);
			if (handle_ == INVALID_SOCKET)
				throw socket_exception(socket_exception_source::Create);
		}

		void set_reuse()
		{
#if defined(_WIN32)
			const int REUSE = SO_REUSEADDR;
#else
			const int REUSE = SO_REUSEPORT;
#endif
			int reuse = 1;
			if (!set_option(SOL_SOCKET, REUSE, reuse))
				throw socket_exception(socket_exception_source::Reuse);
		}

		void bindto(const sockaddr_in* addr)
		{
			if (::bind(handle_, reinterpret_cast<const sockaddr*>(addr), sizeof(*addr)) != 0)
				throw socket_exception(socket_exception_source::Bind);
		}

		void listen()
		{
			if (::listen(handle_, NLISTEN) != 0)
				throw socket_exception(socket_exception_source::Listen);
		}

		socket_t accept(sockaddr_in* peer)
		{
			socklen_t peerlen = sizeof(sockaddr_in);
			return ::accept(handle_, (sockaddr*)peer, &peerlen);
		}

	public:
		static void close(socket_t s)
		{
			if (s == INVALID_SOCKET)
				return;

#if defined(_WIN32)
			::closesocket(s);
#else
			::close(s);
#endif
			s = INVALID_SOCKET;
		}

		void close()
		{
			close(handle_);
		}
	}; // class socket2

#pragma region send | recv

	namespace detail
	{
		inline result_t send(socket_t s, const void* buf, iolen_t n) noexcept
		{
#if defined(_WIN32)
			return ::send(s, reinterpret_cast<const char*>(buf), n, 0);
#else
			return ::send(s, buf, n, 0);
#endif
		}

		inline result_t send_n(socket_t s, const void* buf, iolen_t n)
		{
			result_t nw = 0;
			iolen_t	nx = 0;

			const uint8_t* b = reinterpret_cast<const uint8_t*>(buf);

			while (nw < n) {
				if ((nx = send(s, b + nw, n - nw)) < 0 && get_last_error() == EINTR)
					continue;

				if (nx <= 0)
					break;

				nw += nx;
			}

			return (nw == 0 && nx < 0) ? nx : result_t(nw);
		}

		inline result_t send(socket_t sock, const std::string& s)
		{
			return send_n(sock, s.data(), static_cast<iolen_t>(s.size()));
		}

		inline result_t recv(socket_t s, void* buf, iolen_t n) noexcept
		{
#if defined(_WIN32)
			return ::recv(s, reinterpret_cast<char*>(buf), n, 0);
#else
			return ::recv(s, buf, n, 0);
#endif
		}

		inline result_t readline(socket_t s, char* buf, iolen_t len) noexcept
		{
			char* bufx = buf;
			static char* bp;
			static result_t cnt = 0;
			static char b[1500];
			char c;

			while (--len > 0)
			{
				if (--cnt <= 0)
				{
					cnt = recv(s, b, sizeof(b));
					if (cnt < 0)
					{
						if (get_last_error() == EINTR)
						{
							len++;	// the while will decrement
							continue;
						}
						return -1;
					}
					if (cnt == 0)
						return 0;
					bp = b;
				}
				c = *bp++;
				*buf++ = c;
				if (c == '\n')
				{
					*buf = '\0';
					return static_cast<result_t>(buf - bufx);
				}
			}
			return -1;
		}

		inline result_t recv_n(socket_t s, void* buf, iolen_t n)
		{
			iolen_t	nr = 0;
			result_t nx = 0;

			uint8_t* b = reinterpret_cast<uint8_t*>(buf);

			while (nr < n) {
				if ((nx = recv(s, b + nr, n - nr)) < 0 && get_last_error() == EINTR)
					continue;

				if (nx <= 0)
					break;

				nr += nx;
			}

			return (nr == 0 && nx < 0) ? nx : (result_t)(nr);
		}

		inline result_t send_to(socket_t s, const void* buf, iolen_t n, const sockaddr_in* addr) noexcept
		{
#if defined(_WIN32)
			return ::sendto(s, reinterpret_cast<const char*>(buf), n, 0,
				reinterpret_cast<const sockaddr*>(addr), static_cast<socklen_t>(sizeof(*addr)));
#else
			return ::sendto(s, buf, n, 0,
				reinterpret_cast<const sockaddr*>(addr), static_cast<socklen_t>(sizeof(*addr)));
#endif
		}

		inline result_t recv_from(socket_t s, void* buf, iolen_t n, sockaddr_in* addr = nullptr) noexcept
		{
			sockaddr* p = addr ? reinterpret_cast<sockaddr*>(addr) : nullptr;
			socklen_t len = addr ? static_cast<socklen_t>(sizeof(*addr)) : 0;

#if defined(_WIN32)
			return ::recvfrom(s, reinterpret_cast<char*>(buf),
				n, 0, p, &len);
#else
			return ::recvfrom(s, buf, n, 0, p, &len);
#endif
		}
	} // namespace detail

	inline size_t send_file(const char* file_path, socket_t sock)
	{
#if defined(_WIN32)
		HANDLE hFile = ::CreateFile(file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return -1;

		LARGE_INTEGER liFileSize;
		if (::GetFileSizeEx(hFile, &liFileSize) == FALSE)
			return -1;

		if (::TransmitFile(sock, hFile, 0, 0, NULL, NULL, TF_USE_DEFAULT_WORKER) == FALSE)
		{
			return -1;
		}

		::CloseHandle(hFile);

		return liFileSize.QuadPart;
#else
		struct stat file_stat;
		if (::stat(file_path, &file_stat) < 0)
			return -1;
		if (S_ISREG(file_stat.st_mode))
		{
			int fd;
			if ((fd = ::open(file_path, O_RDONLY)) < 0)
				return -1;

			if (sendfile(sock, fd, 0, file_stat.st_size) != file_stat.st_size)
			{
				::close(fd);
				return -1;
			}
			::close(fd);
			return file_stat.st_size;
		}
		return -1;
#endif
	}

	static constexpr int BUF_SIZE = 65536;

	inline bool recv_file(const char* file_path, size_t file_size, socket_t sock)
	{
		std::ofstream fout(file_path, std::ios::trunc | std::ios::binary);
		if (!fout.is_open())
			return false;

		size_t left_size = file_size;
		result_t len = 0;

		char buf[BUF_SIZE];
		while (left_size > 0)
		{
			int real_size = static_cast<int>(left_size > BUF_SIZE ? BUF_SIZE : left_size);
			len = ::recv(sock, buf, real_size, 0);
			if (len == 0)
				return false;

			if (len > 0)
			{
				fout.write(buf, len);
				left_size -= len;
			}			
		}

		fout.flush();
		fout.close();
		return true;
	}

#pragma endregion

} // namespace rola

#endif // !ROLA_EASY_SOCKET2_SOCKET2_HPP
