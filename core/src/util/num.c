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

const char* e4__num_format_exception(e4__usize e, e4__usize *len)
{
    #define _return_with_len(s) \
        do {    \
            if (len)    \
                *len = sizeof(s) - 1;   \
            return s;   \
        } while (0)

    switch (e) {
        /* error constants - standard */
        case e4__E_OK:
            _return_with_len("ok");
        case e4__E_ABORT:
            _return_with_len("abort");
        case e4__E_STKUNDERFLOW:
            _return_with_len("stack underflow");
        case e4__E_RSTKUNDERFLOW:
            _return_with_len("return stack underflow");
        case e4__E_DIVBYZERO:
            _return_with_len("division by zero");
        case e4__E_UNDEFWORD:
            _return_with_len("undefined word");
        case e4__E_COMPONLYWORD:
            _return_with_len("interpreting a compile-only word");
        case e4__E_INVFORGET:
            _return_with_len("invalid forget");
        case e4__E_ZLNAME:
            _return_with_len("attempt to use zero-length string as a name");
        case e4__E_UNSUPPORTED:
            _return_with_len("unsupported operation");
        case e4__E_CSMISMATCH:
            _return_with_len("control structure mismatch");
        case e4__E_RSTKIMBALANCE:
            _return_with_len("return stack imbalance");
        case e4__E_NESTEDCOMPILE:
            _return_with_len("compiler nesting");
        case e4__E_INVNAMEARG:
            _return_with_len("invalid name argument");
        case e4__E_QUIT:
            _return_with_len("quit");

        /* error constants - system */
        case e4__E_FAILURE:
            _return_with_len("generic failure");
        case e4__E_BYE:
            _return_with_len("bye");
        case e4__E_BUG:
            _return_with_len("system bug");
        case e4__E_INVBUILTINMUT:
            _return_with_len("attempt to mutate builtin");

        /* unknown */
        default:
            break;
    }

    _return_with_len("unknown");

    #undef _return_with_len
}

e4__usize e4__num_sdiv(e4__usize n, e4__usize d)
{
    register e4__bool negate = 0;

    if (e4__USIZE_IS_NEGATIVE(n)) {
        n = e4__USIZE_NEGATE(n);
        negate = !negate;
    }

    if (e4__USIZE_IS_NEGATIVE(d)) {
        d = e4__USIZE_NEGATE(d);
        negate = !negate;
    }

    n = n / d;

    if (negate)
        n = e4__USIZE_NEGATE(n);

    return n;
}

e4__usize e4__num_smod(e4__usize n, e4__usize d)
{
    register e4__bool negate = 0;

    if (e4__USIZE_IS_NEGATIVE(n)) {
        n = e4__USIZE_NEGATE(n);
        negate = 1;
    }

    if (e4__USIZE_IS_NEGATIVE(d)) {
        d = e4__USIZE_NEGATE(d);
    }

    n = n % d;

    if (negate)
        n = e4__USIZE_NEGATE(n);

    return n;
}
