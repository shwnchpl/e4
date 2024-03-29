#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

#include <string.h>

static void e4t__test_util_double(void)
{
    /* Test e4__usize_todouble. */
    e4t__ASSERT_DEQ(e4__usize_todouble(1), e4__double_u(1, 0));
    e4t__ASSERT_DEQ(e4__usize_todouble(-1), e4__double_u(-1, -1));
    e4t__ASSERT_DEQ(e4__usize_todouble(-2), e4__double_u(-2, -1));

    /* Test e4__double_negate. */
    e4t__ASSERT_DEQ(e4__double_negate(e4__double_u(0, 0)),
            e4__double_u(0, 0));
    e4t__ASSERT_DEQ(e4__double_negate(e4__double_u(1, 0)),
            e4__double_u(-1, -1));
    e4t__ASSERT_DEQ(e4__double_negate(e4__double_u(5, 0)),
            e4__double_u(-5, -1));
    e4t__ASSERT_DEQ(e4__double_negate(e4__double_u(79, 351)),
            e4__double_u(-79, -352));
}

static void e4t__test_util_double_div(void)
{
    e4__usize r;
    struct e4__double dq;

    /* XXX: Some of these tests may only work on a system that
       represents negative numbers using two's complement. */

    /* Test unsigned double cell by cell to double cell division. */
    e4t__ASSERT_EQ(e4__double_div(e4__double_u(0, 0), 0, 0, &dq, &r),
            e4__E_DIVBYZERO);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(0, 0), 5, 0, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(0, 0));
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(5, 0), 1, 0, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(5, 0));
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(573, 0), 37, 0, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(15, 0));
    e4t__ASSERT_EQ(r, 18);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(0, 1), 2, 0, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u((e4__usize)1 << 63, 0));
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(0, 1), 1, 0, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(0, 1));
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(2, 4), 2, 0, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(1, 2));
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(0x206ade00d7babd8e,
            0xe7a7a29ef454b647), 0x1081a4b61c4a8, 0, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(0x30bd431e56cac42d, 0xe08c));
    e4t__ASSERT_EQ(r, 0xf8c5206a8c06);

    /* Test signed double cell by cell to double cell division. */
    e4t__ASSERT_OK(e4__double_div(e4__double_u(0, 0), -1,
            e4__F_SIGNED, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(0, 0));
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(-5, -1), 1,
            e4__F_SIGNED, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(-5, -1));
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(5, 0), -1,
            e4__F_SIGNED, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(-5, -1));
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(-5, -1), -1,
            e4__F_SIGNED, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(5, 0));
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(-5, -1), 2,
            e4__F_SIGNED, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(-2, -1));
    e4t__ASSERT_EQ(r, -1);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(5, 0), -2,
            e4__F_SIGNED, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(-2, -1));
    e4t__ASSERT_EQ(r, 1);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(-5, -1), -2,
            e4__F_SIGNED, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(2, 0));
    e4t__ASSERT_EQ(r, -1);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(0, 1), 2,
            e4__F_SIGNED, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u((e4__usize)1 << 63, 0));
    e4t__ASSERT_EQ(r, 0);

    /* Test floored double cell by cell to double cell division. */
    e4t__ASSERT_OK(e4__double_div(e4__double_u(-5, -1), 2,
            e4__F_SIGNED | e4__F_FLOORDIV, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(-3, -1));
    e4t__ASSERT_EQ(r, 1);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(5, 0), -2,
            e4__F_SIGNED | e4__F_FLOORDIV, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(-3, -1));
    e4t__ASSERT_EQ(r, -1);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(-5, -1), -2,
            e4__F_SIGNED | e4__F_FLOORDIV, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(2, 0));
    e4t__ASSERT_EQ(r, -1);

    e4t__ASSERT_OK(e4__double_div(e4__double_u(4, 0), -2,
            e4__F_SIGNED | e4__F_FLOORDIV, &dq, &r));
    e4t__ASSERT_DEQ(dq, e4__double_u(-2, -1));
    e4t__ASSERT_EQ(r, 0);
}

static void e4t__test_util_double_mul(void)
{
    e4t__ASSERT_DEQ(e4__usize_mul(0, 0, 0), e4__double_u(0, 0));
    e4t__ASSERT_DEQ(e4__usize_mul(1, 0, 0), e4__double_u(0, 0));
    e4t__ASSERT_DEQ(e4__usize_mul(0, 1, 0), e4__double_u(0, 0));
    e4t__ASSERT_DEQ(e4__usize_mul(0, 0, e4__F_SIGNED), e4__double_u(0, 0));
    e4t__ASSERT_DEQ(e4__usize_mul(1, 0, e4__F_SIGNED), e4__double_u(0, 0));
    e4t__ASSERT_DEQ(e4__usize_mul(0, 1, e4__F_SIGNED), e4__double_u(0, 0));

    e4t__ASSERT_DEQ(e4__usize_mul(1, 1, 0), e4__double_u(1, 0));
    e4t__ASSERT_DEQ(e4__usize_mul(1, 1, e4__F_SIGNED), e4__double_u(1, 0));
    e4t__ASSERT_DEQ(e4__usize_mul(-1, 1, e4__F_SIGNED),
            e4__double_u(-1, -1));
    e4t__ASSERT_DEQ(e4__usize_mul(-1, -1, e4__F_SIGNED), e4__double_u(1, 0));

    e4t__ASSERT_DEQ(e4__usize_mul(1, 5, 0), e4__double_u(5, 0));
    e4t__ASSERT_DEQ(e4__usize_mul(1, 5, e4__F_SIGNED), e4__double_u(5, 0));
    e4t__ASSERT_DEQ(e4__usize_mul(-1, 5, e4__F_SIGNED),
            e4__double_u(-5, -1));
    e4t__ASSERT_DEQ(e4__usize_mul(1, -5, e4__F_SIGNED),
            e4__double_u(-5, -1));
    e4t__ASSERT_DEQ(e4__usize_mul(-1, -5, e4__F_SIGNED), e4__double_u(5, 0));

    e4t__ASSERT_DEQ(e4__usize_mul(2, 5, 0), e4__double_u(10, 0));
    e4t__ASSERT_DEQ(e4__usize_mul(2, 5, e4__F_SIGNED), e4__double_u(10, 0));
    e4t__ASSERT_DEQ(e4__usize_mul(-2, 5, e4__F_SIGNED),
            e4__double_u(-10, -1));
    e4t__ASSERT_DEQ(e4__usize_mul(2, -5, e4__F_SIGNED),
            e4__double_u(-10, -1));
    e4t__ASSERT_DEQ(e4__usize_mul(-2, -5, e4__F_SIGNED),
            e4__double_u(10, 0));

    e4t__ASSERT_DEQ(e4__usize_mul(-1, 2, 0), e4__double_u(-2, 1));
    e4t__ASSERT_DEQ(e4__usize_mul(-5, -10, 0), e4__double_u(50, -15));
    e4t__ASSERT_DEQ(e4__usize_mul(-1, -1, 0), e4__double_u(1, -2));
}

