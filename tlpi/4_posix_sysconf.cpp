#include "tlpi.h"

#ifdef unix

// print 'msg' plus sysconf() value for 'name'
static void sysconfPrint(const char* msg, int name)
{
	long lim;
	errno = 0;
	lim = sysconf(name);
	if (lim != -1) // call succeeded, limit determinate
	{
		printf("%s %ld\n", msg, lim);
	}
	else {
		if (errno == 0) // call succeeded, limit indeterminate
			printf("%s (indeterminate)\n", msg);
		else
			errExit("sysconf %s", msg);
	}
}

static void fpathconfPrint(const char* msg, int fd, int name)
{
	long lim;
	errno = 0;
	lim = fpathconf(fd, name);
	if (lim != -1)
		printf("%s %ld\n", msg, lim);
	else {
		if (errno == 0)
			printf("%s (indeterminate)\n", msg);
		else
			errExit("fpathconf %s", msg);
	}
}

int main(int argc, char* argv[])
{
	sysconfPrint("_SC_ARG_MAX:          ", _SC_ARG_MAX);
	sysconfPrint("_SC_LOGIN_NAME_MAX:   ", _SC_LOGIN_NAME_MAX);
	sysconfPrint("_SC_OPEN_MAX:         ", _SC_OPEN_MAX);
	sysconfPrint("_SC_NGROUPS_MAX:      ", _SC_NGROUPS_MAX);
	sysconfPrint("_SC_PAGESIZE:         ", _SC_PAGESIZE);
	sysconfPrint("_SC_RTSIG_MAX:        ", _SC_RTSIG_MAX);

	fpathconfPrint("_PC_NAME_MAX:   ", STDIN_FILENO, _PC_NAME_MAX);
	fpathconfPrint("_PC_PATH_MAX:   ", STDIN_FILENO, _PC_PATH_MAX);
	fpathconfPrint("_PC_PIPE_BUF:   ", STDIN_FILENO, _PC_PIPE_BUF);

	exit(EXIT_SUCCESS);
}

#endif