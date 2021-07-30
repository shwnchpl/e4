#include "e4.h"
#include "e4-task.h"
#include <string.h>
#include <ctype.h>

int e4__mem_strncasecmp(const char *left, const char *right, e4__usize n)
{
    while (n-- > 0 && tolower(*left++) == tolower(*right++))
        if (!left[-1])
            return 0;

    return !++n ? 0 : tolower(left[-1]) - tolower(right[-1]);
}

e4__usize e4__mem_number(const char *buf, e4__usize length, e4__u8 base,
        e4__u8 flags, e4__usize *out)
{
    register e4__bool number;
    register e4__bool negate;
    register e4__usize result = 0;
    register const char *start = buf;

    /* XXX: Ambiguous behavior. This implementation clamps to the
       acceptable range. */
    if (base < 2) base = 2;
    if (base > 36) base = 36;

    if (flags & e4__F_CHAR_LITERAL && length > 2 && buf[0] == '\'' &&
            buf[2] == '\'') {
        *out = buf[1];
        return 3;
    }

    if (flags & e4__F_NEG_PREFIX && *buf == '-') {
        negate = 1;
        ++buf;
    }

    if (flags & e4__F_BASE_PREFIX)
        switch (*buf++) {
            case '0':
                switch (*buf++) {
                    case 'x': case 'X': base = 16; break;
                    case 'o': case 'O': base = 8; break;
                    case 'b': case 'B': base = 2; break;
                    default: buf -= 2; break;
                }
                break;
            case '$': base = 16; break;
            case '#': base = 10; break;
            case '%': base = 2; break;
            default: --buf; break;
        }

    for (number = 0; buf - start < length; number = 1, ++buf) {
        register e4__usize d;
        register const char c = tolower(*buf);

        if (isdigit(c))
            d = c - '0';
        else if (c >= 'a' && c <= 'z')
            d = c - 'a' + 10;
        else
            break;

        if (d >= base)
            break;

        result *= base;
        result += d;
    }

    if (number) {
        if (negate)
            result = e4__USIZE_NEGATE(result);
        *out = result;
        return buf - start;
    }

    return 0;
}

const char* e4__mem_parse(const char *buf, char delim, e4__usize size,
        e4__usize flags, e4__usize *length)
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
