#ifndef ROLA_EASY_SOCKET2_SOCKET_INIT_HPP
#define ROLA_EASY_SOCKET2_SOCKET_INIT_HPP

#include "platform2.h"
#include "socket_exception.hpp"

namespace rola
{
	class socket_init
	{
	public:
		socket_init()
		{
#ifdef _WIN32
			WSADATA wsa;
			int retcode = ::WSAStartup(MAKEWORD(2, 2), &wsa);
			if (retcode != 0)
				throw socket_exception(retcode, rola::socket_exception_source::Init);
#endif // _WIN32
		}

		~socket_init() noexcept(false)
		{
#ifdef _WIN32
			if (::WSACleanup() != 0)
				throw socket_exception(SOCKET_ERROR, rola::socket_exception_source::Destroy);
#endif // _WIN32			
		}
	};
} // namespace rola

#endif // !ROLA_EASY_SOCKET2_SOCKET_INIT_HPP
