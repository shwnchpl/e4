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

char* e4__mem_word(struct e4__task *task, char delim, const char *buf)
{
    register const char *start;
    register char *p = (char*)task->pob;

    while (*buf == delim)
        ++buf;
    start = buf;

    /* FIXME: This may not play nice with \r\n style newlines.
       I guess maybe this is implementation dependent. */
    while (*buf != delim && buf - start < 256 && *buf && *buf != '\n')
        ++buf;

    *p = (char)(buf - start);
    memcpy(&p[1], start, *p);

    return p;
}