static void e4t__test_util_double_ndiv(void)
{
    e4__usize q, r;

    /* XXX: Some of these tests may only work on a system that
       represents negative numbers using two's complement. */

    /* Test unsigned double cell by cell to cell narrowing division. */
    e4t__ASSERT_EQ(e4__double_ndiv(e4__double_u(0, 0), 0, 0, &q, &r),
            e4__E_DIVBYZERO);
    e4t__ASSERT_EQ(e4__double_ndiv(e4__double_u(0, 1), 1, 0, &q, &r),
            e4__E_RSLTOUTORANGE);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(0, 0), 5, 0, &q, &r));
    e4t__ASSERT_EQ(q, 0);
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(5, 0), 1, 0, &q, &r));
    e4t__ASSERT_EQ(q, 5);
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(573, 0), 37, 0, &q, &r));
    e4t__ASSERT_EQ(q, 15);
    e4t__ASSERT_EQ(r, 18);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(0, 1), 2, 0, &q, &r));
    e4t__ASSERT_EQ(q, (e4__usize)1 << 63);
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(5, 1), 8, 0, &q, &r));
    e4t__ASSERT_EQ(q, (e4__usize)1 << 61);
    e4t__ASSERT_EQ(r, 5);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(0x95440111682f25b6,
            0x157186c4791), 0xb29430a256d21, 0, &q, &r));
    e4t__ASSERT_EQ(q, 0x1ebd7510f03cdb);
    e4t__ASSERT_EQ(r, 0x5e75995a70e7b);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(-1, -2), -1, 0, &q, &r));
    e4t__ASSERT_EQ(q, -1);
    e4t__ASSERT_EQ(r, -2);

    /* Test signed double cell by cell to cell narrowing division. */
    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(0, 0), -1,
            e4__F_SIGNED, &q, &r));
    e4t__ASSERT_EQ(q, 0);
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(-5, -1), 1,
            e4__F_SIGNED, &q, &r));
    e4t__ASSERT_EQ(q, -5);
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(5, 0), -1,
            e4__F_SIGNED, &q, &r));
    e4t__ASSERT_EQ(q, -5);
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(-5, -1), -1,
            e4__F_SIGNED, &q, &r));
    e4t__ASSERT_EQ(q, 5);
    e4t__ASSERT_EQ(r, 0);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(-5, -1), 2,
            e4__F_SIGNED, &q, &r));
    e4t__ASSERT_EQ(q, -2);
    e4t__ASSERT_EQ(r, -1);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(5, 0), -2,
            e4__F_SIGNED, &q, &r));
    e4t__ASSERT_EQ(q, -2);
    e4t__ASSERT_EQ(r, 1);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(-5, -1), -2,
            e4__F_SIGNED, &q, &r));
    e4t__ASSERT_EQ(q, 2);
    e4t__ASSERT_EQ(r, -1);

    e4t__ASSERT_EQ(e4__double_ndiv(e4__double_u(0, 1), 2,
            e4__F_SIGNED, &q, &r), e4__E_RSLTOUTORANGE);
    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(0, 1), 2,
            e4__F_SIGNED, NULL, &r));
    e4t__ASSERT_EQ(r, 0);

    /* Test floored double cell by cell to cell narrowing division. */
    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(-5, -1), 2,
            e4__F_SIGNED | e4__F_FLOORDIV, &q, &r));
    e4t__ASSERT_EQ(q, -3);
    e4t__ASSERT_EQ(r, 1);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(5, 0), -2,
            e4__F_SIGNED | e4__F_FLOORDIV, &q, &r));
    e4t__ASSERT_EQ(q, -3);
    e4t__ASSERT_EQ(r, -1);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(-5, -1), -2,
            e4__F_SIGNED | e4__F_FLOORDIV, &q, &r));
    e4t__ASSERT_EQ(q, 2);
    e4t__ASSERT_EQ(r, -1);

    e4t__ASSERT_OK(e4__double_ndiv(e4__double_u(4, 0), -2,
            e4__F_SIGNED | e4__F_FLOORDIV, &q, &r));
    e4t__ASSERT_EQ(q, -2);
    e4t__ASSERT_EQ(r, 0);
}

static void e4t__test_util_cbuf(void)
{
    struct e4__cbuf cbuf;
    e4__usize len;
    char buf[20];
    char *r0, *r1, *r2;

    /* Test pushing unescaped strings. */
    e4__mem_cbuf_init(&cbuf, buf, sizeof(buf));

    e4t__ASSERT_EQ(e4__mem_cbuf_push(&cbuf, NULL, 40), NULL);
    e4t__ASSERT_EQ(e4__mem_cbuf_push(&cbuf, NULL, 0), NULL);

    e4t__ASSERT_MATCH((r0 = e4__mem_cbuf_push(&cbuf, "foo", 4)), "foo");
    e4t__ASSERT_MATCH((r1 = e4__mem_cbuf_push(&cbuf, "bar", 4)), "bar");
    e4t__ASSERT_MATCH((r2 = e4__mem_cbuf_push(&cbuf, "bas", 4)), "bas");

    e4t__ASSERT_MATCH(r0, "foo");
    e4t__ASSERT_MATCH(r1, "bar");
    e4t__ASSERT_MATCH(r2, "bas");

    e4t__ASSERT(e4__mem_cbuf_push(&cbuf, "justsomejunk", 8));
    e4t__ASSERT_MATCH(r0, "foo");

    e4t__ASSERT(e4__mem_cbuf_push(&cbuf, "newjunk", 8));
    e4t__ASSERT_MATCH(r0, "newjunk");
    e4t__ASSERT_MATCH(r1, "unk");

    /* Test pushing escaped strings. */
    e4__mem_cbuf_init(&cbuf, buf, sizeof(buf));

    len = 0;
    e4t__ASSERT_EQ(e4__mem_cbuf_epush(&cbuf, NULL, &len), NULL);

    len = 8;
    e4t__ASSERT_MATCH((r0 = e4__mem_cbuf_epush(&cbuf, "foo\\r\\n", &len)),
            "foo\r\n");
    e4t__ASSERT_EQ(len, 6);
    len = 8;
    e4t__ASSERT_MATCH((r1 = e4__mem_cbuf_epush(&cbuf, "bar\\r\\n", &len)),
            "bar\r\n");
    e4t__ASSERT_EQ(len, 6);
    len = 8;
    e4t__ASSERT_MATCH((r2 = e4__mem_cbuf_epush(&cbuf, "bas\\r\\n", &len)),
            "bas\r\n");
    e4t__ASSERT_EQ(len, 6);

    e4t__ASSERT_MATCH(r0, "foo\r\n");
    e4t__ASSERT_MATCH(r1, "bar\r\n");
    e4t__ASSERT_MATCH(r2, "bas\r\n");

    len = 7;
    e4t__ASSERT_MATCH(e4__mem_cbuf_epush(&cbuf, "qu\\r\\n", &len), "qu\r\n");
    e4t__ASSERT_MATCH(r0, "qu\r\n");
    e4t__ASSERT_EQ(len, 5);

    len = 42;
    e4t__ASSERT_EQ(e4__mem_cbuf_epush(&cbuf,
            "quux\\zbarbasfooblah\\nandthenevenmorejunk", &len), NULL);
    e4t__ASSERT_MATCH(r0, "quux");
    e4t__ASSERT_EQ(len, 42);

    e4t__ASSERT_EQ(e4__mem_cbuf_epush(&cbuf,
            "quuxfzbarbasfooblahmnandthenevenmorejunk", &len), NULL);
    e4t__ASSERT_EQ(len, 42);

    e4t__ASSERT(e4__mem_cbuf_push(&cbuf, "f", 1));

    len = 42;
    e4t__ASSERT_EQ(e4__mem_cbuf_epush(&cbuf,
            "quux\\zbarbasfooblah\\nandthenevenmorejunk", &len), NULL);
    e4t__ASSERT_MATCH(r0, "fquux");
    e4t__ASSERT_EQ(len, 42);

    e4t__ASSERT_EQ(e4__mem_cbuf_epush(&cbuf,
            "quuxfzbarbasfooblahmnandthenevenmorejunk", &len), NULL);
    e4t__ASSERT_EQ(len, 42);
}

