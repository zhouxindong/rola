#ifndef ROLA_EASY_SOCKET2_UNCONNECTED_SOCKET_HPP
#define ROLA_EASY_SOCKET2_UNCONNECTED_SOCKET_HPP

#include "socket2.hpp"

namespace rola
{
	/// <summary>
	/// unconnected socket
	/// non copyable, can movable
	/// </summary>
	class unconnected_socket : public socket2
	{
	public:
		explicit unconnected_socket(socket_t h) noexcept
			: socket2{ h }
		{}

		unconnected_socket(const unconnected_socket&) = delete;
		unconnected_socket& operator=(const unconnected_socket&) = delete;

		unconnected_socket(unconnected_socket&& rhs) noexcept
			: socket2{ std::move(rhs) }
		{}

		unconnected_socket& operator=(unconnected_socket&& rhs) noexcept
		{
			socket2::operator =(std::move(rhs));
			return *this;
		}

	public:
		result_t write_to(const void* buf, iolen_t n, const sockaddr_in* addr) noexcept
		{
#if defined(_WIN32)
			return ::sendto(handle(), reinterpret_cast<const char*>(buf), n, 0,
				reinterpret_cast<const sockaddr*>(addr), static_cast<socklen_t>(sizeof(*addr)));
#else
			return ::sendto(handle(), buf, n, 0,
				reinterpret_cast<const sockaddr*>(addr), static_cast<socklen_t>(sizeof(*addr)));
#endif
		}

		result_t read_from(void* buf, iolen_t n, sockaddr_in* addr = nullptr) noexcept
		{
			sockaddr* p = addr ? reinterpret_cast<sockaddr*>(addr) : nullptr;
			socklen_t len = addr ? static_cast<socklen_t>(sizeof(*addr)) : 0;

#if defined(_WIN32)
			return ::recvfrom(handle(), reinterpret_cast<char*>(buf),
				n, 0, p, &len);
#else
			return ::recvfrom(handle(), buf, n, 0, p, &len);
#endif
		}
	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_UNCONNECTED_SOCKET_HPP
