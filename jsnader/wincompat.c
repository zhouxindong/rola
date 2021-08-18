#include <sys/timeb.h>
#include "etcp.hpp"
#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>

#endif



//extern int sys_nerr;
//extern char* jd_sys_errlist[];
//static char* bsdsocketerrs[] =
//{
//	"Resource temporarily unavailable",
//	"Operation now in progress",
//	"Operation already in progress",
//	"Socket operation on non-socket",
//	"Destination address required",
//	"Message too long",
//	"Protocol wrong type for socket",
//	"Bad protocol option",
//	"Protocol not supported",
//	"Socket type not supported",
//	"Operation not supported",
//	"Protocol family not supported",
//	"Address family not supported by protocol family",
//	"Address already in use",
//	"Can't assign requested address",
//	"Network is down",
//	"Network is unreachable",
//	"Network dropped connection on reset",
//	"Software caused connection abort",
//	"Connection reset by peer",
//	"No buffer space available",
//	"Socket is already connected",
//	"Socket is not connected",
//	"Cannot send after socket shutdown",
//	"Too many references: can't splice",
//	"Connection timed out",
//	"Connection refused",
//	"Too many levels of symbolic links",
//	"File name too long",
//	"Host is down",
//	"No route to host",
//};

//#ifdef _WIN32
//
//
//
//int inet_aton(char* cp, struct in_addr* pin)
//{
//	int rc;
//	rc = inet_addr(cp);
//	if (rc == -1 && strcmp(cp, "255.255.255.255"))
//		return 0;
//	pin->S_un.S_addr = rc;
//	return 1;
//}
//
//int gettimeofday(struct timeval* tvp, struct timezone* tzp)
//{
//	struct _timeb tb;
//	_ftime(&tb);
//	if (tvp)
//	{
//		tvp->tv_sec = tb.time;
//		tvp->tv_usec = tb.millitm * 1000;
//	}
//	if (tzp)
//	{
//		tzp->tz_minuteswest = tb.timezone;
//		tzp->tz_dsttime = tb.dstflag;
//	}
//}
//
//char* strerror(int err)
//{
//	if (err >= 0 && err < sys_nerr)
//		return sys_errlist[err];
//	else if (err >= MINBSDSOCKERR && err < MAXBSDSOCKERR)
//		return bsdsocketerrs[err - MINBSDSOCKERR];
//	else if (err == WSASYSNOTREADY)
//		return "Network subsystem is unusable";
//	else if (err == WSAVERNOTSUPPORTED)
//		return "This version of Winsock not supported";
//	else if (err == WSANOTINITIALISED)
//		return "Winsock not initialized";
//	else
//		return "Unknown error";
//}
//
//#endif