static void e4t__test_util_exceptionformat(void)
{
    e4__usize len = 0;

    /* Test formatting a few exceptions. */
    e4t__ASSERT_MATCH(e4__usize_format_exception(e4__E_OK, NULL), "ok");
    e4t__ASSERT_MATCH(e4__usize_format_exception(e4__E_STKUNDERFLOW, NULL),
            "stack underflow");
    e4t__ASSERT_MATCH(e4__usize_format_exception(1345, NULL), "unknown");

    /* Test that, when requested, length is set as expected. */
    e4t__ASSERT_MATCH(e4__usize_format_exception(e4__E_RSTKUNDERFLOW, &len),
            "return stack underflow");
    e4t__ASSERT_EQ(len, 22);
}

static void e4t__test_util_math(void)
{
    e4__usize i, r;

    /* XXX: Parts of this test only work correctly on a 64 bit system
       that represents negative numbers using two's complement. */

    e4t__ASSERT_EQ(e4__USIZE_NEGATE((e4__usize)5), -5);
    e4t__ASSERT_EQ(e4__USIZE_IS_NEGATIVE((e4__usize)-5), 1);

    e4t__ASSERT_EQ(e4__usize_sdiv((e4__usize)-10, (e4__usize)3, NULL), -3);
    e4t__ASSERT_EQ(e4__usize_sdiv((e4__usize)10, (e4__usize)-3, NULL), -3);
    e4t__ASSERT_EQ(e4__usize_sdiv((e4__usize)-10, (e4__usize)-3, NULL), 3);
    e4t__ASSERT_EQ(e4__usize_sdiv((e4__usize)10, (e4__usize)3, NULL), 3);

    e4t__ASSERT_EQ(e4__usize_sdiv((e4__usize)10, (e4__usize)7, &r), 1);
    e4t__ASSERT_EQ(r, 3);
    e4t__ASSERT_EQ(e4__usize_sdiv((e4__usize)-10, (e4__usize)7, &r), -1);
    e4t__ASSERT_EQ(r, -3);
    e4t__ASSERT_EQ(e4__usize_sdiv((e4__usize)10, (e4__usize)-7, &r), -1);
    e4t__ASSERT_EQ(r, 3);
    e4t__ASSERT_EQ(e4__usize_sdiv((e4__usize)-10, (e4__usize)-7, &r), 1);
    e4t__ASSERT_EQ(r, -3);

    e4t__ASSERT_EQ(e4__mem_aligned(0), 0);
    e4t__ASSERT_EQ(e4__mem_aligned(1), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(2), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(3), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(4), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(5), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(6), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(7), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(8), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(9), 16);

    e4t__ASSERT_EQ(e4__usize_clz(0), e4__USIZE_BIT);
    for (i = 0; i < e4__USIZE_BIT; ++i)
        e4t__ASSERT_EQ(e4__usize_clz((e4__usize)1 << i),
                e4__USIZE_BIT - 1 - i);
}

static void e4t__test_util_mem_dict(void)
{
    char buf[4096] = {0,};
    char *here = buf;
    e4__usize wrote;
    struct e4__dict_header *dict = NULL;
    const struct e4__dict_header *last, *h0, *h1, *h2;

    #define _d(s, c)    \
        do {    \
            wrote = e4__mem_dict_entry(here, dict, s, sizeof(s) - 1, 0, c,  \
                    NULL);  \
            dict = (struct e4__dict_header *)here;  \
            here += wrote;  \
        } while (0)
    #define _l(s)   e4__mem_dict_lookup(dict, s, sizeof(s) - 1)
    #define _s(d, p)    e4__mem_dict_suggest(d, p, sizeof(p) - 1)
    #define _lxt(x) e4__mem_dict_lookup_xt(dict, x)

    /* Test basic lookup. */
    _d("first-entry", (e4__code_ptr)0xabcde);
    e4t__ASSERT_EQ(_l("first-entry"), dict);
    e4t__ASSERT_EQ(_l("first-entry")->xt->code, 0xabcde);
    e4t__ASSERT_EQ((e4__usize)here % sizeof(e4__cell), 0);

    _d("second-entry", (e4__code_ptr)0x12345);
    e4t__ASSERT_EQ(_l("second-entry"), dict);
    e4t__ASSERT_EQ(_l("second-entry")->xt->code, 0x12345);

    e4t__ASSERT(_l("first-entry"));
    e4t__ASSERT_EQ(_l("first-entry")->xt->code, 0xabcde);
    e4t__ASSERT_EQ(_l("FIRST-entry")->xt->code, 0xabcde);

    e4t__ASSERT_EQ((e4__usize)_l("not-present"), (e4__usize)NULL);

    /* Test prefix suggestion. */
    _d("prefix-first", (e4__code_ptr)0x11111);
    _d("prefix-second", (e4__code_ptr)0x22222);
    _d("prefix-third", (e4__code_ptr)0x33333);

    e4t__ASSERT((last = _s(dict, "prefix")));
    e4t__ASSERT_EQ(last->xt->code, 0x33333);
    e4t__ASSERT((last = _s(last->link, "prefix")));
    e4t__ASSERT_EQ(last->xt->code, 0x22222);
    e4t__ASSERT((last = _s(last->link, "prefix")));
    e4t__ASSERT_EQ(last->xt->code, 0x11111);
    e4t__ASSERT(!(last = _s(last->link, "prefix")));

    /* Lookup based on execute token works correctly. */
    _d("xt-entry-1", (e4__code_ptr)0xabcde);
    e4t__ASSERT((h0 = _l("xt-entry-1")));
    e4t__ASSERT((h1 = _lxt((e4__cell)h0->xt)));
    e4t__ASSERT_EQ(h0, h1);

    _d("xt-entry-2", (e4__code_ptr)0x12345);
    e4t__ASSERT((h1 = _l("xt-entry-2")));

    /* XXX: In user code, no one should ever use a dictionary header
       returned from one of the lookup APIs in this way, but for the
       purposes of this test it is known to be safe. */
    ((struct e4__dict_header *)h1)->xt = h0->xt;

    e4t__ASSERT((h2 = _lxt((e4__cell)h1->xt)));
    e4t__ASSERT_EQ(h1, h2);
    e4t__ASSERT(h0 != h2);
    e4t__ASSERT_EQ(h0->xt, h2->xt);

    e4t__ASSERT(!_lxt(NULL));

    #undef _lxt
    #undef _s
    #undef _l
    #undef _d
}

