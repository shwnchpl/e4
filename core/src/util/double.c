#include "e4.h"

struct e4__double e4__double_u(e4__usize low, e4__usize high)
{
    register struct e4__double d;
    d.low = low;
    d.high = high;
    return d;
}

e4__usize e4__double_div(struct e4__double n, e4__usize d, e4__usize flags,
        struct e4__double *q, e4__usize *r)
{
    register e4__usize res;
    register e4__bool negate = 0;
    struct e4__double q_ = {0};
    e4__usize r_;

    /* Based on Hank Warren's unsigned doubleword division reference
       implementation in "Hacker's Delight" 2nd Edition (Figure 9-5). */

    /* FIXME: Handle flags. */

    if (flags & e4__F_SIGNED) {
        if (e4__USIZE_IS_NEGATIVE(n.high)) {
            n = e4__double_negate(n);
            negate = !negate;
        }

        if (e4__USIZE_IS_NEGATIVE(d)) {
            d = e4__USIZE_NEGATE(d);
            negate = !negate;
        }
    }

    if (n.high < d) {
        if ((res = e4__double_ndiv(n, d, 0, &q_.low, &r_)))
            return res;
    } else {
        if ((res = e4__double_ndiv(e4__double_u(n.high, 0), d, 0,
                &q_.high, &r_)))
            return res;
        if ((res = e4__double_ndiv(e4__double_u(n.low, r_), d, 0,
                &q_.low, &r_)))
            return res;
    }

    /* Signed overflow cannot happen because we are not narrowing. */

    q_ = negate ? e4__double_negate(q_) : q_;
    r_ = negate ? e4__USIZE_NEGATE(r_) : r_;

    if ((flags & e4__F_SIGNED) && (flags & e4__F_FLOORDIV) &&
            e4__USIZE_IS_NEGATIVE(r_)) {
        if (!q_.low--)
            q_.high -= 1;
        r_ += d;
    }

    if (q)
        *q = q_;
    if (r)
        *r = r_;

    return e4__E_OK;
}

e4__usize e4__double_ndiv(struct e4__double n, e4__usize d,
        e4__usize flags, e4__usize *q, e4__usize *r)
{
    const e4__usize base = (e4__usize)1 << (e4__USIZE_BIT >> 1);
    register e4__usize d1, d0, q1, q0, n32, n21, n10, n1, n0, rem, s, q_, r_;
    e4__bool negate = 0;

    /* Based on Hank Warren's long unsigned division reference
       implementation in "Hacker's Delight" 2nd Edition (Figure 9-3). */

    if (!d)
        return e4__E_DIVBYZERO;

    if (flags & e4__F_SIGNED) {
        if (e4__USIZE_IS_NEGATIVE(n.high)) {
            n = e4__double_negate(n);
            negate = !negate;
        }

        if (e4__USIZE_IS_NEGATIVE(d)) {
            d = e4__USIZE_NEGATE(d);
            negate = !negate;
        }
    }

    if (n.high >= d)
        return e4__E_RSLTOUTORANGE;

    s = e4__usize_clz(d);
    d <<= s;
    e4__USIZE_SPLIT(d, d1, d0);

    n32 = n.high << s;
    if (s)
        n32 |= n.low >> (e4__USIZE_BIT - s);
    n10 = n.low << s;
    e4__USIZE_SPLIT(n10, n1, n0);

    q1 = n32 / d1;
    rem = n32 % d1;

    while (q1 >= base || q1 * d0 > e4__USIZE_U(rem, n1)) {
        q1 -= 1;
        rem += d1;
        if (rem >= base)
            break;
    }

    n21 = e4__USIZE_U(n32, n1) - q1 * d;

    q0 = n21 / d1;
    rem = n21 % d1;

    while (q0 >= base || q0 * d0 > e4__USIZE_U(rem, n0)) {
        q0 -= 1;
        rem += d1;
        if (rem >= base)
            break;
    }

    q_ = e4__USIZE_U(q1, q0);

    if (q && (flags & e4__F_SIGNED) && e4__USIZE_IS_NEGATIVE(q_))
        /* Only check for signed overflow when returning quotient. Since
           sign overflow on remainder is not possible, if only remainder
           is being returned, we don't need to care.

           If the sign bit is already set in this case, we are in an
           overflow. */
        return e4__E_RSLTOUTORANGE;

    q_ = negate ? e4__USIZE_NEGATE(q_) : q_;

    r_ = (e4__USIZE_U(n21, n0) - q0 * d) >> s;
    if (negate)
        r_ = e4__USIZE_NEGATE(r_);

    if ((flags & e4__F_SIGNED) && (flags & e4__F_FLOORDIV) &&
            e4__USIZE_IS_NEGATIVE(r_)) {
        q_ -= 1;
        r_ += d >> s;
    }

    if (q)
        *q = q_;
    if (r)
        *r = r_;

    return e4__E_OK;
}

struct e4__double e4__double_negate(struct e4__double d)
{
    d.high ^= (e4__usize)-1;
    d.low ^= (e4__usize)-1;
    d.high += !++d.low;
    return d;
}
