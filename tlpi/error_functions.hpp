#ifndef ROLA_TLPI_ERROR_FUNCTIONS_HPP
#define ROLA_TLPI_ERROR_FUNCTIONS_HPP

#include <stdarg.h>
#include "ename.c.inc"
#include "tlpi_hdr.h"

#ifdef __GNUC__

	/* This macro stops 'gcc -Wall' complaining that "control reaches
	   end of non-void function" if we use the following functions to
	   terminate main() or some other non-void function. */

#define NORETURN __attribute__ ((__noreturn__))
#else
#define NORETURN
#endif

#ifdef __GNUC__                 /* Prevent 'gcc -Wall' complaining  */
__attribute__((__noreturn__))  /* if we call this function as last */
#endif 

static void
terminate(Boolean useExit3)
{
	char* s;

	/* Dump core if EF_DUMPCORE environment variable is defined and
	   is a nonempty string; otherwise call exit(3) or _exit(2),
	   depending on the value of 'useExit3'. */

	s = getenv("EF_DUMPCORE");

	if (s != NULL && *s != '\0')
		abort();
	else if (useExit3)
		exit(EXIT_FAILURE);
	else
		_exit(EXIT_FAILURE);
}

/* Diagnose 'errno' error by:

	  * outputting a string containing the error name (if available
		in 'ename' array) corresponding to the value in 'err', along
		with the corresponding error message from strerror(), and

	  * outputting the caller-supplied error message specified in
		'format' and 'ap'. */

static void
outputError(Boolean useErr, int err, Boolean flushStdout,
	const char* format, va_list ap)
{
#define BUF_SIZE 500
	char buf[BUF_SIZE], userMsg[BUF_SIZE], errText[BUF_SIZE];

	vsnprintf(userMsg, BUF_SIZE, format, ap);

	if (useErr)
		snprintf(errText, BUF_SIZE, " [%s %s]",
			(err > 0 && err <= MAX_ENAME) ?
			ename[err] : "?UNKNOWN?", strerror(err));
	else
		snprintf(errText, BUF_SIZE, ":");

#if __GNUC__ >= 7
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
#endif
	snprintf(buf, BUF_SIZE, "ERROR%s %s\n", errText, userMsg);
#if __GNUC__ >= 7
#pragma GCC diagnostic pop
#endif

	if (flushStdout)
		fflush(stdout);       /* Flush any pending stdout */
	fputs(buf, stderr);
	fflush(stderr);           /* In case stderr is not line-buffered */
}

//namespace rola
//{
//	namespace tlpi
//	{
		/* Display error message including 'errno' diagnostic, and
		return to caller */

		inline void errMsg(const char* format, ...)
		{
			va_list argList;
			int savedErrno;

			savedErrno = errno;       /* In case we change it here */

			va_start(argList, format);
			outputError(TRUE, errno, TRUE, format, argList);
			va_end(argList);

			errno = savedErrno;
		}

		/* Display error message including 'errno' diagnostic, and
		terminate the process */
                inline void errExit(const char* format, ...)
		{
			va_list argList;

			va_start(argList, format);
			outputError(TRUE, errno, TRUE, format, argList);
			va_end(argList);

			terminate(TRUE);
		}

                inline void err_exit(const char* format, ...)
		{
			va_list argList;

			va_start(argList, format);
			outputError(TRUE, errno, FALSE, format, argList);
			va_end(argList);

			terminate(FALSE);
		}

                inline void errExitEN(int errnum, const char* format, ...)
		{
			va_list argList;

			va_start(argList, format);
			outputError(TRUE, errnum, TRUE, format, argList);
			va_end(argList);

			terminate(TRUE);
		}

                inline void fatal(const char* format, ...)
		{
			va_list argList;

			va_start(argList, format);
			outputError(FALSE, 0, TRUE, format, argList);
			va_end(argList);

			terminate(TRUE);
		}

                inline void usageErr(const char* format, ...)
		{
			va_list argList;

			fflush(stdout);           /* Flush any pending stdout */

			fprintf(stderr, "Usage: ");
			va_start(argList, format);
			vfprintf(stderr, format, argList);
			va_end(argList);

			fflush(stderr);           /* In case stderr is not line-buffered */
			exit(EXIT_FAILURE);
		}

                inline void cmdLineErr(const char* format, ...)
		{
			va_list argList;

			fflush(stdout);           /* Flush any pending stdout */

			fprintf(stderr, "Command-line usage error: ");
			va_start(argList, format);
			vfprintf(stderr, format, argList);
			va_end(argList);

			fflush(stderr);           /* In case stderr is not line-buffered */
			exit(EXIT_FAILURE);
		}
//	} // namespace tlpi
//} // namespace rola

#endif // ROLA_TLPI_ERROR_FUNCTIONS_HPP
