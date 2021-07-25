#include "e4.h"

e4__usize e4__num_digit(e4__usize u, e4__u8 base, char *d)
{
    register const char digit = u % base;
    *d = digit < 10 ? '0' + digit : 'A' + digit - 10;
    return u / base;
}

char* e4__num_format(e4__usize n, e4__u8 base, e4__u8 flags, char *buf,
        e4__usize sz)
{
    register e4__bool negative = 0;
    register char *start = buf;

    /* This just works with the most negative number since NEGATE ends
       up doing nothing to change the number itself, but we'll prepend
       a minus sign to the number since we know it had its sign bit set
       prior to this operation. */
    if ((flags & e4__F_SIGNED) && (negative = e4__USIZE_IS_NEGATIVE(n)))
        n = e4__USIZE_NEGATE(n);

    buf += sz;
    while (buf > (start + negative))
        if (!(n = e4__num_digit(n, base, --buf)))
            break;

    if (negative)
        *--buf = '-';

    return buf;
}
