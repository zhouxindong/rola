#ifndef ROLA_EASY_SOCKET_PLATFORM_H
#define ROLA_EASY_SOCKET_PLATFORM_H

#include <cstdint>

#if defined(_WIN32)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
// unix:-lc -lstdc++
#define SOCKPP_SOCKET_T_DEFINED

using socket_t = SOCKET;

using socklen_t = int;
using in_port_t = uint16_t;
using in_addr_t = uint32_t;

using sa_family_t = u_short;

#ifndef _SSIZE_T_DEFINED 
#define _SSIZE_T_DEFINED 
#undef ssize_t
using ssize_t = SSIZE_T;
#endif // _SSIZE_T_DEFINED

#ifndef _SUSECONDS_T
#define _SUSECONDS_T
typedef long suseconds_t;	// signed # of microseconds in timeval
#endif	// _SUSECONDS_T

#define SHUT_RD SD_RECEIVE
#define SHUT_WR SD_SEND
#define SHUT_RDWR SD_BOTH

struct iovec
{
	void* iov_base;
	size_t iov_len;
};

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
#endif

#endif

