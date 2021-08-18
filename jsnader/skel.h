#ifndef ROLA_JSNADER_SKEL_H
#define ROLA_JSNADER_SKEL_H

#if defined(_WIN32)

#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")

struct timezone
{
	long tz_minuteswest;
	long tz_dsttime;
};

typedef unsigned int u_int32_t;
typedef const char opt_4th_t;

#define EMSGSIZE WSAEMSGSIZE
#define INIT() init(argv)
#define EXIT(s) do {WSACleanup(); exit((s));}	\
				while(0)
#define CLOSE(s) if(closesocket(s))	\
					error(1, errno, "close failed")
#define errno (GetLastError())
#define set_errno(e) SetLastError((e))
#define isvalidsock(s) ((s) != SOCKET_ERROR)
#define bzero(b,n) memset((b), 0, (n))
#define sleep(t) Sleep((t)*1000)
#define WINDOWS

#define MINBSDSOCKERR (WSAEWOULDBLOCK)
#define MAXBSDSOCKERR (MINBSDSOCKERR + \
						(sizeof(bsdsocketerrs) / \
						sizeof(bsdsocketerrs[0])))

#else

typedef const int opt_4th_t;

#define INIT() init(argv);
#define EXIT(s) exit(s)
#define CLOSE(s) if(close(s)) error(1, errno, \
					"close failed")
#define set_errno(e) errno=(e)
#define isvalidsock(s) ((s) >= 0)

typedef int SOCKET;

#endif

#endif // ROLA_JSNADER_SKEL_H
