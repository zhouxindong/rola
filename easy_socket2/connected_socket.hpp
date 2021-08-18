#ifndef ROLA_EASY_SOCKET2_CONNECTED_SOCKET_HPP
#define ROLA_EASY_SOCKET2_CONNECTED_SOCKET_HPP

#include <type_traits>
#include <vector>

#include "socket2.hpp"

namespace rola
{
	/// <summary>
	/// connected socket
	/// non copyable, can movable
	/// </summary>
	class connected_socket : public socket2
	{
	public:
		explicit connected_socket(socket_t h) noexcept
			: socket2{ h }
		{
			set_non_blocking(false);
		}

		connected_socket(const connected_socket&)				= delete;
		connected_socket& operator=(const connected_socket&)	= delete;

		connected_socket(connected_socket&& rhs) noexcept
			: socket2{ std::move(rhs) }
		{
			set_non_blocking(false);
		}

		connected_socket& operator=(connected_socket&& rhs) noexcept
		{
			socket2::operator =(std::move(rhs));
			set_non_blocking(false);
			return *this;
		}

	public:
		result_t write(const void* buf, iolen_t n) noexcept
		{
			return detail::send(this->handle(), buf, n);
		}

		result_t write_n(const void* buf, iolen_t n)
		{
			return detail::send_n(this->handle(), buf, n);
		}

		result_t write(const std::string& s)
		{
			return detail::send(this->handle(), s);
		}

		result_t write(const std::vector<char>& v)
		{
			return detail::send(this->handle(), v.data(), (iolen_t)v.size());
		}

		result_t read(void* buf, iolen_t n) noexcept
		{
			return detail::recv(this->handle(), buf, n);
		}

		result_t read_n(void* buf, iolen_t n)
		{
			return detail::recv_n(this->handle(), buf, n);
		}
	};
} // namespace rola

#endif // ROLA_EASY_SOCKET2_CONNECTED_SOCKET_HPP
