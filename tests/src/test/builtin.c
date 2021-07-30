#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

/* Covers: CLEAR .S DROP DUP OVER ROT SWAP TUCK ROLL */
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
    e4t__test_builtin_stackmanip();
}
