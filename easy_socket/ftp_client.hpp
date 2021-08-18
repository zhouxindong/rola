#ifndef ROLA_EASY_SOCKET_FTP_CLIENT_HPP
#define ROLA_EASY_SOCKET_FTP_CLIENT_HPP

#define NOSSL

#if defined(_WIN32)
#include <time.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#include <sys/time.h>
#endif

#ifdef NOLFS
#define off64_t long
#else
#define off64_t long long
#endif

#define fseeko64 fseek
#define fopen64 fopen

#if defined(__APPLE__)
#define off64_t __darwin_off_t
#define fseeko64 fseeko
#define fopen64 fopen
#endif
#include <sys/types.h>

#ifndef _FTPLIB_SSL_CLIENT_METHOD_
#define _FTPLIB_SSL_CLIENT_METHOD_ TLSv1_2_client_method
#endif

//#ifdef _WIN32
//#include <chrono>
//struct timeval
//{
//	__int64 tv_sec;
//	__int64 tv_usec;
//};
//
//inline void gettimeofday(struct timeval* tv)
//{
//	auto time_now = std::chrono::system_clock::now();
//	std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
//	auto duration_in_s = std::chrono::duration_cast<std::chrono::seconds>(time_now.time_since_epoch()).count();
//	auto duration_in_us = std::chrono::duration_cast<std::chrono::microseconds>(time_now.time_since_epoch()).count();
//
//	tv->tv_sec = duration_in_s;
//	tv->tv_usec = duration_in_us;
//}
//#endif // WIN32

#ifndef NOLFS
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#if defined(_WIN32)
#include <windows.h>
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>

#if defined(_WIN32)
#define SETSOCKOPT_OPTVAL_TYPE (const char *)
#else
#define SETSOCKOPT_OPTVAL_TYPE (void *)
#endif

#if defined(_WIN32)
#define net_read(x,y,z) recv(x,(char*)y,z,0)
#define net_write(x,y,z) send(x,(char*)y,z,0)
#define net_close closesocket
#else
#define net_read read
#define net_write write
#define net_close close
#endif

#if defined(_WIN32)
typedef int socklen_t;
#endif

#if defined(_WIN32)
#define memccpy _memccpy
#define strdup _strdup
#endif

/* socket values */
//#define SETSOCKOPT_OPTVAL_TYPE (void *)
#define FTPLIB_BUFSIZ 1024
#define ACCEPT_TIMEOUT 30

/* lo types */
#define FTPLIB_CONTROL 0
#define FTPLIB_READ 1
#define FTPLIB_WRITE 2

namespace rola
{
	//SSL
	//typedef struct ssl_st SSL;
	//typedef struct ssl_ctx_st SSL_CTX;
	//typedef struct bio_st BIO;
	//typedef struct x509_st X509;

	// callback function signatures
	typedef int (*FtpCallbackXfer)(off64_t xfered, void* arg);
	typedef int (*FtpCallbackIdle)(void* arg);
	typedef void (*FtpCallbackLog)(char* str, void* arg, bool out);
	//SSL
	//typedef bool (*FtpCallbackCert)(void* arg, X509* cert);

	struct ftphandle {
		char* cput;
		char* cget;
		int handle;
		int cavail;
		int cleft;
		char* buf;
		int dir;
		ftphandle* ctrl;
		int cmode;
		struct timeval idletime;
		FtpCallbackXfer xfercb;
		FtpCallbackIdle idlecb;
		FtpCallbackLog logcb;
		void* cbarg;
		off64_t xfered;
		off64_t cbbytes;
		off64_t xfered1;
		char response[256];
		//SSL
		//SSL* ssl;
		//SSL_CTX* ctx;
		//BIO* sbio;
		//int tlsctrl;
		//int tlsdata;
		//FtpCallbackCert certcb;

		off64_t offset;
		bool correctpasv;
	};

	class ftplib
	{
	public:

		enum accesstype
		{
			dir = 1,
			dirverbose,
			fileread,
			filewrite,
			filereadappend,
			filewriteappend
		};

		enum transfermode
		{
			ascii = 'A',
			image = 'I'
		};

		enum connmode
		{
			pasv = 1,
			port
		};

		enum fxpmethod
		{
			defaultfxp = 0,
			alternativefxp
		};

		//enum dataencryption
		//{
		//	unencrypted = 0,
		//	secure
		//};

		ftplib();
		~ftplib();
		char* LastResponse();
		int Connect(const char* host);
		int Login(const char* user, const char* pass);
		int Site(const char* cmd);
		int Raw(const char* cmd);
		int SysType(char* buf, int max);
		int Mkdir(const char* path);
		int Chdir(const char* path);
		int Cdup();
		int Rmdir(const char* path);
		int Pwd(char* path, int max);
		int Nlst(const char* outputfile, const char* path);
		int Dir(const char* outputfile, const char* path);
		int Size(const char* path, int* size, transfermode mode);
		int ModDate(const char* path, char* dt, int max);
		int Get(const char* outputfile, const char* path, transfermode mode, off64_t offset = 0);
		int Put(const char* inputfile, const char* path, transfermode mode, off64_t offset = 0);
		int Rename(const char* src, const char* dst);
		int Delete(const char* path);
		int Quit();
		void SetCallbackIdleFunction(FtpCallbackIdle pointer);
		void SetCallbackLogFunction(FtpCallbackLog pointer);
		void SetCallbackXferFunction(FtpCallbackXfer pointer);
		void SetCallbackArg(void* arg);
		void SetCallbackBytes(off64_t bytes);
		void SetCorrectPasv(bool b) { mp_ftphandle->correctpasv = b; };
		void SetCallbackIdletime(int time);
		void SetConnmode(connmode mode);
		static int Fxp(ftplib* src, ftplib* dst, const char* pathSrc, const char* pathDst, transfermode mode, fxpmethod method);
		ftphandle* RawOpen(const char* path, accesstype type, transfermode mode);
		int RawClose(ftphandle* handle);
		int RawWrite(void* buf, int len, ftphandle* handle);
		int RawRead(void* buf, int max, ftphandle* handle);
		// SSL
		//int SetDataEncryption(dataencryption enc);
		//int NegotiateEncryption();
		//void SetCallbackCertFunction(FtpCallbackCert pointer);

	private:
		ftphandle* mp_ftphandle{ nullptr };

