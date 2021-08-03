#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

/* Covers REFILL WORD */
static void e4t__test_builtin_parseword(void)
{
    struct e4__task *task = e4t__transient_task();
    char *res;
    e4__u8 len;

    e4t__term_ibuf_feed("first second third", -1);

    e4__execute(task, (void *)&e4__BUILTIN_XT[e4__B_REFILL]);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ((e4__usize)e4__stack_pop(task), e4__BF_TRUE);

    e4__stack_push(task, (e4__cell)' ');
    e4__execute(task, (void *)&e4__BUILTIN_XT[e4__B_WORD]);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 5);
    e4t__ASSERT(!e4__mem_strncasecmp(res, "first", len));

    e4__stack_push(task, (e4__cell)' ');
    e4__execute(task, (void *)&e4__BUILTIN_XT[e4__B_WORD]);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 6);
    e4t__ASSERT(!e4__mem_strncasecmp(res, "second", len));

    e4__stack_push(task, (e4__cell)' ');
    e4__execute(task, (void *)&e4__BUILTIN_XT[e4__B_WORD]);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 5);
    e4t__ASSERT(!e4__mem_strncasecmp(res, "third", len));

    e4__stack_push(task, (e4__cell)' ');
    e4__execute(task, (void *)&e4__BUILTIN_XT[e4__B_WORD]);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 0);
}

/* Covers CLEAR .S DROP DUP OVER ROT SWAP TUCK ROLL */
static void e4t__test_builtin_stackmanip(void)
{
    struct e4__task *task = e4t__transient_task();

    e4t__ASSERT_OK(e4__evaluate(task, "1 -2 3 .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 1 -2 3");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 clear .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<0>");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 drop .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<2> 1 2");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 dup .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 2 3 3");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 over .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 2 3 2");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 rot .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 2 3 1");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 swap .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 1 3 2");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 tuck .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 3 2 3");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 4 5 4 roll .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<5> 1 3 4 5 2");
}

void e4t__test_builtin(void)
{
    e4t__test_builtin_parseword();
    e4t__test_builtin_stackmanip();
}