static void e4t__test_util_mem_dump(void)
{
    /* XXX: Parts of this test only work correctly on a 64 bit system
       where unsigned long values have 8 bit alignment. */

    static const char *test_str =
            "test \x88\x99\xaa string with some \x01 \x02 \x03 data";

    const char *cursor;
    e4__usize wrote_sz;
    unsigned long buffer[8];
    char line[80] = {0,};
    char expected_line[3][80] = {0,};
    char *unaligned_buf = ((char *)buffer) + 3;
    e4__usize remaining = strlen(test_str);

    #define _f  "%016lx   "
    sprintf(expected_line[0],
            _f "???? ??74 6573 7420  8899 aa20 7374 7269   ...test ... stri\n",
            (unsigned long)&buffer[0]);
    sprintf(expected_line[1],
            _f "6e67 2077 6974 6820  736f 6d65 2001 2002   ng with some . .\n",
            (unsigned long)&buffer[2]);
    sprintf(expected_line[2],
            _f "2003 2064 6174 61??  ???? ???? ???? ????    . data.........\n",
            (unsigned long)&buffer[4]);
    #undef _f

    /* XXX: It would violate strict aliasing rules if we ever looked at
       buffer again after modifying it using unaligned_buf. So long as
       we don't do that, everything should be fine. */
    memcpy(unaligned_buf, test_str, remaining);
    cursor = unaligned_buf;

    e4t__ASSERT_EQ(remaining, 36);

    wrote_sz = e4__mem_dump(&cursor, &remaining, line);
    e4t__ASSERT_EQ(wrote_sz, 79);
    e4t__ASSERT_EQ(remaining, 23);
    e4t__ASSERT_MATCH(line, expected_line[0]);

    wrote_sz = e4__mem_dump(&cursor, &remaining, line);
    e4t__ASSERT_EQ(wrote_sz, 79);
    e4t__ASSERT_EQ(remaining, 7);
    e4t__ASSERT_MATCH(line, expected_line[1]);

    wrote_sz = e4__mem_dump(&cursor, &remaining, line);
    e4t__ASSERT_EQ(wrote_sz, 79);
    e4t__ASSERT_EQ(remaining, 0);
    e4t__ASSERT_MATCH(line, expected_line[2]);
}

static void e4t__test_util_mem_pno(void)
{
    char buffer[131] = {0};
    char *b = &buffer[129];
    struct e4__double d;

    /* XXX: Parts of this test only work correctly on a 64 bit
       system. */

    /* Test simple hold and holds utilities in isolation. */
    e4__mem_pno_hold(&b, 'o');
    e4t__ASSERT_EQ(b, &buffer[128]);
    e4t__ASSERT_MATCH(&b[1], "o");

    e4__mem_pno_hold(&b, 'o');
    e4t__ASSERT_EQ(b, &buffer[127]);
    e4t__ASSERT_MATCH(&b[1], "oo");

    e4__mem_pno_hold(&b, 'f');
    e4t__ASSERT_EQ(b, &buffer[126]);
    e4t__ASSERT_MATCH(&b[1], "foo");

    e4__mem_pno_holds(&b, "bas bar ", 8);
    e4t__ASSERT_EQ(b, &buffer[118]);
    e4t__ASSERT_MATCH(&b[1], "bas bar foo");

    /* Test formatting of single digits. */
    b = &buffer[129];
    d = e4__double_u(532, 0);

    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 10, &d));
    e4t__ASSERT_MATCH(&b[1], "2");
    e4t__ASSERT_DEQ(d, e4__double_u(53, 0));

    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 10, &d));
    e4t__ASSERT_MATCH(&b[1], "32");
    e4t__ASSERT_DEQ(d, e4__double_u(5, 0));

    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 10, &d));
    e4t__ASSERT_MATCH(&b[1], "532");
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));

    b = &buffer[129];
    d = e4__double_u(0xbeef, 0);

    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 16, &d));
    e4t__ASSERT_MATCH(&b[1], "f");
    e4t__ASSERT_DEQ(d, e4__double_u(0xbee, 0));

    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 16, &d));
    e4t__ASSERT_MATCH(&b[1], "ef");
    e4t__ASSERT_DEQ(d, e4__double_u(0xbe, 0));

    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 16, &d));
    e4t__ASSERT_MATCH(&b[1], "eef");
    e4t__ASSERT_DEQ(d, e4__double_u(0xb, 0));

    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 16, &d));
    e4t__ASSERT_MATCH(&b[1], "beef");
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));

    /* Test formatting of signle digits with base out of range
       clamps to the correct base. */
    b = &buffer[129];
    d = e4__double_u(6, 0);

    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 1, &d));
    e4t__ASSERT_DEQ(d, e4__double_u(3, 0));
    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 1, &d));
    e4t__ASSERT_DEQ(d, e4__double_u(1, 0));
    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 0, &d));
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));

    e4t__ASSERT_MATCH(&b[1], "110");

    b = &buffer[129];
    d = e4__double_u(23349, 0);

    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 90, &d));
    e4t__ASSERT_DEQ(d, e4__double_u(648, 0));
    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 80, &d));
    e4t__ASSERT_DEQ(d, e4__double_u(18, 0));
    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 70, &d));
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));

    e4t__ASSERT_MATCH(&b[1], "I0L");

    /* Test formatting an entire number in one go. */
    b = &buffer[129];
    d = e4__double_u(532, 0);

    e4t__ASSERT_OK(e4__mem_pno_digits(&b, b - buffer, 10, 0, &d));
    e4t__ASSERT_MATCH(&b[1], "532");
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));

    b = &buffer[129];
    d = e4__double_u(0xbeef, 0);

    e4t__ASSERT_OK(e4__mem_pno_digits(&b, b - buffer, 16, 0, &d));
    e4t__ASSERT_MATCH(&b[1], "beef");
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));

    /* Test formatting an entire number when that number is 0. */
    b = &buffer[129];
    d = e4__double_u(0, 0);

    e4t__ASSERT_OK(e4__mem_pno_digits(&b, b - buffer, 10, 0, &d));
    e4t__ASSERT_MATCH(&b[1], "0");
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));

    /* Test that attempting to fomrat a number with not enough space
       writes the digits that can fit then returns
       e4__E_PNOOVERFLOW. */
    b = &buffer[129];
    d = e4__double_u(0xbeef, 0);

    e4t__ASSERT_EQ(e4__mem_pno_digits(&b, 2, 16, 0, &d), e4__E_PNOOVERFLOW);
    e4t__ASSERT_MATCH(&b[1], "ef");
    e4t__ASSERT_DEQ(d, e4__double_u(0xbe, 0));

    /* Test printing a number that actually makes use of two cells. */
    b = &buffer[129];
    d = e4__double_u(-1, 0xc0b);

    e4t__ASSERT_OK(e4__mem_pno_digits(&b, b - buffer, 16, 0, &d));
    e4t__ASSERT_MATCH(&b[1], "c0bffffffffffffffff");
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));

    /* Test formatting a signed number. */
    b = &buffer[129];
    d = e4__double_u(593, 0);

    e4t__ASSERT_OK(e4__mem_pno_digits(&b, b - buffer, 10, e4__F_SIGNED, &d));
    e4t__ASSERT_MATCH(&b[1], "593");
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));

    /* Test formatting a signed negative number */
    b = &buffer[129];
    d = e4__double_u(-593, -1);

    e4t__ASSERT_OK(e4__mem_pno_digits(&b, b - buffer, 10, e4__F_SIGNED, &d));
    e4t__ASSERT_MATCH(&b[1], "-593");
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));

    /* Test overflow of only the '-' sign. */
    b = &buffer[129];
    d = e4__double_u(-35, -1);

    e4t__ASSERT_EQ(e4__mem_pno_digits(&b, 2, 10, e4__F_SIGNED, &d),
            e4__E_PNOOVERFLOW);
    e4t__ASSERT_MATCH(&b[1], "35");
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));

    /* Test mixing various types of formatting together. */
    b = &buffer[129];
    d = e4__double_u(1995, 0);

    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 10, &d));
    e4t__ASSERT_OK(e4__mem_pno_digit(&b, 10, &d));
    e4__mem_pno_hold(&b, '.');
    e4t__ASSERT_OK(e4__mem_pno_digits(&b, b - buffer, 10, 0, &d));
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));
    e4__mem_pno_holds(&b, "Price: $", 8);
    e4t__ASSERT_MATCH(&b[1], "Price: $19.95");
}

