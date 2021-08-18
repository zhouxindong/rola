#ifndef ROLA_EASY_SOCKET2_PLATFORM2_H
#define ROLA_EASY_SOCKET2_PLATFORM2_H

#include <cstdint>
#include <iostream>
#include <chrono>
#include <string>
#include <sstream>

#if defined(_WIN32)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE
#endif

#if !defined(_WINSOCK_DEPRECATED_NO_WARNINGS)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#ifndef _SUSECONDS_T
#define _SUSECONDS_T
typedef long suseconds_t;	// signed # of microseconds in timeval
#endif	// _SUSECONDS_T

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#include <MSWSock.h>
#pragma comment(lib, "Mswsock.lib")

using socket_t = SOCKET;
using socklen_t = int;
using result_t = int;
using iolen_t = int;

#define SHUT_RD SD_RECEIVE
#define SHUT_WR SD_SEND
#define SHUT_RDWR SD_BOTH

#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#ifdef __FreeBSD__
#include <netinet/in.h>
#endif
#include <netdb.h>
#include <signal.h>
#include <cerrno>
#include <cstring>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstdlib>

using socket_t = int;
using result_t = ssize_t;
using iolen_t = size_t;

#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif // !INVALID_SOCKET

#define NLISTEN			1	// listen backlog
#define ACCEPT_WAIT		1	// unit: ms

static int get_last_error()
{
#if defined(_WIN32)
	return ::WSAGetLastError();
#else
	int err = errno;
	return err;
#endif
}

static void pause_wait_press()
{
	std::cout << "paused! press any key to continue...";
	std::cin.get();
}

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
#endif // ROLA_EASY_SOCKET2_PLATFORM2_H

