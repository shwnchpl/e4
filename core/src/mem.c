#include "e4.h"
#include "e4-task.h"
#include <string.h>
#include <ctype.h>

int e4__mem_strncasecmp(const char *left, const char *right, unsigned long n)
{
    while (n-- > 0 && tolower(*left++) == tolower(*right++))
        if (!*left)
            return 0;

    return !++n ? 0 : tolower(*--left) - tolower(*--right);
}

const char* e4__mem_parse(const char *buf, char delim, unsigned long size,
        unsigned long flags, unsigned long *length)
{
    register const char* start;

    if (flags & e4__F_SKIP_LEADING)
        while (size && *buf == delim)
            ++buf, --size;

    start = buf;
    *length = 0;

    /* FIXME: Consider wrapping the end on newline behavior around some
       kind of flag. */
    while (size && *buf != delim && *buf != '\n')
        ++buf, --size, ++*length;

    return start;
}
