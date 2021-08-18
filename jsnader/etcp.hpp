#ifndef ROLA_JSNADER_ETCP_HPP
#define ROLA_JSNADER_ETCP_HPP

#include <errno.h>
#include <stdlib.h>

#ifdef unix
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "skel.h"

#define TRUE			1
#define FALSE			0
#define NLISTEN			5		/* max waiting connections */
#define NSMB			5		/* number shared memory bufs */
#define SMBUFSZ			256		/* size of shared memory buf */

inline char* program_name() /* for error */
{
	static char* pn;
	return pn;
}

#ifdef __SVR4
#define bzero(b, n)		memset((b), 0, (n))
#endif // __SVR4

typedef void (*tofunc_t)(void*);

inline void error(int status, int err, char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "%s: ", program_name());
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (err)
		fprintf(stderr, ": %s (%d)\n", strerror(err), err);
	if (status)
		EXIT(status);
}

/// <summary>
/// read fixed amount data
/// </summary>
/// <param name=""></param>
/// <param name=""></param>
/// <param name=""></param>
/// <returns></returns>
inline int readn(SOCKET fd, char* bp, size_t len)
{
	int cnt;
	int rc;
	cnt = len;
	while (cnt > 0)
	{
		rc = recv(fd, bp, cnt, 0);
		if (rc < 0)				/* read error? */
		{
			if (errno == EINTR)	/* interrupted? */
				continue;		/* restart the read */
			return -1;			/* return error */
		}

		if (rc == 0)			/* EOF? */
			return len - cnt;	/* return short count */
		bp += rc;
		cnt -= rc;
	}

	return len;
}

/// <summary>
/// head + data
/// </summary>
/// <param name=""></param>
/// <param name=""></param>
/// <param name=""></param>
/// <returns></returns>
inline int readvrec(SOCKET fd, char* bp, size_t len)
{
	u_int32_t reclen;
	int rc;

	/* retrieve the length of the record */
	rc = readn(fd, (char*)&reclen, sizeof(u_int32_t));
	if (rc != sizeof(u_int32_t))
		return rc < 0 ? -1 : 0;

	reclen = ntohl(reclen);
	if (reclen > len)
	{
		/*
		* not enough room for the record
		* discard it and return an error
		*/
		while (reclen > 0)
		{
			rc = readn(fd, bp, len);
			if (rc != len)
				return rc < 0 ? -1 : 0;
			reclen -= len;
			if (reclen < len)
				len = reclen;
		}
		set_errno(EMSGSIZE);
		return -1;
	}

	/* retrieve the record itself */
	rc = readn(fd, bp, reclen);
	if (rc != reclen)
		return rc < 0 ? -1 : 0;
	return rc;
}

inline int readcrlf(SOCKET, char*, size_t);

inline int readline(SOCKET fd, char* bufptr, size_t len)
{
	char* bufx = bufptr;
	static char* bp;
	static int cnt = 0;
	static char b[1500];
	char c;

	while (--len > 0)
	{
		if (--cnt <= 0)
		{
			cnt = recv(fd, b, sizeof(b), 0);
			if (cnt < 0)
			{
				if (errno == EINTR)
				{
					len++; /* the while will decrement */
					continue;
				}
				return -1;
			}
			if (cnt == 0)
				return 0;
			bp = b;
		}
		c = *bp++;
		*bufptr++ = c;
		if (c == '\n')
		{
			*bufptr = '\0';
			return bufptr - bufx;
		}
	}
	set_errno(EMSGSIZE);
	return -1;
}




inline int tselect(int, fd_set*, fd_set*, fd_set*);
inline unsigned int timeout(tofunc_t, void*, int);
inline void untimeout(unsigned int);
inline void init_smb(int);
inline void* smballoc(void);
inline void smbfree(void*);
inline void smbsend(SOCKET, void*);
inline void* smbrecv(SOCKET);

#ifdef _WIN32

inline void init(char** argv)
{
	WSADATA wsadata;
	char* ppn = program_name();
	(ppn = strrchr(argv[0], '\\')) ?
		ppn++ : (ppn = argv[0]);
	WSAStartup(MAKEWORD(2, 2), &wsadata);
}

inline int inet_aton(char* cp, struct in_addr* pin)
{
	int rc;
	rc = inet_addr(cp);
	if (rc == -1 && strcmp(cp, "255.255.255.255"))
		return 0;
	pin->S_un.S_addr = rc;
	return 1;
}

#else
inline void init(char** argv)
{
    char* ppn = program_name();
    (ppn = strrchr(argv[0], '/')) ?
                ppn++ : (ppn = argv[0]);
}
#endif

inline  void set_address(char* hname, char* sname, struct sockaddr_in* sap, char* protocol)
{
	struct servent* sp;
	struct hostent* hp;

	char* endptr;
	short port;
	bzero(sap, sizeof(*sap));
	sap->sin_family = AF_INET;
	if (hname != NULL)
	{
		if (!inet_aton(hname, &sap->sin_addr))
		{
			hp = gethostbyname(hname);
			if (hp == NULL)
				error(1, 0, "unknown host: %s\n", hname);
			sap->sin_addr = *(struct in_addr*)hp->h_addr;
		}
	}
	else
		sap->sin_addr.s_addr = htonl(INADDR_ANY);

	port = strtol(sname, &endptr, 0);
	if (*endptr == '\0')
		sap->sin_port = htons(port);
	else
	{
		sp = getservbyname(sname, protocol);
		if (sp == NULL)
			error(1, 0, "unknown service: %s\n", sname);
		sap->sin_port = sp->s_port;
	}
}

inline SOCKET tcp_server(char* hname, char* sname)
{
	struct sockaddr_in local;
	SOCKET s;
	opt_4th_t on = 1;
	set_address(hname, sname, &local, "tcp");
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (!isvalidsock(s))
		error(1, errno, "socket call failed");

	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)))
		error(1, errno, "setsockopt failed");

	if (bind(s, (struct sockaddr*)&local, sizeof(local)))
		error(1, errno, "bind failed");

	if (listen(s, NLISTEN))
		error(1, errno, "listen failed");
	return s;
}

inline SOCKET tcp_client(char* host, char* port)
{
	struct sockaddr_in peer;
	SOCKET s;
	set_address(host, port, &peer, "tcp");
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (!isvalidsock(s))
		error(1, errno, "socket call failed");

	if (connect(s, (struct sockaddr*)&peer, sizeof(peer)))
		error(1, errno, "connect failed");

	return s;
}

inline SOCKET udp_server(char* hname, char* sname)
{
	SOCKET s;
	struct sockaddr_in local;

	set_address(hname, sname, &local, "udp");
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (!isvalidsock(s))
		error(1, errno, "socket call failed");
	if (bind(s, (struct sockaddr*)&local, sizeof(local)))
		error(1, errno, "bind failed");

	return s;
}

inline SOCKET udp_client(char* hname, char* sname, struct sockaddr_in* sap)
{
	SOCKET s;
	set_address(hname, sname, sap, "udp");
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (!isvalidsock(s))
		error(1, errno, "socket call failed");
	return s;
}

#endif // !ROLA_JSNADER_ETCP_HPP
