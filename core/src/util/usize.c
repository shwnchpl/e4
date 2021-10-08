#include "e4.h"

e4__usize e4__usize_clz(e4__usize u)
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

e4__usize e4__usize_digit(e4__usize u, e4__u8 base, char *d)
{
    register const char digit = u % base;
    *d = digit < 10 ? '0' + digit : 'A' + digit - 10;
    return u / base;
}

char* e4__usize_format(e4__usize n, e4__u8 base, e4__u8 flags, char *buf,
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
        if (!(n = e4__usize_digit(n, base, --buf)))
            break;

    if (negative)
        *--buf = '-';

    return buf;
}

const char* e4__usize_format_exception(e4__usize e, e4__usize *len)
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
        case e4__E_ABORTQ:
            _return_with_len("abort with message");
        case e4__E_STKOVERFLOW:
            _return_with_len("stack overflow");
        case e4__E_STKUNDERFLOW:
            _return_with_len("stack underflow");
        case e4__E_RSTKUNDERFLOW:
            _return_with_len("return stack underflow");
        case e4__E_DICTOVERFLOW:
            _return_with_len("dictionary overflow");
        case e4__E_INVMEMADDR:
            _return_with_len("invalid memory address");
        case e4__E_DIVBYZERO:
            _return_with_len("division by zero");
        case e4__E_RSLTOUTORANGE:
            _return_with_len("result out of range");
        case e4__E_UNDEFWORD:
            _return_with_len("undefined word");
        case e4__E_COMPONLYWORD:
            _return_with_len("interpreting a compile-only word");
        case e4__E_INVFORGET:
            _return_with_len("invalid forget");
        case e4__E_ZLNAME:
            _return_with_len("attempt to use zero-length string as a name");
        case e4__E_PNOOVERFLOW:
            _return_with_len("pictured numeric output string overflow");
        case e4__E_PSTROVERFLOW:
            _return_with_len("parsed string overflow");
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
        case e4__E_DICTUNDERFLOW:
            _return_with_len("dictionary underflow");

        /* unknown */
        default:
            break;
    }

    _return_with_len("unknown");

    #undef _return_with_len
}

struct e4__double e4__usize_mul(e4__usize l, e4__usize r, e4__u8 flags)
{
    /* Flags includes e4__F_SIGNED. */
    register struct e4__double prod = {0};
    register e4__bool negate = 0;
    register e4__usize lh, ll, rh, rl, p0, p1, p2, c0, c1, c2, s0;

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
    e4__USIZE_SPLIT(l, lh, ll);
    e4__USIZE_SPLIT(r, rh, rl);

    e4__USIZE_SPLIT(ll * rl, c0, p0);
    e4__USIZE_SPLIT(ll * rh + c0, c1, p1);
    e4__USIZE_SPLIT(lh * rl, c2, p2);

    s0 = e4__USIZE_U(p1, p0);

    prod.low = s0 + e4__USIZE_U(p2, 0);
    prod.high = c1 + c2 + lh * rh + (prod.low < s0);

    if (negate) {
        prod = e4__double_negate(prod);
    }

    return prod;
}

e4__usize e4__usize_sdiv(e4__usize n, e4__usize d, e4__usize *r)
{
    register e4__usize q;
    register e4__bool n_negative = 0;
    register e4__bool d_negative = 0;

    if ((n_negative = e4__USIZE_IS_NEGATIVE(n)))
        n = e4__USIZE_NEGATE(n);

    if ((d_negative = e4__USIZE_IS_NEGATIVE(d)))
        d = e4__USIZE_NEGATE(d);

    q = n / d;
    if (r)
        *r = n % d;

    if (n_negative ^ d_negative)
        q = e4__USIZE_NEGATE(q);

    if (r && n_negative)
        *r = e4__USIZE_NEGATE(*r);

    return q;
}

struct e4__double e4__usize_todouble(e4__usize n)
{
    struct e4__double d;
    d.low = n;
    d.high = e4__USIZE_IS_NEGATIVE(d.low) ? -1 : 0;
    return d;
}

void e4__usize_togmt(e4__usize t, struct e4__gmt *gmt)
{
    static const e4__usize days_per_fyear = 365 * 4 + 1;
    static const e4__usize days_per_cent = 25 * days_per_fyear - 1;
    static const e4__usize days_per_qcent = days_per_cent * 4 + 1;
    static const e4__u8 month_days[] = {
        31, 30, 31, 30, 31, 31,
        30, 31, 30, 31, 31, 29,
    };
    register e4__usize days, fyears, cents, qcents;
    register e4__bool negated = 0;

    /* FIXME: Consider only defining this utility when
       e4__INCLUDE_FACILITY_EXT has been defined. */

    /* XXX: Does not account for leap seconds. */

    gmt->sec = t % 60;
    gmt->min = (t / 60) % 60;
    gmt->hour = (t / 3600) % 24;

    days = t / 86400;

    /* XXX: Normalize to March 1st, 2000. This may result in t going
       negative. If it does, that's alright; we will handle this case
       when writing final values. */
    days -= 11017;
    if ((negated = e4__USIZE_IS_NEGATIVE(days)))
        days = e4__USIZE_NEGATE(days);

    qcents = days / days_per_qcent;
    days %= days_per_qcent;

    cents = days / days_per_cent;
    days %= days_per_cent;

    fyears = days / days_per_fyear;
    days %= days_per_fyear;

    gmt->year = days / 365 + 4 * fyears + 100 * cents + 400 * qcents + negated;
    if (negated)
        gmt->year = e4__USIZE_NEGATE(gmt->year);
    gmt->year += 2000;

    days %= 365;
    if (negated) {
        /* For the years that would require us to have negated, every
           four year set contains a leap year, so we can simply switch
           to a floored modulo and add one day to account for the leap
           day. */
        days = e4__USIZE_NEGATE(days) + 366;
    }

    if (!days && (fyears || !(cents % 4))) {
        gmt->mon = 2;
        gmt->mday = 29;
    } else {
        for (gmt->mon = 0; gmt->mon < 12 && days >= month_days[gmt->mon];
                ++gmt->mon)
            days -= month_days[gmt->mon];

        gmt->year += (gmt->mon > 9);
        gmt->mon = ((gmt->mon + 2) % 12) + 1;
        gmt->mday = (e4__u8)days + 1;
    }
}
