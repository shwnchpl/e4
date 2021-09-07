#include "e4.h"

e4__usize e4__num_clz(e4__usize u)
{
    register e4__usize c, m, w;

    /* FIXME: It would be faster to use __builtin_clz where available.
       If/when there is some way to easily run tests with and without
       __GNU_C__ defined, consider conditionally using it here. */

    c = 0;
    w = e4__USIZE_BIT;
    m = (e4__usize)-1;

    if (!u)
        return w;

    while ((w >>= 1)) {
        if (!((m <<= w) & u)) {
            c += w;
            u <<= w;
        }
    }

    return c;
}

e4__usize e4__num_digit(e4__usize u, e4__u8 base, char *d)
{
    register const char digit = u % base;
    *d = digit < 10 ? '0' + digit : 'A' + digit - 10;
    return u / base;
}

struct e4__double e4__num_double(e4__usize low, e4__usize high)
{
    register struct e4__double d;
    d.low = low;
    d.high = high;
    return d;
}

struct e4__double e4__num_double_negate(struct e4__double d)
{
    d.high ^= (e4__usize)-1;
    d.low ^= (e4__usize)-1;
    d.high += !++d.low;
    return d;
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
        case e4__E_INVMEMADDR:
            _return_with_len("invalid memory address");
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
        case e4__E_USERINTERRUPT:
            _return_with_len("user interrupt");
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

struct e4__double e4__num_mul(e4__usize l, e4__usize r, e4__u8 flags)
{
    /* Flags includes e4__F_SIGNED. */
    register struct e4__double prod = {0};
    register e4__bool negate = 0;
    register e4__usize lh, ll, rh, rl, p0, p1, p2, c0, c1, c2, s0;

    #define _e4__U_LMASK    (((e4__usize)-1) >> (e4__USIZE_BIT >> 1))
    #define _e4__U_MMASK    (((e4__usize)-1) << (e4__USIZE_BIT >> 1))
    #define _e4__U_LSH(u)   ((u) & _e4__U_LMASK)
    #define _e4__U_MSH(u)   (((u) & _e4__U_MMASK) >> (e4__USIZE_BIT >> 1))
    #define _e4__U(m, l)    \
        (((m) << (e4__USIZE_BIT >> 1) & _e4__U_MMASK) | (l & _e4__U_LMASK))
    #define _e4__U_SPLIT(u, h, l)   \
        h = _e4__U_MSH(u), l = _e4__U_LSH(u)

    if (flags & e4__F_SIGNED) {
        if (e4__USIZE_IS_NEGATIVE(l)) {
            l = e4__USIZE_NEGATE(l);
            negate = !negate;
        }

        if (e4__USIZE_IS_NEGATIVE(r)) {
            r = e4__USIZE_NEGATE(r);
            negate = !negate;
        }
    }

    /* Perform the basic multiplication algorithm. */
    _e4__U_SPLIT(l, lh, ll);
    _e4__U_SPLIT(r, rh, rl);

    _e4__U_SPLIT(ll * rl, c0, p0);
    _e4__U_SPLIT(ll * rh + c0, c1, p1);
    _e4__U_SPLIT(lh * rl, c2, p2);

    s0 = _e4__U(p1, p0);

    prod.low = s0 + _e4__U(p2, 0);
    prod.high = c1 + c2 + lh * rh + (prod.low < s0);

    if (negate) {
        prod = e4__num_double_negate(prod);
    }

    #undef _e4__U_SPLIT
    #undef _e4__U
    #undef _e4__U_MSH
    #undef _e4__U_LSH
    #undef _e4__U_MMASK
    #undef _e4__U_LMASK

    return prod;
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

struct e4__double e4__num_todouble(e4__usize n)
{
    struct e4__double d;
    d.low = n;
    d.high = e4__USIZE_IS_NEGATIVE(d.low) ? -1 : 0;
    return d;
}
