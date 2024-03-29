#define _GNU_SOURCE /* to get various declarations from <stdlib.h> */
#include <stdlib.h>
#include "tlpi.h"

extern char** environ;

int main_2procenvmodify(int argc, char* argv[])
{
#ifdef unix

	int j;
	char** ep;
	clearenv();     /* erase entire environment */

	for (j = 1; j < argc; j++)
	{
		if (putenv(argv[j]) != 0)
			errExit("putenv: %s", argv[j]);
	}

	if (setenv("GREET", "Hello world", 0) == -1)
		errExit("setenv");

	unsetenv("BYE");

	for (ep = environ; *ep != NULL; ep++)
		puts(*ep);

	exit(EXIT_SUCCESS);

#endif
	return 0;
}
