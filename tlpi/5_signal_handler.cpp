#ifdef unix

#include <signal.h>
#include "tlpi.h"

static void sigHandler(int sig)
{
	printf("Ouch\n");
}

int main(int argc, char* argv[])
{
	int j;

	if (signal(SIGINT, sigHandler) == SIG_ERR)
		errExit("signal");

	for (j = 0;; j++)
	{
		printf("%d\n", j);
		sleep(3);
	}
}

#endif

#ifdef unix

#include <signal.h>
#include "tlpi.h"

static void sigHandler(int sig)
{
	static int count = 0;

	if (sig == SIGINT) // Ctrl+C
	{
		count++;
		printf("Caught SIGINT (%d)\n", count);
		return; // resume execution at point of interruption
	}

	if (sig == SIGQUIT) /* Ctrl + \ */
	{
		printf("Caught SIGQUIT - that's all folks!\n");
		exit(EXIT_SUCCESS);
	}
}

int main(int argc, char* argv[])
{
	int j;

	if (signal(SIGINT, sigHandler) == SIG_ERR)
		errExit("signal");
	if (signal(SIGQUIT, sigHandler) == SIG_ERR)
		errExit("signal");

	for (;;)
		pause(); /* block until a signal is caught */
}

#endif
