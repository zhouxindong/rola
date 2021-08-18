#include <stdio.h>
#include <stdlib.h>
#include "tlpi.h"

int main_1fileiotemp(int argc, char* argv[])
{
#ifdef unix
	int fd;
	char templ[] = "/tmp/somestringXXXXXX";
	fd = mkstemp(templ);
	if (fd == -1)
		errExit("mkstemp");

	printf("Generated filename was: %s\n", templ);
	unlink(templ); /* Name disappers immediately, but the file is removed only after close() */

	/* use file I/O system calls - read(), write(), and so on */
	if (close(fd) == -1)
		errExit("close");
#endif
	return 0;
}