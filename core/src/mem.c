#include "e4.h"
#include "e4-task.h"
#include <string.h>

char* e4__mem_word(struct e4__task *task, char delim, const char *buf)
{
    register const char *start;
    register char *p = (char*)task->pad;

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