static void e4t__test_util_mem_see(void)
{
    char buf[4096] = {0,};
    char line_buf[81] = {0,};
    char fmt_buf[81] = {0,};
    char *here = buf, *line;
    int fmt_buf_len;
    e4__usize wrote, line_len;
    struct e4__dict_header *dict = NULL, header_mut;
    struct e4__execute_token dt_xt, *dtdt_xt, *threaded_xt;
    void *does_thunk_addr, *does_thunk_data_addr;
    void *marker_here_addr;
    void *variable_data_addr;
    const void *state;

    /* FIXME: This test would benefit from a case sensitive
       e4t__ASSERT_MATCH variant, should one ever be added. */

    /* XXX: Parts of this test assume a 64-bit system and may not pass
       on other architectures. If this test does fail in such a case, it
       likely is only the result of assumptions that are made about the
       length of the hexadecimal representation of addresses and does
       not indicate a serious issue. */

    /* XXX: This test does not exercise every possible overflow case
       because some of them simply are not possible on a 64-bit
       architecture (namely, lines containing only numbers can never
       overflow). */

    /* XXX: Several parts of this test can be expected to fail if the
       following condition is not true. */
    e4t__ASSERT_EQ(sizeof(unsigned long), sizeof(void *));

    /********************************************
     * Define utility macros to simplify testing.
     ********************************************/

    #define _df(s, c, u, f)  \
        do {    \
            wrote = e4__mem_dict_entry(here, dict, s, sizeof(s) - 1, f, c,  \
                    u);  \
            dict = (struct e4__dict_header *)here;  \
            here += wrote;  \
        } while (0)
    #define _d(s, c, u) _df(s, c, u, 0)
    #define _l(s)   e4__mem_dict_lookup(dict, s, sizeof(s) - 1)
    /* XXX: This works because here will always be aligned based on how
       we are adding to it. */
    #define _c(b)   \
        do {    \
            *((e4__cell *)here) = (e4__cell)b;  \
            here += sizeof(e4__cell);   \
        } while (0)
    #define _s_setup(s) \
        do {    \
            header_mut = *_l(s);    \
            state = &header_mut;    \
        } while (0)
    #define _s_line()   \
        e4__mem_see(&state, &header_mut, dict, line_buf, &line)
    #define _expect_line(s) \
        do {    \
            line_len = _s_line();   \
            e4t__ASSERT_EQ(line_len, sizeof(s) - 1);    \
            e4t__ASSERT_MATCH(line, s); \
        } while (0)
    #define _expect_formatted_line(s, p)    \
        do {    \
            fmt_buf_len = sprintf(fmt_buf, s, (unsigned long)p);    \
            line_len = _s_line();   \
            e4t__ASSERT_EQ(line_len, fmt_buf_len);  \
            e4t__ASSERT_MATCH(line, fmt_buf);   \
        } while (0)
    #define _long_name  \
        "very-long-entry-that-will-definitely-" \
        "cause-an-overflow-because-it-is-itself-over-80-characters"

    /************************************************
     * Create each possible type of dictionary entry.
     ************************************************/

    /* Start by creating some basic "native code" definitions to which
       other threaded code definitions can refer. */
    _df("native-0x123", (e4__code_ptr)0x123, (void *)0x456,
            e4__F_BUILTIN | e4__F_CONSTANT | e4__F_COMPONLY | e4__F_IMMEDIATE);
    _d("native-0xabc", (e4__code_ptr)0xabc, NULL);
    _d("native-0xfed", (e4__code_ptr)0xfed, NULL);

    /* Add the sentinel as a definition, for convenience. This is a
       hack. */
    _d("SENTINEL", NULL, NULL);
    ((struct e4__dict_header *)_l("SENTINEL"))->xt =
            (struct e4__execute_token *)&e4__BUILTIN_XT[e4__B_SENTINEL];

    /* Create a threaded definition. */
    _df("threaded-0x123", e4__execute_threaded, NULL, e4__F_IMMEDIATE);
    _c(_l("native-0x123")->xt);
    _c(_l("native-0xfed")->xt);
    _c(0x8999);
    _c(_l("native-0xabc")->xt);
    _c(&e4__BUILTIN_XT[e4__B_SENTINEL]);
    threaded_xt = _l("threaded-0x123")->xt;

    /* Create a threaded thunk definition. */
    _d("threaded-thunk-0x123", e4__execute_threadedthunk, threaded_xt->data);

    /* Create a defer thunk that refers to threaded code in the
       dictionary. */
    _d("defer-thunk-0x123", e4__execute_deferthunk, threaded_xt);

    /* Create a defer thunk that refers to anonymous native code. */
    dt_xt.code = (e4__code_ptr)0x789;
    dt_xt.user = (void *)0x777;
    _d("defer-thunk-0x765", e4__execute_deferthunk, &dt_xt);

    /* Create a defer thunk that refers to a defer thunk. */
    dtdt_xt = _l("defer-thunk-0x765")->xt;
    _df("defer-thunk-0xf00", e4__execute_deferthunk, dtdt_xt, e4__F_IMMEDIATE);

    /* Create a does> thunk. */
    does_thunk_addr = here;
    _c(_l("native-0xfed")->xt);
    _c(_l("native-0xfed")->xt);
    _c(&e4__BUILTIN_XT[e4__B_SENTINEL]);
    _d("does-thunk-0x123", e4__execute_doesthunk, does_thunk_addr);
    does_thunk_data_addr = here;

    /* Create a value. */
    _d("value-0x123", e4__execute_value, NULL);
    _c(0x9001);

    /* Create a variable. */
    _d("variable-0x123", e4__execute_variable, NULL);
    variable_data_addr = here;
    _c(0x9002);

    /* Create a user value. */
    _d("userval-0x123", e4__execute_userval, (void *)0x123);

    /* Create a user variable. */
    _d("uservar-0x555", e4__execute_uservar, (void *)0x555);

    /* Create a marker. */
    marker_here_addr = here;
    _d("marker-HERE", e4__execute_marker, (void *)here);

    /* Create a dictionary entry that will overflow. */
    _d(_long_name, (e4__code_ptr)0xdac, NULL);

    /* Create a dictionary entry that refers to the entry that will
       overflow. */
    _df("threaded-0xdac", e4__execute_threaded, NULL,
            e4__F_BUILTIN | e4__F_COMPONLY);
    _c(_l("native-0x123")->xt);
    _c(_l(_long_name)->xt);
    _c(0x9003);
    _c(&e4__BUILTIN_XT[e4__B_SENTINEL]);

    /***********************************************************
     * Look-up each entry and ensure that output is as expected.
     ***********************************************************/

    /* Check the initial native code entry. */
    _s_setup("native-0x123");
    _expect_line(": native-0x123 ( native - user: 0x456 )\n");
    _expect_line("        [BUILTIN] [CONSTANT] [COMPONLY] [IMMEDIATE]\n");
    _expect_line("    (native code @ 0x123)\n");
    e4t__ASSERT_EQ(state, NULL);

    /* Check threaded definition. */
    _s_setup("threaded-0x123");
    _expect_line(": threaded-0x123 ( threaded - user: 0x0 )\n");
    _expect_line("        [IMMEDIATE]\n");
    _expect_line("    native-0x123\n");
    _expect_line("    native-0xfed\n");
    _expect_line("    0x8999\n");
    _expect_line("    native-0xabc\n");
    _expect_line("    SENTINEL\n");
    e4t__ASSERT_EQ(state, NULL);

    /* Check threaded thunk definition. */
    _s_setup("threaded-thunk-0x123");
    _expect_formatted_line(
            ": threaded-thunk-0x123 ( threaded thunk - user: 0x%lX )\n",
            threaded_xt->data);
    _expect_line("    native-0x123\n");
    _expect_line("    native-0xfed\n");
    _expect_line("    0x8999\n");
    _expect_line("    native-0xabc\n");
    _expect_line("    SENTINEL\n");
    e4t__ASSERT_EQ(state, NULL);

    /* Check defer thunk that refers to threaded code in the
       dictionary. */
    _s_setup("defer-thunk-0x123");
    _expect_formatted_line(
            ": defer-thunk-0x123 ( defer thunk - user: 0x%lX )\n",
            threaded_xt);
    _expect_line("> threaded-0x123 ( threaded - user: 0x0 )\n");
    _expect_line("        [IMMEDIATE]\n");
    _expect_line("    native-0x123\n");
    _expect_line("    native-0xfed\n");
    _expect_line("    0x8999\n");
    _expect_line("    native-0xabc\n");
    _expect_line("    SENTINEL\n");
    e4t__ASSERT_EQ(state, NULL);

    /* Check defer thunk that refers to anonymous native code. */
    _s_setup("defer-thunk-0x765");
    _expect_formatted_line(
            ": defer-thunk-0x765 ( defer thunk - user: 0x%lX )\n", &dt_xt);
    _expect_line("> _ ( native - user: 0x777 )\n");
    _expect_line("    (native code @ 0x789)\n");
    e4t__ASSERT_EQ(state, NULL);

    /* Check defer thunk that refers to a defer thunk. */
    _s_setup("defer-thunk-0xf00");
    _expect_formatted_line(
            ": defer-thunk-0xf00 ( defer thunk - user: 0x%lX )\n", dtdt_xt);
    _expect_line("        [IMMEDIATE]\n");
    _expect_formatted_line(
            "> defer-thunk-0x765 ( defer thunk - user: 0x%lX )\n", &dt_xt);
    _expect_line("> _ ( native - user: 0x777 )\n");
    _expect_line("    (native code @ 0x789)\n");
    e4t__ASSERT_EQ(state, NULL);

    /* Check does> thunk. */
    _s_setup("does-thunk-0x123");
    _expect_formatted_line(
            ": does-thunk-0x123 ( does thunk - user: 0x%lX )\n",
            does_thunk_addr);
    _expect_formatted_line("    (data @ 0x%lX)\n", does_thunk_data_addr);
    _expect_line("    native-0xfed\n");
    _expect_line("    native-0xfed\n");
    _expect_line("    SENTINEL\n");
    e4t__ASSERT_EQ(state, NULL);

    /* Check value. */
    _s_setup("value-0x123");
    _expect_line(": value-0x123 ( value - user: 0x0 )\n");
    _expect_line("    0x9001\n");
    e4t__ASSERT_EQ(state, NULL);

    /* Check variable. */
    _s_setup("variable-0x123");
    _expect_line(": variable-0x123 ( variable - user: 0x0 )\n");
    _expect_formatted_line("    (data @ 0x%lX)\n", variable_data_addr);
    e4t__ASSERT_EQ(state, NULL);

    /* Check user value. */
    _s_setup("userval-0x123");
    _expect_line(": userval-0x123 ( user value - user: 0x123 )\n");
    _expect_line("");
    e4t__ASSERT_EQ(state, NULL);

    /* Check user variable. */
    _s_setup("uservar-0x555");
    _expect_line(": uservar-0x555 ( user variable - user: 0x555 )\n");
    _expect_line("");
    e4t__ASSERT_EQ(state, NULL);

    /* Check marker. */
    _s_setup("marker-HERE");
    _expect_formatted_line(
            ": marker-HERE ( marker - user: 0x%lX )\n", marker_here_addr);
    _expect_line("");
    e4t__ASSERT_EQ(state, NULL);

    /* Check dictionary entry that will overflow. */
    _s_setup(_long_name);
    line_len = _s_line();
    e4t__ASSERT_EQ(line_len, -1);
    e4t__ASSERT_EQ(state, NULL);

    /* Check dictionary entry that refers to the entry that will
       overflow. */
    _s_setup("threaded-0xdac");
    _expect_line(": threaded-0xdac ( threaded - user: 0x0 )\n");
    _expect_line("        [BUILTIN] [COMPONLY]\n");
    _expect_line("    native-0x123\n");
    line_len = _s_line();
    e4t__ASSERT_EQ(line_len, -1);
    e4t__ASSERT_EQ(state, NULL);

    #undef _long_name
    #undef _expect_formatted_line
    #undef _expect_line
    #undef _s_line
    #undef _s_setup
    #undef _c
    #undef _l
    #undef _d
    #undef _df
}