		int FtpXfer(const char* localfile, const char* path, ftphandle* nControl, accesstype type, transfermode mode);
		int FtpOpenPasv(ftphandle* nControl, ftphandle** nData, transfermode mode, int dir, char* cmd);
		int FtpSendCmd(const char* cmd, char expresp, ftphandle* nControl);
		int FtpAcceptConnection(ftphandle* nData, ftphandle* nControl);
		int FtpOpenPort(ftphandle* nControl, ftphandle** nData, transfermode mode, int dir, char* cmd);
		int FtpRead(void* buf, int max, ftphandle* nData);
		int FtpWrite(void* buf, int len, ftphandle* nData);
		int FtpAccess(const char* path, accesstype type, transfermode mode, ftphandle* nControl, ftphandle** nData);
		int FtpClose(ftphandle* nData);
		int socket_wait(ftphandle* ctl);
		int readline(char* buf, int max, ftphandle* ctl);
		int writeline(char* buf, int len, ftphandle* nData);
		int readresp(char c, ftphandle* nControl);
		void sprint_rest(char* buf, off64_t offset);
		void ClearHandle();
		int CorrectPasvResponse(unsigned char* v);
	};

#pragma region ftplib defines

	inline rola::ftplib::ftplib()
	{
#if defined(_WIN32)
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(1, 1), &wsa))
		{
			printf("WSAStartup() failed, %lu\n", (unsigned long)GetLastError());
		}
