#include "tlpi.h"

extern char** environ;

int main_2procenvdisp(int argc, char* argv[])
{
#ifdef unix
	char** ep;
	for (ep = environ; *ep != NULL; ep++)
		puts(*ep);
	exit(EXIT_SUCCESS);

#endif
	return 0;
}