static void e4t__test_util_numformat(void)
{
    static char buf[31] = {0,};

    #define _f(n, b, f) e4__usize_format(n, b, f, buf, sizeof(buf) - 1)
    e4t__ASSERT_MATCH(_f(531, 10, 0), "531");
    e4t__ASSERT_MATCH(_f(0xf3, 16, 0), "f3");
    e4t__ASSERT_MATCH(_f(-0xf3, 16, 0), "ffffffffffffff0d");
    e4t__ASSERT_MATCH(_f(-0xf3, 16, e4__F_SIGNED), "-f3");
    e4t__ASSERT_MATCH(_f(46655, 36, e4__F_SIGNED), "zzz");
    e4t__ASSERT_MATCH(_f(-50, 2, e4__F_SIGNED), "-110010");
    e4t__ASSERT_MATCH(_f(0, 2, e4__F_SIGNED), "0");
    #undef _f
}

static void e4t__test_util_numparse(void)
{
    /* XXX: Parts of this test only work correctly on a system that
       represents negative numbers using two's complement. */
    e4__usize num = 0;

    #define _p(n, b, f) e4__mem_number(n, sizeof(n) - 1, b, f, &num)
    #define _f  (e4__F_CHAR_LITERAL | e4__F_NEG_PREFIX | e4__F_BASE_PREFIX)

    e4t__ASSERT_EQ(_p("0xac", 10, _f), 4);
    e4t__ASSERT_EQ(num, 172);

    e4t__ASSERT_EQ(_p("ac", 16, _f), 2);
    e4t__ASSERT_EQ(num, 172);

    e4t__ASSERT_EQ(_p("ac", 5, _f), 0);
    e4t__ASSERT_EQ(num, 172);

    e4t__ASSERT_EQ(_p("32ac", 5, _f), 2);
    e4t__ASSERT_EQ(num, 17);

    e4t__ASSERT_EQ(_p("$ac", 5, _f), 3);
    e4t__ASSERT_EQ(num, 172);

    e4t__ASSERT_EQ(_p("-$ac", 5, _f), 4);
    e4t__ASSERT_EQ(num, -172);

    e4t__ASSERT_EQ(_p("%011011", 5, _f), 7);
    e4t__ASSERT_EQ(num, 27);

    e4t__ASSERT_EQ(_p("0b011011", 5, _f), 8);
    e4t__ASSERT_EQ(num, 27);

    e4t__ASSERT_EQ(_p("0o777xx", 5, _f), 5);
    e4t__ASSERT_EQ(num, 511);

    e4t__ASSERT_EQ(_p("#777   ", 5, _f), 4);
    e4t__ASSERT_EQ(num, 777);

    e4t__ASSERT_EQ(_p("'a'jieoag", 5, _f), 3);
    e4t__ASSERT_EQ(num, 97);

    e4t__ASSERT_EQ(_p("zOo", 36, _f), 3);
    e4t__ASSERT_EQ(num, 46248);

    e4t__ASSERT_EQ(_p("zOo", 99, _f), 3);
    e4t__ASSERT_EQ(num, 46248);

    e4t__ASSERT_EQ(_p("000", 10, 0), 3);
    e4t__ASSERT_EQ(num, 0);

    e4t__ASSERT_EQ(_p("-5", 10, 0), 0);
    e4t__ASSERT_EQ(num, 0);

    e4t__ASSERT_EQ(_p("$5", 10, 0), 0);
    e4t__ASSERT_EQ(num, 0);

    e4t__ASSERT_EQ(_p("'5'", 10, 0), 0);
    e4t__ASSERT_EQ(num, 0);

    #undef _f
    #undef _p
}

