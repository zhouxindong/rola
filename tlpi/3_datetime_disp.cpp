
#include <locale.h>
#include <time.h>
#ifdef unix
#include <sys/time.h>
#endif
#include "tlpi.h"

#define SECONDS_IN_TROPICAL_YEAR (365.24219*24*60*60)
#define BUF_SIZE 1000

char* currTime(const char* format)
{
	static char buf[BUF_SIZE];  /* Nonreentrant */
	time_t t;
	size_t s;
	struct tm* tm;

	t = time(NULL);
	tm = localtime(&t);
	if (tm == NULL)
		return NULL;

	s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);
	return (s == 0) ? NULL : buf;
}


int main(int argc, char* argv[])
{
#ifdef unix

	time_t t;
	struct tm* gmp;
	struct tm* locp;
	struct tm gm, loc;
	struct timeval tv;

	t = time(NULL);
	printf("Seconds since the Epoch (1 Jan 1970): %ld", (long)t);
	printf(" (about %6.3f years)\n", t / SECONDS_IN_TROPICAL_YEAR);

	if (gettimeofday(&tv, NULL) == -1)
		errExit("gettimeofday");
	printf(" gettimeofday() returned %ld secs, %ld microsecs\n", (long)tv.tv_sec, (long)tv.tv_usec);

	gmp = gmtime(&t);
	if (gmp == NULL)
		errExit("gmtime");

	gm = *gmp; /* save local copy, since *gmp may be modified by asctime() or gmtime() */
	printf("Broken down by gmtime():\n");
	printf(" year=%d mon=%d mday=%d hour=%d min=%d sec=%d ", gm.tm_year,
		gm.tm_mon, gm.tm_mday, gm.tm_hour, gm.tm_min, gm.tm_sec);
	printf("wday=%d yday=%d isdst=%d\n", gm.tm_wday, gm.tm_yday, gm.tm_isdst);

	locp = localtime(&t);
	if (locp == NULL)
		errExit("localtime");

	loc = *locp;    /* save local copy */
	printf("Broken down by localtime():\n");
	printf(" year=%d mon=%d mday=%d hour%d min=%d sec=%d ",
		loc.tm_year, loc.tm_mon, loc.tm_mday,
		loc.tm_hour, loc.tm_min, loc.tm_sec);
	printf("wday=%d yday=%d isdst=%d\n\n",
		loc.tm_wday, loc.tm_yday, loc.tm_isdst);

	printf("asctime() formats the gmtime() value as:    %s", asctime(&gm));
	printf("ctime() formats the time() value as:        %s", ctime(&t));

	printf("mktime() of gmtime() value:     %ld secs\n", (long)mktime(&gm));
	printf("mktime() of localtime() value:  %ld secs\n", (long)mktime(&loc));

	printf("currTime(NULL): %s\n", currTime(NULL));

	exit(EXIT_SUCCESS);
#endif
	return 0;
}
