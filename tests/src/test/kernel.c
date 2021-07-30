#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

static void e4t__test_kernel_evaluate(void)
{
    struct e4__task *task = e4t__transient_task();

    /* FIXME: Add more evaluate tests. */

    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 4 5 4 rll .s clear", -1, 0),
            e4__E_UNDEFWORD);
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "");
}

static void e4t__test_kernel_numformat(void)
{
    static char buf[31] = {0,};

    #define _f(n, b, f) e4__num_format(n, b, f, buf, sizeof(buf) - 1)
    e4t__ASSERT_MATCH(_f(531, 10, 0), "531");
    e4t__ASSERT_MATCH(_f(0xf3, 16, 0), "f3");
    e4t__ASSERT_MATCH(_f(-0xf3, 16, 0), "ffffffffffffff0d");
    e4t__ASSERT_MATCH(_f(-0xf3, 16, e4__F_SIGNED), "-f3");
    e4t__ASSERT_MATCH(_f(46655, 36, e4__F_SIGNED), "zzz");
    e4t__ASSERT_MATCH(_f(-50, 2, e4__F_SIGNED), "-110010");
    e4t__ASSERT_MATCH(_f(0, 2, e4__F_SIGNED), "0");
    #undef _f
}

static void e4t__test_kernel_numparse(void)
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

static void e4t__test_kernel_math(void)
{
    /* XXX: Parts of this test only work correctly on a system that
       represents negative numbers using two's complement. */
    e4t__ASSERT_EQ(e4__USIZE_NEGATE((e4__usize)5), -5);
    e4t__ASSERT_EQ(e4__USIZE_IS_NEGATIVE((e4__usize)-5), 1);

    e4t__ASSERT_EQ(e4__num_sdiv((e4__usize)-10, (e4__usize)3), -3);
    e4t__ASSERT_EQ(e4__num_sdiv((e4__usize)10, (e4__usize)-3), -3);
    e4t__ASSERT_EQ(e4__num_sdiv((e4__usize)-10, (e4__usize)-3), 3);
    e4t__ASSERT_EQ(e4__num_sdiv((e4__usize)10, (e4__usize)3), 3);
}

void e4t__test_kernel(void)
{
    e4t__test_kernel_evaluate();
    e4t__test_kernel_math();
    e4t__test_kernel_numformat();
    e4t__test_kernel_numparse();
}