static void e4t__test_util_strnescape(void)
{
    static const char *escaped_str = "foo\\abar\\b some more \\e\\f\\l"
        "and still more\\mand\\nmore\\q\\r\\tso long\\v\\z\\\"\\\\\\xf3"
        "\\p\\xr9\\";
    char buf[2];
    e4__usize chunk_len;
    const char *cursor = escaped_str;
    e4__usize len = strlen(cursor);
    const char *chunk;

    #define _expect_chunk(s)    \
        do {    \
            chunk = e4__mem_strnescape(&cursor, &len, &chunk_len, buf); \
            e4t__ASSERT(chunk); \
            e4t__ASSERT_EQ(chunk_len, sizeof(s) - 1);   \
            e4t__ASSERT(!e4__mem_strncasecmp(chunk, s, sizeof(s) - 1)); \
        } while (0)

    _expect_chunk("foo");
    _expect_chunk("\a");
    _expect_chunk("bar");
    _expect_chunk("\b");
    _expect_chunk(" some more ");
    _expect_chunk("\033");
    _expect_chunk("\f");
    _expect_chunk("\n");
    _expect_chunk("and still more");
    _expect_chunk("\r\n");
    _expect_chunk("and");
    _expect_chunk("\n");
    _expect_chunk("more");
    _expect_chunk("\"");
    _expect_chunk("\r");
    _expect_chunk("\t");
    _expect_chunk("so long");
    _expect_chunk("\v");
    _expect_chunk("\0");
    _expect_chunk("\"");
    _expect_chunk("\\");
    _expect_chunk("\xf3");
    _expect_chunk("\\p");
    _expect_chunk("\\xr9");
    _expect_chunk("\\");

    chunk = e4__mem_strnescape(&cursor, &len, &chunk_len, buf);
    e4t__ASSERT_EQ(len, 0);
    e4t__ASSERT_EQ(chunk, NULL);

    chunk = e4__mem_strnescape(&cursor, &len, &chunk_len, buf);
    e4t__ASSERT_EQ(len, 0);
    e4t__ASSERT_EQ(chunk, NULL);

    #undef _expect_chunk
}

