#ifndef ROLA_TLPI_GET_NUM_HPP
#define ROLA_TLPI_GET_NUM_HPP

#define GN_NONNEG       01      /* Value must be >= 0 */
#define GN_GT_0         02      /* Value must be > 0 */

/* By default, integers are decimal */
#define GN_ANY_BASE   0100      /* Can use any base - like strtol(3) */
#define GN_BASE_8     0200      /* Value is expressed in octal */
#define GN_BASE_16    0400      /* Value is expressed in hexadecimal */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <errno.h>


/* Print a diagnostic message that contains a function name ('fname'),
   the value of a command-line argument ('arg'), the name of that
   command-line argument ('name'), and a diagnostic error message ('msg'). */

static void
gnFail(const char* fname, const char* msg, const char* arg, const char* name)
{
    fprintf(stderr, "%s error", fname);
    if (name != NULL)
        fprintf(stderr, " (in %s)", name);
    fprintf(stderr, ": %s\n", msg);
    if (arg != NULL && *arg != '\0')
        fprintf(stderr, "        offending text: %s\n", arg);

    exit(EXIT_FAILURE);
}

/* Convert a numeric command-line argument ('arg') into a long integer,
   returned as the function result. 'flags' is a bit mask of flags controlling
   how the conversion is done and what diagnostic checks are performed on the
   numeric result; see get_num.h for details.

   'fname' is the name of our caller, and 'name' is the name associated with
   the command-line argument 'arg'. 'fname' and 'name' are used to print a
   diagnostic message in case an error is detected when processing 'arg'. */

static long
getNum(const char* fname, const char* arg, int flags, const char* name)
{
    long res;
    char* endptr;
    int base;

    if (arg == NULL || *arg == '\0')
        gnFail(fname, "null or empty string", arg, name);

    base = (flags & GN_ANY_BASE) ? 0 : (flags & GN_BASE_8) ? 8 :
        (flags & GN_BASE_16) ? 16 : 10;

    errno = 0;
    res = strtol(arg, &endptr, base);
    if (errno != 0)
        gnFail(fname, "strtol() failed", arg, name);

    if (*endptr != '\0')
        gnFail(fname, "nonnumeric characters", arg, name);

    if ((flags & GN_NONNEG) && res < 0)
        gnFail(fname, "negative value not allowed", arg, name);

    if ((flags & GN_GT_0) && res <= 0)
        gnFail(fname, "value must be > 0", arg, name);

    return res;
}

//namespace rola
//{
//	namespace tlpi
//	{
        /* Convert a numeric command-line argument string to a long integer. See the
        comments for getNum() for a description of the arguments to this function. */
        inline long getLong(const char* arg, int flags, const char* name)
        {
            return getNum("getLong", arg, flags, name);
        }

        /* Convert a numeric command-line argument string to an integer. See the
        comments for getNum() for a description of the arguments to this function. */
        inline int getInt(const char* arg, int flags, const char* name)
        {
			long res;

			res = getNum("getInt", arg, flags, name);

			if (res > INT_MAX || res < INT_MIN)
				gnFail("getInt", "integer out of range", arg, name);

			return (int)res;
        }
//	} // namespace tlpi
//} // namespace rola

#endif // ROLA_TLPI_GET_NUM_HPP