#endif

		mp_ftphandle = static_cast<ftphandle*>(calloc(1, sizeof(ftphandle)));
		if (mp_ftphandle == NULL) perror("calloc");
		mp_ftphandle->buf = static_cast<char*>(malloc(FTPLIB_BUFSIZ));
		if (mp_ftphandle->buf == NULL)
		{
			perror("calloc");
			free(mp_ftphandle);
		}

		ClearHandle();
	}

	inline ftplib::~ftplib()
	{
		free(mp_ftphandle->buf);
		free(mp_ftphandle);
	}

	/*
	* return a pointer to the last response received
	*/
	inline char* ftplib::LastResponse()
	{
		if ((mp_ftphandle) && (mp_ftphandle->dir == FTPLIB_CONTROL)) return mp_ftphandle->response;
		return NULL;
	}

	/*
	* connect to remove server
	* return 1 if connected, 0 if not
	*/
	inline int ftplib::Connect(const char* host)
	{
		//int sControl;
		SOCKET sControl;
		struct sockaddr_in sin;
		struct hostent* phe;
		struct servent* pse;
		int on = 1;
		int ret;
		char* lhost;
		char* pnum;

		mp_ftphandle->dir = FTPLIB_CONTROL;
		mp_ftphandle->ctrl = NULL;
		mp_ftphandle->xfered = 0;
		mp_ftphandle->xfered1 = 0;

		mp_ftphandle->offset = 0;
		mp_ftphandle->handle = 0;

		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		lhost = strdup(host);
		pnum = strchr(lhost, ':');
		if (pnum == NULL)
		{
			if ((pse = getservbyname("ftp", "tcp")) == NULL)
			{
				perror("getservbyname");
				free(lhost);
				return 0;
			}
			sin.sin_port = pse->s_port;
		}
		else
		{
			*pnum++ = '\0';
			if (isdigit(*pnum)) sin.sin_port = htons(atoi(pnum));
			else
			{
				pse = getservbyname(pnum, "tcp");
				sin.sin_port = pse->s_port;
			}
		}

#if defined(_WIN32)
		if ((sin.sin_addr.s_addr = inet_addr(lhost)) == -1)
#else
		ret = inet_aton(lhost, &sin.sin_addr);
		if (ret == 0)
#endif
		{
			if ((phe = gethostbyname(lhost)) == NULL)
			{
				perror("gethostbyname");
				free(lhost);
				return 0;
			}
			memcpy((char*)&sin.sin_addr, phe->h_addr, phe->h_length);
		}

		free(lhost);

		sControl = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sControl == -1)
		{
			perror("socket");
			return 0;
		}

		if (setsockopt(sControl, SOL_SOCKET, SO_REUSEADDR, SETSOCKOPT_OPTVAL_TYPE & on, sizeof(on)) == -1)
		{
			perror("setsockopt");
			net_close(sControl);
			return 0;
		}
		if (connect(sControl, (struct sockaddr*)&sin, sizeof(sin)) == -1)
		{
			perror("connect");
			net_close(sControl);
			return 0;
		}

		mp_ftphandle->handle = sControl;

		if (readresp('2', mp_ftphandle) == 0)
		{
			net_close(sControl);
			mp_ftphandle->handle = 0;
			return 0;
		}

		return 1;
	}

	/*
	* log in to remote server
	* return 1 if logged in, 0 otherwise
	*/
	inline int ftplib::Login(const char* user, const char* pass)
	{
		char tempbuf[64];

		if (((strlen(user) + 7) > sizeof(tempbuf)) || ((strlen(pass) + 7) > sizeof(tempbuf))) return 0;
		sprintf(tempbuf, "USER %s", user);
		if (!FtpSendCmd(tempbuf, '3', mp_ftphandle))
		{
			if (mp_ftphandle->ctrl != NULL) return 1;
			if (*LastResponse() == '2') return 1;
			return 0;
	}
		sprintf(tempbuf, "PASS %s", pass);
		return FtpSendCmd(tempbuf, '2', mp_ftphandle);
	}

	/*
	* send a SITE command
	* return 1 if command successful, 0 otherwise
	*/
	inline int ftplib::Site(const char* cmd)
	{
		char buf[256];

		if ((strlen(cmd) + 7) > sizeof(buf)) return 0;
		sprintf(buf, "SITE %s", cmd);
		if (!FtpSendCmd(buf, '2', mp_ftphandle)) return 0;
		return 1;
	}

	/*
	* send a raw string string
	* return 1 if command successful, 0 otherwise
	*/
	inline int ftplib::Raw(const char* cmd)
	{
		char buf[256];
		strncpy(buf, cmd, 256);
		if (!FtpSendCmd(buf, '2', mp_ftphandle)) return 0;
		return 1;
	}

	/*
	* send a SYST command
	* Fills in the user buffer with the remote system type.  If more
	* information from the response is required, the user can parse
	* it out of the response buffer returned by FtpLastResponse().
	* 
	* return 1 if command successful, 0 otherwise
	*/
	inline int ftplib::SysType(char* buf, int max)
	{
		int l = max;
		char* b = buf;
		char* s;
		if (!FtpSendCmd("SYST", '2', mp_ftphandle)) return 0;
		s = &mp_ftphandle->response[4];
		while ((--l) && (*s != ' ')) *b++ = *s++;
		*b++ = '\0';
		return 1;
	}

	/*
	* create a directory at server
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::Mkdir(const char* path)
	{
		char buf[256];

		if ((strlen(path) + 6) > sizeof(buf)) return 0;
		sprintf(buf, "MKD %s", path);
		if (!FtpSendCmd(buf, '2', mp_ftphandle)) return 0;
		return 1;
	}

	/*
	* change path at remote
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::Chdir(const char* path)
	{
		char buf[256];

		if ((strlen(path) + 6) > sizeof(buf)) return 0;
		sprintf(buf, "CWD %s", path);
		if (!FtpSendCmd(buf, '2', mp_ftphandle)) return 0;
		return 1;
	}

	/*
	* move to parent directory at remote
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::Cdup()
	{
		if (!FtpSendCmd("CDUP", '2', mp_ftphandle)) return 0;
		return 1;
	}

	/*
	* remove directory at remote
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::Rmdir(const char* path)
	{
		char buf[256];

		if ((strlen(path) + 6) > sizeof(buf)) return 0;
		sprintf(buf, "RMD %s", path);
		if (!FtpSendCmd(buf, '2', mp_ftphandle)) return 0;
		return 1;
	}

	/*
	* get working directory at remote
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::Pwd(char* path, int max)
	{
		int l = max;
		char* b = path;
		char* s;

		if (!FtpSendCmd("PWD", '2', mp_ftphandle)) return 0;
		s = strchr(mp_ftphandle->response, '"');
		if (s == NULL) return 0;
		s++;
		while ((--l) && (*s) && (*s != '"')) *b++ = *s++;
		*b = '\0';
		return 1;
	}

	/*
	* issue an NLST command and write response to output
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::Nlst(const char* outputfile, const char* path)
	{
		mp_ftphandle->offset = 0;
		return FtpXfer(outputfile, path, mp_ftphandle, ftplib::dir, ftplib::ascii);
	}

	/*
	* issue a LIST command and write response to output
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::Dir(const char* outputfile, const char* path)
	{
		mp_ftphandle->offset = 0;
		return FtpXfer(outputfile, path, mp_ftphandle, ftplib::dirverbose, ftplib::ascii);
	}

	/*
	* determine the size of a remote file
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::Size(const char* path, int* size, transfermode mode)
	{
		char cmd[256];
		int resp, sz, rv = 1;

		if ((strlen(path) + 7) > sizeof(cmd)) return 0;

		sprintf(cmd, "TYPE %c", mode);
		if (!FtpSendCmd(cmd, '2', mp_ftphandle)) return 0;

		sprintf(cmd, "SIZE %s", path);
		if (!FtpSendCmd(cmd, '2', mp_ftphandle)) rv = 0;
		else
		{
			if (sscanf(mp_ftphandle->response, "%d %d", &resp, &sz) == 2) *size = sz;
			else rv = 0;
		}
		return rv;
	}

	/*
	* determine the modification date of a remote file
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::ModDate(const char* path, char* dt, int max)
	{
		char buf[256];
		int rv = 1;

		if ((strlen(path) + 7) > sizeof(buf)) return 0;
		sprintf(buf, "MDTM %s", path);
		if (!FtpSendCmd(buf, '2', mp_ftphandle)) rv = 0;
		else strncpy(dt, &mp_ftphandle->response[4], max);
		return rv;
	}

	/*
	* issue a GET command and write received data to output
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::Get(const char* outputfile, const char* path, transfermode mode, off64_t offset)
	{
		mp_ftphandle->offset = offset;
		if (offset == 0) return FtpXfer(outputfile, path, mp_ftphandle, ftplib::fileread, mode);
		else return FtpXfer(outputfile, path, mp_ftphandle, ftplib::filereadappend, mode);
	}

	/*
	* issue a PUT command and send data from input
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::Put(const char* inputfile, const char* path, transfermode mode, off64_t offset)
	{
		mp_ftphandle->offset = offset;
		if (offset == 0) return FtpXfer(inputfile, path, mp_ftphandle, ftplib::filewrite, mode);
		else return FtpXfer(inputfile, path, mp_ftphandle, ftplib::filewriteappend, mode);
	}

	inline int ftplib::Rename(const char* src, const char* dst)
	{
		char cmd[256];

		if (((strlen(src) + 7) > sizeof(cmd)) || ((strlen(dst) + 7) > sizeof(cmd))) return 0;
		sprintf(cmd, "RNFR %s", src);
		if (!FtpSendCmd(cmd, '3', mp_ftphandle)) return 0;
		sprintf(cmd, "RNTO %s", dst);
		if (!FtpSendCmd(cmd, '2', mp_ftphandle)) return 0;

		return 1;
	}

	inline int ftplib::Delete(const char* path)
	{
		char cmd[256];

		if ((strlen(path) + 7) > sizeof(cmd)) return 0;
		sprintf(cmd, "DELE %s", path);
		if (!FtpSendCmd(cmd, '2', mp_ftphandle)) return 0;
		return 1;
	}

	/*
	* disconnect from remote
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::Quit()
	{
		if (mp_ftphandle->dir != FTPLIB_CONTROL) return 0;
		if (mp_ftphandle->handle == 0)
		{
			strcpy(mp_ftphandle->response, "error: no anwser from server\n");
			return 0;
		}
		if (!FtpSendCmd("QUIT", '2', mp_ftphandle))
		{
			net_close(mp_ftphandle->handle);
			return 0;
		}
		else
		{
			net_close(mp_ftphandle->handle);
			return 1;
		}
	}

	inline void ftplib::SetCallbackIdleFunction(FtpCallbackIdle pointer)
	{
		mp_ftphandle->idlecb = pointer;
	}

	inline void ftplib::SetCallbackLogFunction(FtpCallbackLog pointer)
	{
		mp_ftphandle->logcb = pointer;
	}

	inline void ftplib::SetCallbackXferFunction(FtpCallbackXfer pointer)
	{
		mp_ftphandle->xfercb = pointer;
	}

	inline void ftplib::SetCallbackArg(void* arg)
	{
		mp_ftphandle->cbarg = arg;
	}

	inline void ftplib::SetCallbackBytes(off64_t bytes)
	{
		mp_ftphandle->cbbytes = bytes;
	}

	inline void ftplib::SetCallbackIdletime(int time)
	{
		mp_ftphandle->idletime.tv_sec = time / 1000;
		mp_ftphandle->idletime.tv_usec = (time % 1000) * 1000;
	}

	inline void ftplib::SetConnmode(connmode mode)
	{
		mp_ftphandle->cmode = mode;
	}

	inline void rola::ftplib::sprint_rest(char* buf, off64_t offset)
	{
#if defined(__APPLE__)
		sprintf(buf, "REST %lld", offset);
#else
		sprintf(buf, "REST %ld", offset);
#endif
	}

	inline void ftplib::ClearHandle()
	{
		mp_ftphandle->dir = FTPLIB_CONTROL;
		mp_ftphandle->ctrl = NULL;
		mp_ftphandle->cmode = ftplib::pasv;
		mp_ftphandle->idlecb = NULL;
		mp_ftphandle->idletime.tv_sec = mp_ftphandle->idletime.tv_usec = 0;
		mp_ftphandle->cbarg = NULL;
		mp_ftphandle->xfered = 0;
		mp_ftphandle->xfered1 = 0;
		mp_ftphandle->cbbytes = 0;

		mp_ftphandle->offset = 0;
		mp_ftphandle->handle = 0;
		mp_ftphandle->logcb = NULL;
		mp_ftphandle->xfercb = NULL;
		mp_ftphandle->correctpasv = false;
	}

	inline int ftplib::CorrectPasvResponse(unsigned char* v)
	{
		struct sockaddr ipholder;
		socklen_t ipholder_size = sizeof(ipholder);

		if (getpeername(mp_ftphandle->handle, &ipholder, &ipholder_size) == -1)
		{
			perror("getpeername");
			net_close(mp_ftphandle->handle);
			return 0;
		}

		for (int i = 2; i < 6; i++) v[i] = ipholder.sa_data[i];

		return 1;
	}

	/*
	* return a handle for a data stream
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::FtpAccess(const char* path, accesstype type, transfermode mode, ftphandle* nControl, ftphandle** nData)
	{
		char buf[256];
		int dir;

		if ((path == NULL) && ((type == ftplib::filewrite)
			|| (type == ftplib::fileread)
			|| (type == ftplib::filereadappend)
			|| (type == ftplib::filewriteappend)))
		{
			sprintf(nControl->response, "Missing path argument for file transfer\n");
			return 0;
		}
		sprintf(buf, "TYPE %c", mode);
		if (!FtpSendCmd(buf, '2', nControl)) return 0;

		switch (type)
		{
		case ftplib::dir:
			strcpy(buf, "NLST");
			dir = FTPLIB_READ;
			break;
		case ftplib::dirverbose:
			strcpy(buf, "LIST -aL");
			dir = FTPLIB_READ;
			break;
		case ftplib::filereadappend:
		case ftplib::fileread:
			strcpy(buf, "RETR");
			dir = FTPLIB_READ;
			break;
		case ftplib::filewriteappend:
		case ftplib::filewrite:
			strcpy(buf, "STOR");
			dir = FTPLIB_WRITE;
			break;
		default:
			sprintf(nControl->response, "Invalid open type %d\n", type);
			return 0;
		}
		if (path != NULL)
		{
			int i = strlen(buf);
			buf[i++] = ' ';
			if ((strlen(path) + i) >= sizeof(buf)) return 0;
			strcpy(&buf[i], path);
		}

		if (nControl->cmode == ftplib::pasv)
		{
			if (FtpOpenPasv(nControl, nData, mode, dir, buf) == -1) return 0;
		}

		if (nControl->cmode == ftplib::port)
		{
			if (FtpOpenPort(nControl, nData, mode, dir, buf) == -1) return 0;
			if (!FtpAcceptConnection(*nData, nControl))
			{
				FtpClose(*nData);
				*nData = NULL;
				return 0;
			}
		}
		return 1;
	}

	/*
	* close a data connection
	*/
	inline int ftplib::FtpClose(ftphandle* nData)
	{
		ftphandle* ctrl;

		if (nData->dir == FTPLIB_WRITE)
		{
			if (nData->buf != NULL) writeline(NULL, 0, nData);
		}
		else if (nData->dir != FTPLIB_READ) return 0;
		if (nData->buf) free(nData->buf);
		shutdown(nData->handle, 2);
		net_close(nData->handle);

		ctrl = nData->ctrl;
		free(nData);
		if (ctrl) return readresp('2', ctrl);
		return 1;
	}

	/*
	* socket_wait - wait for socket to receive or flush data
	*
	* return 1 if no user callback, otherwise, return value returned by
	* user callback
	*/
	inline int rola::ftplib::socket_wait(ftphandle* ctl)
	{
		fd_set fd, * rfd = NULL, * wfd = NULL;
		struct timeval tv;
		int rv = 0;

		if (ctl->idlecb == NULL) return 1;

		/*if ((ctl->dir == FTPLIB_CONTROL)
			|| (ctl->idlecb == NULL)
			|| ((ctl->idletime.tv_sec == 0)
			&& //(ctl->idletime.tv_usec 0))
		return 1;*/

		if (ctl->dir == FTPLIB_WRITE) wfd = &fd;
		else rfd = &fd;

		FD_ZERO(&fd);
		do
		{
			FD_SET(ctl->handle, &fd);
			tv = ctl->idletime;
			rv = select(ctl->handle + 1, rfd, wfd, NULL, &tv);
			if (rv == -1)
			{
				rv = 0;
				strncpy(ctl->ctrl->response, strerror(errno), sizeof(ctl->ctrl->response));
				break;
			}
			else if (rv > 0)
			{
				rv = 1;
				break;
			}
		} while ((rv = ctl->idlecb(ctl->cbarg)));

		return rv;
	}

	/*
	* read a line of text
	* return -1 on error or byte count
	*/
	inline int ftplib::readline(char* buf, int max, ftphandle* ctl)
	{
		int x, retval = 0;
		char* end, * bp = buf;
		int eof = 0;

		if ((ctl->dir != FTPLIB_CONTROL) && (ctl->dir != FTPLIB_READ))
			return -1;
		if (max == 0)
			return 0;
		do
		{
			if (ctl->cavail > 0)
			{
				x = (max >= ctl->cavail) ? ctl->cavail : max - 1;
				end = static_cast<char*>(memccpy(bp, ctl->cget, '\n', x));
				if (end != NULL)
					x = end - bp;
				retval += x;
				bp += x;
				*bp = '\0';
				max -= x;
				ctl->cget += x;
				ctl->cavail -= x;
				if (end != NULL)
				{
					bp -= 2;
					if (strcmp(bp, "\r\n") == 0)
					{
						*bp++ = '\n';
						*bp++ = '\0';
						--retval;
					}
					break;
				}
			}
			if (max == 1)
			{
				*buf = '\0';
				break;
			}
			if (ctl->cput == ctl->cget)
			{
				ctl->cput = ctl->cget = ctl->buf;
				ctl->cavail = 0;
				ctl->cleft = FTPLIB_BUFSIZ;
			}
			if (eof)
			{
				if (retval == 0)
					retval = -1;
				break;
			}

			if (!socket_wait(ctl)) return retval;
			x = net_read(ctl->handle, ctl->cput, ctl->cleft);

			if (x == -1)
			{
				perror("read");
				retval = -1;
				break;
			}

			// LOGGING FUNCTIONALITY!!!

			if ((ctl->dir == FTPLIB_CONTROL) && (mp_ftphandle->logcb != NULL))
			{
				*((ctl->cput) + x) = '\0';
				mp_ftphandle->logcb(ctl->cput, mp_ftphandle->cbarg, true);
			}

			if (x == 0) eof = 1;
			ctl->cleft -= x;
			ctl->cavail += x;
			ctl->cput += x;
		} while (1);
		return retval;
	}

	inline int ftplib::writeline(char* buf, int len, ftphandle* nData)
	{
		int x, nb = 0, w;
		char* ubp = buf, * nbp;
		char lc = 0;

		if (nData->dir != FTPLIB_WRITE)
			return -1;
		nbp = nData->buf;
		for (x = 0; x < len; x++)
		{
			if ((*ubp == '\n') && (lc != '\r'))
			{
				if (nb == FTPLIB_BUFSIZ)
				{
					if (!socket_wait(nData)) return x;
					w = net_write(nData->handle, nbp, FTPLIB_BUFSIZ);
					if (w != FTPLIB_BUFSIZ)
					{
						printf("write(1) returned %d, errno = %d\n", w, errno);
						return(-1);
					}
					nb = 0;
				}
				nbp[nb++] = '\r';
			}
			if (nb == FTPLIB_BUFSIZ)
			{
				if (!socket_wait(nData))
					return x;

				w = net_write(nData->handle, nbp, FTPLIB_BUFSIZ);
				if (w != FTPLIB_BUFSIZ)
				{
					printf("write(2) returned %d, errno = %d\n", w, errno);
					return(-1);
				}
				nb = 0;
			}
			nbp[nb++] = lc = *ubp++;
		}
		if (nb)
		{
			if (!socket_wait(nData)) return x;
			w = net_write(nData->handle, nbp, nb);
			if (w != nb)
			{
				printf("write(3) returned %d, errno = %d\n", w, errno);
				return(-1);
			}
		}
		return len;
	}

	/*
	* read a response from the server
	* return 0 if first char doesn't match
	* return 1 if first char matches
	*/
	inline int ftplib::readresp(char c, ftphandle* nControl)
	{
		char match[5];

		if (readline(nControl->response, 256, nControl) == -1)
		{
			perror("Control socket read failed");
			return 0;
		}

		if (nControl->response[3] == '-')
		{
			strncpy(match, nControl->response, 3);
			match[3] = ' ';
			match[4] = '\0';
			do
			{
				if (readline(nControl->response, 256, nControl) == -1)
				{
					perror("Control socket read failed");
					return 0;
				}
			} while (strncmp(nControl->response, match, 4));
		}
		if (nControl->response[0] == c) return 1;
		return 0;
	}

	/*
	* issue a command and transfer data
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::FtpXfer(const char* localfile, const char* path, ftphandle* nControl, accesstype type, transfermode mode)
	{
		int l, c;
		char* dbuf;
		FILE* local = NULL;
		ftphandle* nData;

		if (localfile != NULL)
		{
			char ac[3] = "  ";
			if ((type == ftplib::dir) || (type == ftplib::dirverbose)) { ac[0] = 'w'; ac[1] = '\0'; }
			if (type == ftplib::fileread) { ac[0] = 'w'; ac[1] = '\0'; }
			if (type == ftplib::filewriteappend) { ac[0] = 'r'; ac[1] = '\0'; }
			if (type == ftplib::filereadappend) { ac[0] = 'a'; ac[1] = '\0'; }
			if (type == ftplib::filewrite) { ac[0] = 'r'; ac[1] = '\0'; }
			if (mode == ftplib::image) ac[1] = 'b';

			local = fopen64(localfile, ac);
			if (local == NULL)
			{
				strncpy(nControl->response, strerror(errno), sizeof(nControl->response));
				return 0;
			}
			if (type == ftplib::filewriteappend) fseeko64(local, mp_ftphandle->offset, SEEK_SET);
		}
		if (local == NULL) local = ((type == ftplib::filewrite)
			|| (type == ftplib::filewriteappend)) ? stdin : stdout;
		if (!FtpAccess(path, type, mode, nControl, &nData)) {
			if (localfile != NULL) fclose(local);
			return 0;
		}

		dbuf = static_cast<char*>(malloc(FTPLIB_BUFSIZ));
		if ((type == ftplib::filewrite) || (type == ftplib::filewriteappend))
		{
			while ((l = fread(dbuf, 1, FTPLIB_BUFSIZ, local)) > 0)
			{
				if ((c = FtpWrite(dbuf, l, nData)) < l)
				{
					printf("short write: passed %d, wrote %d\n", l, c);
					break;
				}
			}
		}
		else
		{
			while ((l = FtpRead(dbuf, FTPLIB_BUFSIZ, nData)) > 0)
			{
				if (fwrite(dbuf, 1, l, local) <= 0)
				{
					perror("localfile write");
					break;
				}
			}
		}
		free(dbuf);
		fflush(local);
		if (localfile != NULL) fclose(local);
		return FtpClose(nData);
	}

	/*
	* establishes a PASV connection for data transfer
	* return 1 if successful, -1 otherwise
	*/
	inline int ftplib::FtpOpenPasv(ftphandle* nControl, ftphandle** nData, transfermode mode, int dir, char* cmd)
	{
		int sData;
		union {
			struct sockaddr sa;
			struct sockaddr_in in;
		} sin;
		struct linger lng = { 0, 0 };
		unsigned int l;
		int on = 1;
		ftphandle* ctrl;
		char* cp;
		unsigned char v[6];
		int ret;

		if (nControl->dir != FTPLIB_CONTROL) return -1;
		if ((dir != FTPLIB_READ) && (dir != FTPLIB_WRITE))
		{
			sprintf(nControl->response, "Invalid direction %d\n", dir);
			return -1;
		}
		if ((mode != ftplib::ascii) && (mode != ftplib::image))
		{
			sprintf(nControl->response, "Invalid mode %c\n", mode);
			return -1;
		}
		l = sizeof(sin);

		memset(&sin, 0, l);
		sin.in.sin_family = AF_INET;
		if (!FtpSendCmd("PASV", '2', nControl)) return -1;
		cp = strchr(nControl->response, '(');
		if (cp == NULL) return -1;
		cp++;
#if defined(_WIN32)
		unsigned int v_i[6];
		sscanf(cp, "%u,%u,%u,%u,%u,%u", &v_i[2], &v_i[3], &v_i[4], &v_i[5], &v_i[0], &v_i[1]);
		for (int i = 0; i < 6; i++) v[i] = (unsigned char)v_i[i];
#else
		sscanf(cp, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &v[2], &v[3], &v[4], &v[5], &v[0], &v[1]);
#endif
		if (nControl->correctpasv) if (!CorrectPasvResponse(v)) return -1;
		sin.sa.sa_data[2] = v[2];
		sin.sa.sa_data[3] = v[3];
		sin.sa.sa_data[4] = v[4];
		sin.sa.sa_data[5] = v[5];
		sin.sa.sa_data[0] = v[0];
		sin.sa.sa_data[1] = v[1];

		if (mp_ftphandle->offset != 0)
		{
			char buf[256];
			sprint_rest(buf, mp_ftphandle->offset);
			if (!FtpSendCmd(buf, '3', nControl)) return 0;
		}

		sData = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sData == -1)
		{
			perror("socket");
			return -1;
		}
		if (setsockopt(sData, SOL_SOCKET, SO_REUSEADDR, SETSOCKOPT_OPTVAL_TYPE & on, sizeof(on)) == -1)
		{
			perror("setsockopt");
			net_close(sData);
			return -1;
		}
		if (setsockopt(sData, SOL_SOCKET, SO_LINGER, SETSOCKOPT_OPTVAL_TYPE & lng, sizeof(lng)) == -1)
		{
			perror("setsockopt");
			net_close(sData);
			return -1;
		}

		if (nControl->dir != FTPLIB_CONTROL) return -1;
		memcpy(cmd + strlen(cmd), "\r\n\0", 3);
		ret = net_write(nControl->handle, cmd, strlen(cmd));
		if (ret <= 0)
		{
			perror("write");
			return -1;
		}

		if (connect(sData, &sin.sa, sizeof(sin.sa)) == -1)
		{
			perror("connect");
			net_close(sData);
			return -1;
		}
		if (!readresp('1', nControl))
		{
			net_close(sData);
			return -1;
		}
		ctrl = static_cast<ftphandle*>(calloc(1, sizeof(ftphandle)));
		if (ctrl == NULL)
		{
			perror("calloc");
			net_close(sData);
			return -1;
		}
		if ((mode == 'A') && ((ctrl->buf = static_cast<char*>(malloc(FTPLIB_BUFSIZ))) == NULL))
		{
			perror("calloc");
			net_close(sData);
			free(ctrl);
			return -1;
		}
		ctrl->handle = sData;
		ctrl->dir = dir;
		ctrl->ctrl = (nControl->cmode == ftplib::pasv) ? nControl : NULL;
		ctrl->idletime = nControl->idletime;
		ctrl->cbarg = nControl->cbarg;
		ctrl->xfered = 0;
		ctrl->xfered1 = 0;
		ctrl->cbbytes = nControl->cbbytes;
		if (ctrl->idletime.tv_sec || ctrl->idletime.tv_usec) ctrl->idlecb = nControl->idlecb;
		else ctrl->idlecb = NULL;
		if (ctrl->cbbytes) ctrl->xfercb = nControl->xfercb;
		else ctrl->xfercb = NULL;
		*nData = ctrl;

		return 1;
	}

	/*
	* send a command and wait for expected response
	* return 1 if proper response received, 0 otherwise
	*/
	inline int rola::ftplib::FtpSendCmd(const char* cmd, char expresp, ftphandle* nControl)
	{
		char buf[256];
		int x;

		if (!nControl->handle) return 0;

		if (nControl->dir != FTPLIB_CONTROL) return 0;
		sprintf(buf, "%s\r\n", cmd);

		x = net_write(nControl->handle, buf, strlen(buf));
		if (x <= 0)
		{
			perror("write");
			return 0;
		}

		if (mp_ftphandle->logcb != NULL) mp_ftphandle->logcb(buf, mp_ftphandle->cbarg, false);

		return readresp(expresp, nControl);
	}

	/*
	* accept connection from server
	* return 1 if successful, 0 otherwise
	*/
	inline int ftplib::FtpAcceptConnection(ftphandle* nData, ftphandle* nControl)
	{
		int sData;
		struct sockaddr addr;
		socklen_t l;
		int i;
		struct timeval tv;
		fd_set mask;
		int rv = 0;

		FD_ZERO(&mask);
		FD_SET(nControl->handle, &mask);
		FD_SET(nData->handle, &mask);
		tv.tv_usec = 0;
		tv.tv_sec = ACCEPT_TIMEOUT;
		i = nControl->handle;
		if (i < nData->handle) i = nData->handle;
		i = select(i + 1, &mask, NULL, NULL, &tv);

		if (i == -1)
		{
			strncpy(nControl->response, strerror(errno), sizeof(nControl->response));
			net_close(nData->handle);
			nData->handle = 0;
			rv = 0;
		}
		else if (i == 0)
		{
			strcpy(nControl->response, "timed out waiting for connection");
			net_close(nData->handle);
			nData->handle = 0;
			rv = 0;
		}
		else
		{
			if (FD_ISSET(nData->handle, &mask))
			{
				l = sizeof(addr);
				sData = accept(nData->handle, &addr, &l);
				i = errno;
				net_close(nData->handle);
				if (sData > 0)
				{
					rv = 1;
					nData->handle = sData;
					nData->ctrl = nControl;
				}
				else
				{
					strncpy(nControl->response, strerror(i), sizeof(nControl->response));
					nData->handle = 0;
					rv = 0;
				}
			}
			else if (FD_ISSET(nControl->handle, &mask))
			{
				net_close(nData->handle);
				nData->handle = 0;
				readresp('2', nControl);
				rv = 0;
			}
		}
		return rv;
	}

	/*
	* establishes a PORT connection for data transfer
	* return 1 if successful, -1 otherwise
	*/
	inline int ftplib::FtpOpenPort(ftphandle* nControl, ftphandle** nData, transfermode mode, int dir, char* cmd)
	{
		int sData;
		union {
			struct sockaddr sa;
			struct sockaddr_in in;
		} sin;
		struct linger lng = { 0, 0 };
		socklen_t l;
		int on = 1;
		ftphandle* ctrl;
		char buf[256];

		if (nControl->dir != FTPLIB_CONTROL) return -1;
		if ((dir != FTPLIB_READ) && (dir != FTPLIB_WRITE))
		{
			sprintf(nControl->response, "Invalid direction %d\n", dir);
			return -1;
		}
		if ((mode != ftplib::ascii) && (mode != ftplib::image))
		{
			sprintf(nControl->response, "Invalid mode %c\n", mode);
			return -1;
		}
		l = sizeof(sin);

		if (getsockname(nControl->handle, &sin.sa, &l) < 0)
		{
			perror("getsockname");
			return -1;
		}

		sData = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sData == -1)
		{
			perror("socket");
			return -1;
		}
		if (setsockopt(sData, SOL_SOCKET, SO_REUSEADDR, SETSOCKOPT_OPTVAL_TYPE & on, sizeof(on)) == -1)
		{
			perror("setsockopt");
			net_close(sData);
			return -1;
		}
		if (setsockopt(sData, SOL_SOCKET, SO_LINGER, SETSOCKOPT_OPTVAL_TYPE & lng, sizeof(lng)) == -1)
		{
			perror("setsockopt");
			net_close(sData);
			return -1;
		}

		sin.in.sin_port = 0;
		if (bind(sData, &sin.sa, sizeof(sin)) == -1)
		{
			perror("bind");
			net_close(sData);
			return -1;
		}
		if (listen(sData, 1) < 0)
		{
			perror("listen");
			net_close(sData);
			return -1;
		}
		if (getsockname(sData, &sin.sa, &l) < 0) return 0;
		sprintf(buf, "PORT %hhu,%hhu,%hhu,%hhu,%hhu,%hhu",
			(unsigned char)sin.sa.sa_data[2],
			(unsigned char)sin.sa.sa_data[3],
			(unsigned char)sin.sa.sa_data[4],
			(unsigned char)sin.sa.sa_data[5],
			(unsigned char)sin.sa.sa_data[0],
			(unsigned char)sin.sa.sa_data[1]);
		if (!FtpSendCmd(buf, '2', nControl))
		{
			net_close(sData);
			return -1;
		}

		if (mp_ftphandle->offset != 0)
		{
			char buf[256];
			sprint_rest(buf, mp_ftphandle->offset);
			if (!FtpSendCmd(buf, '3', nControl))
			{
				net_close(sData);
				return 0;
			}
		}

		ctrl = static_cast<ftphandle*>(calloc(1, sizeof(ftphandle)));
		if (ctrl == NULL)
		{
			perror("calloc");
			net_close(sData);
			return -1;
		}
		if ((mode == 'A') && ((ctrl->buf = static_cast<char*>(malloc(FTPLIB_BUFSIZ))) == NULL))
		{
			perror("calloc");
			net_close(sData);
			free(ctrl);
			return -1;
		}

		if (!FtpSendCmd(cmd, '1', nControl))
		{
			FtpClose(*nData);
			*nData = NULL;
			return -1;
		}

		ctrl->handle = sData;
		ctrl->dir = dir;
		ctrl->ctrl = (nControl->cmode == ftplib::pasv) ? nControl : NULL;
		ctrl->idletime = nControl->idletime;
		ctrl->cbarg = nControl->cbarg;
		ctrl->xfered = 0;
		ctrl->xfered1 = 0;
		ctrl->cbbytes = nControl->cbbytes;
		if (ctrl->idletime.tv_sec || ctrl->idletime.tv_usec) ctrl->idlecb = nControl->idlecb;
		else ctrl->idlecb = NULL;
		if (ctrl->cbbytes) ctrl->xfercb = nControl->xfercb;
		else ctrl->xfercb = NULL;
		*nData = ctrl;

		return 1;
	}

	/*
	* read from a data connection
	*/
	inline int ftplib::FtpRead(void* buf, int max, ftphandle* nData)
	{
		int i;

		if (nData->dir != FTPLIB_READ)
			return 0;
		if (nData->buf) i = readline(static_cast<char*>(buf), max, nData);
		else
		{
			i = socket_wait(nData);
			if (i != 1) return 0;
			i = net_read(nData->handle, buf, max);
		}
		if (i == -1) return 0;
		nData->xfered += i;
		if (nData->xfercb && nData->cbbytes)
		{
			nData->xfered1 += i;
			if (nData->xfered1 > nData->cbbytes)
			{
				if (nData->xfercb(nData->xfered, nData->cbarg) == 0) return 0;
				nData->xfered1 = 0;
			}
		}
		return i;
	}

	/*
	* write to a data connection
	*/
	inline int ftplib::FtpWrite(void* buf, int len, ftphandle* nData)
	{
		int i;

		if (nData->dir != FTPLIB_WRITE) return 0;
		if (nData->buf) i = writeline(static_cast<char*>(buf), len, nData);
		else
		{
			socket_wait(nData);
			i = net_write(nData->handle, buf, len);
		}
		if (i == -1) return 0;
		nData->xfered += i;

		if (nData->xfercb && nData->cbbytes)
		{
			nData->xfered1 += i;
			if (nData->xfered1 > nData->cbbytes)
			{
				if (nData->xfercb(nData->xfered, nData->cbarg) == 0) return 0;
				nData->xfered1 = 0;
			}
		}
		return i;
	}

	inline int rola::ftplib::Fxp(ftplib* src, ftplib* dst, const char* pathSrc, const char* pathDst, transfermode mode, fxpmethod method)
	{
		char* cp;
		unsigned char v[6];
		char buf[256];
		int retval = 0;

		sprintf(buf, "TYPE %c", mode);
		if (!dst->FtpSendCmd(buf, '2', dst->mp_ftphandle)) return -1;
		if (!src->FtpSendCmd(buf, '2', src->mp_ftphandle)) return -1;

		if (method == ftplib::defaultfxp)
		{
			// PASV dst

			if (!dst->FtpSendCmd("PASV", '2', dst->mp_ftphandle)) return -1;
			cp = strchr(dst->mp_ftphandle->response, '(');
			if (cp == NULL) return -1;
			cp++;
#if defined(_WIN32)
			unsigned int v_i[6];
			sscanf(cp, "%u,%u,%u,%u,%u,%u", &v_i[2], &v_i[3], &v_i[4], &v_i[5], &v_i[0], &v_i[1]);
			for (int i = 0; i < 6; i++) v[i] = (unsigned char)v_i[i];
#else
			sscanf(cp, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &v[2], &v[3], &v[4], &v[5], &v[0], &v[1]);
#endif
			if (dst->mp_ftphandle->correctpasv) if (!dst->CorrectPasvResponse(v)) return -1;

			// PORT src

			sprintf(buf, "PORT %d,%d,%d,%d,%d,%d", v[2], v[3], v[4], v[5], v[0], v[1]);
			if (!src->FtpSendCmd(buf, '2', src->mp_ftphandle)) return -1;

			// RETR src

			strcpy(buf, "RETR");
			if (pathSrc != NULL)
			{
				int i = strlen(buf);
				buf[i++] = ' ';
				if ((strlen(pathSrc) + i) >= sizeof(buf)) return 0;
				strcpy(&buf[i], pathSrc);
			}
			if (!src->FtpSendCmd(buf, '1', src->mp_ftphandle)) return 0;

			// STOR dst

			strcpy(buf, "STOR");
			if (pathDst != NULL)
			{
				int i = strlen(buf);
				buf[i++] = ' ';
				if ((strlen(pathDst) + i) >= sizeof(buf)) return 0;
				strcpy(&buf[i], pathDst);
			}
			if (!dst->FtpSendCmd(buf, '1', dst->mp_ftphandle))
			{
				/* this closes the data connection, to abort the RETR on
				the source FTP. all hail PFTP, it took me several
				hours and i was absolutely clueless, playing around with
				ABOR and whatever, when i desperately checked the PFTP
				source which gave me this final hint. thanks dude(s). */

				dst->FtpSendCmd("PASV", '2', dst->mp_ftphandle);
				src->readresp('4', src->mp_ftphandle);
				return 0;
			}

			retval = (src->readresp('2', src->mp_ftphandle)) & (dst->readresp('2', dst->mp_ftphandle));

		}
		else
		{
			// PASV src

			if (!src->FtpSendCmd("PASV", '2', src->mp_ftphandle)) return -1;
			cp = strchr(src->mp_ftphandle->response, '(');
			if (cp == NULL) return -1;
			cp++;
#if defined(_WIN32)
			unsigned int v_i[6];
			sscanf(cp, "%u,%u,%u,%u,%u,%u", &v_i[2], &v_i[3], &v_i[4], &v_i[5], &v_i[0], &v_i[1]);
			for (int i = 0; i < 6; i++) v[i] = (unsigned char)v_i[i];
#else
			sscanf(cp, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &v[2], &v[3], &v[4], &v[5], &v[0], &v[1]);
#endif
			if (src->mp_ftphandle->correctpasv) if (!src->CorrectPasvResponse(v)) return -1;

			// PORT dst

			sprintf(buf, "PORT %d,%d,%d,%d,%d,%d", v[2], v[3], v[4], v[5], v[0], v[1]);
			if (!dst->FtpSendCmd(buf, '2', dst->mp_ftphandle)) return -1;

			// STOR dest

			strcpy(buf, "STOR");
			if (pathDst != NULL)
			{
				int i = strlen(buf);
				buf[i++] = ' ';
				if ((strlen(pathDst) + i) >= sizeof(buf)) return 0;
				strcpy(&buf[i], pathDst);
			}
			if (!dst->FtpSendCmd(buf, '1', dst->mp_ftphandle)) return 0;

			// RETR src

			strcpy(buf, "RETR");
			if (pathSrc != NULL)
			{
				int i = strlen(buf);
				buf[i++] = ' ';
				if ((strlen(pathSrc) + i) >= sizeof(buf)) return 0;
				strcpy(&buf[i], pathSrc);
			}
			if (!src->FtpSendCmd(buf, '1', src->mp_ftphandle))
			{
				src->FtpSendCmd("PASV", '2', src->mp_ftphandle);
				dst->readresp('4', dst->mp_ftphandle);
				return 0;
			}

			// wait till its finished!

			retval = (src->readresp('2', src->mp_ftphandle)) & (dst->readresp('2', dst->mp_ftphandle));
		}

		return retval;
	}
	
	inline ftphandle* ftplib::RawOpen(const char* path, accesstype type, transfermode mode)
	{
		int ret;
		ftphandle* datahandle;
		ret = FtpAccess(path, type, mode, mp_ftphandle, &datahandle);
		if (ret) return datahandle;
		else return NULL;
	}
	
	inline int ftplib::RawClose(ftphandle* handle)
	{
		return FtpClose(handle);
	}
	
	inline int ftplib::RawWrite(void* buf, int len, ftphandle* handle)
	{
		return FtpWrite(buf, len, handle);
	}
	
	inline int ftplib::RawRead(void* buf, int max, ftphandle* handle)
	{
		return FtpRead(buf, max, handle);
	}

#pragma endregion

} // namespace rola

#endif // !ROLA_EASY_SOCKET_FTP_CLIENT_HPP