static void e4t__test_util_togmt(void)
{
    /* XXX: Parts of this test only work correctly on a 64 bit
       system. */
    struct e4__gmt gmt = {0};

    #define _e4t__ASSERT_GMT(g, y, mo, d, h, mn, s) \
        do {    \
            e4__usize_togmt(g, &gmt);   \
            e4t__ASSERT_EQ(gmt.year, y);    \
            e4t__ASSERT_EQ(gmt.mon, mo);    \
            e4t__ASSERT_EQ(gmt.mday, d);    \
            e4t__ASSERT_EQ(gmt.hour, h);    \
            e4t__ASSERT_EQ(gmt.min, mn);    \
            e4t__ASSERT_EQ(gmt.sec, s);     \
        } while (0)

    _e4t__ASSERT_GMT(1631662695, 2021, 9, 14, 23, 38, 15);
    _e4t__ASSERT_GMT(946684800, 2000, 1, 1, 0, 0, 0);
    _e4t__ASSERT_GMT(31536000, 1971, 1, 1, 0, 0, 0);
    _e4t__ASSERT_GMT(1078041843, 2004, 2, 29, 8, 4, 3);
    _e4t__ASSERT_GMT(699382985, 1992, 2, 29, 17, 3, 5);
    _e4t__ASSERT_GMT(93539898187, 4934, 2, 28, 17, 3, 7);
    _e4t__ASSERT_GMT(978220800, 2000, 12, 31, 0, 0, 0);
    _e4t__ASSERT_GMT(4102444800, 2100, 1, 1, 0, 0, 0);
    _e4t__ASSERT_GMT(4107535860, 2100, 2, 28, 22, 11, 0);
    _e4t__ASSERT_GMT(4133894400, 2100, 12, 31, 0, 0, 0);
    _e4t__ASSERT_GMT(4107556982, 2100, 3, 1, 4, 3, 2);
    _e4t__ASSERT_GMT(13574643060, 2400, 2, 29, 22, 11, 0);
    _e4t__ASSERT_GMT(68270582, 1972, 3, 1, 4, 3, 2);
    _e4t__ASSERT_GMT(349056312, 1981, 1, 23, 0, 5, 12);
    _e4t__ASSERT_GMT(192406260, 1976, 2, 5, 22, 11, 0);

    _e4t__ASSERT_GMT(158191860, 1975, 1, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(160870260, 1975, 2, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(163289460, 1975, 3, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(165967860, 1975, 4, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(168559860, 1975, 5, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(171238260, 1975, 6, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(173830260, 1975, 7, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(176508660, 1975, 8, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(179187060, 1975, 9, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(181779060, 1975, 10, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(184457460, 1975, 11, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(187049460, 1975, 12, 5, 22, 11, 0);

    _e4t__ASSERT_GMT(158191860 + 31536000 , 1976, 1, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(160870260 + 31536000 , 1976, 2, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(163289460 + 31536000 + 86400, 1976, 3, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(165967860 + 31536000 + 86400, 1976, 4, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(168559860 + 31536000 + 86400, 1976, 5, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(171238260 + 31536000 + 86400, 1976, 6, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(173830260 + 31536000 + 86400, 1976, 7, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(176508660 + 31536000 + 86400, 1976, 8, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(179187060 + 31536000 + 86400, 1976, 9, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(181779060 + 31536000 + 86400, 1976, 10, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(184457460 + 31536000 + 86400, 1976, 11, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(187049460 + 31536000 + 86400, 1976, 12, 5, 22, 11, 0);

    _e4t__ASSERT_GMT(3313951860, 2075, 1, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3316630260, 2075, 2, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3319049460, 2075, 3, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3321727860, 2075, 4, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3324319860, 2075, 5, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3326998260, 2075, 6, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3329590260, 2075, 7, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3332268660, 2075, 8, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3334947060, 2075, 9, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3337539060, 2075, 10, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3340217460, 2075, 11, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3342809460, 2075, 12, 5, 22, 11, 0);

    _e4t__ASSERT_GMT(3313951860 + 31536000, 2076, 1, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3316630260 + 31536000, 2076, 2, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3319049460 + 31536000 + 86400, 2076, 3, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3321727860 + 31536000 + 86400, 2076, 4, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3324319860 + 31536000 + 86400, 2076, 5, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3326998260 + 31536000 + 86400, 2076, 6, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3329590260 + 31536000 + 86400, 2076, 7, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3332268660 + 31536000 + 86400, 2076, 8, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3334947060 + 31536000 + 86400, 2076, 9, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3337539060 + 31536000 + 86400, 2076, 10, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3340217460 + 31536000 + 86400, 2076, 11, 5, 22, 11, 0);
    _e4t__ASSERT_GMT(3342809460 + 31536000 + 86400, 2076, 12, 5, 22, 11, 0);

    #undef _e4t__ASSERT_GMT
}

static void e4t__test_util_wordparse(void)
{
    const char *word;
    e4__usize len;

    #define _p(s, d, f) e4__mem_parse(s, d, strlen(s), f, &word)
    len = _p("   foo bar bas", ' ', e4__F_SKIP_LEADING);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo", len));

    len = _p("   foo, bar,, bas", ',', e4__F_SKIP_LEADING);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "   foo", len));

    len = _p("         ", ' ', e4__F_SKIP_LEADING);
    e4t__ASSERT_EQ(len, 0);

    len = _p("  foo\nbar", ' ', e4__F_SKIP_LEADING);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo", len));

    len = _p("  foo.bar", ' ', e4__F_SKIP_LEADING);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo.bar", len));

    len = _p("  foo.bar", ' ', 0);
    e4t__ASSERT_EQ(len, 0);

    e4t__ASSERT_EQ((len = _p("foo\\\"bar\" bas", '"', 0)), 4);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo\\\"", len));

    e4t__ASSERT_EQ((len = _p("foo\\\"bar\" bas", '"', e4__F_IGNORE_ESC)), 8);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo\\\"bar", len));

    #undef _p
}

void e4t__test_util(void)
{
    e4t__test_util_double();
    e4t__test_util_double_div();
    e4t__test_util_double_mul();
    e4t__test_util_double_ndiv();
    e4t__test_util_cbuf();
    e4t__test_util_exceptionformat();
    e4t__test_util_math();
    e4t__test_util_mem_dict();
    e4t__test_util_mem_dump();
    e4t__test_util_mem_pno();
    e4t__test_util_mem_see();
    e4t__test_util_numformat();
    e4t__test_util_numparse();
    e4t__test_util_strnescape();
    e4t__test_util_togmt();
    e4t__test_util_wordparse();
}
