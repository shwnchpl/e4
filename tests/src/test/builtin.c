#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

/* Covers FORGET and look-ahead idiom (which uses builtin WORD) */
static void e4t__test_builtin_forget(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that FORGET fails on zero length names. */
    e4t__ASSERT_EQ(e4__evaluate(task, "forget", -1, 0), e4__E_ZLNAME);
    e4t__ASSERT_EQ(e4__evaluate(task, "forget   ", -1, 0), e4__E_ZLNAME);

    /* Test that FORGET fails on invalid words and builtins. */
    e4t__ASSERT_EQ(e4__evaluate(task, "forget notaword", -1, 0),
            e4__E_INVFORGET);
    e4t__ASSERT_EQ(e4__evaluate(task, "forget dup", -1, 0), e4__E_INVFORGET);

    e4__dict_entry(task, "foo", 3, 0, NULL, NULL);
    e4__dict_entry(task, "bar", 3, 0, NULL, NULL);

    e4t__ASSERT(e4__dict_lookup(task, "foo", 3));
    e4t__ASSERT(e4__dict_lookup(task, "bar", 3));

    /* Test actually forgetting a word. */
    e4t__ASSERT_OK(e4__evaluate(task, "forget bar", -1, 0));
    e4t__ASSERT_EQ((e4__usize)e4__dict_lookup(task, "bar", 3),
            (e4__usize)NULL);

    /* Test that contents of buffer after look-ahead are intact
       and executed as appropriate. */
    e4t__ASSERT_OK(e4__evaluate(task, "forget    foo 17 dup", -1, 0));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ((e4__usize)e4__stack_pop(task), 17);
    e4t__ASSERT_EQ((e4__usize)e4__stack_pop(task), 17);
}

/* Covers >NUMBER and BASE uservar */
static void e4t__test_builtin_parsenum(void)
{
    struct e4__task *task = e4t__transient_task();
    e4__cell base = e4__task_uservar(task, e4__UV_BASE);
    e4__usize rcount;
    const char *remaining;
    e4__usize result;

    /* FIXME: Add more to this test once double-cell numbers have been
       implemented. */
    e4t__ASSERT_EQ((e4__usize)e4__DEREF(base), 10);

    #define _push(s, n) \
        do {    \
            e4__stack_push(task, (e4__cell)n);  \
            e4__stack_push(task, (e4__cell)0);  \
            e4__stack_push(task, (e4__cell)s);  \
            e4__stack_push(task, (e4__cell)(sizeof(s) - 1));    \
        } while (0)
    #define _pop() \
        do {    \
            rcount = (e4__usize)e4__stack_pop(task);    \
            remaining = (const char *)e4__stack_pop(task);  \
            e4__stack_pop(task);    \
            result = (e4__usize)e4__stack_pop(task);    \
        } while (0)

    _push("42", 0);
    e4t__ASSERT_OK(e4__evaluate(task, ">NUMBER", -1, 0));
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    _pop();
    e4t__ASSERT_EQ(result, 42);
    e4t__ASSERT_EQ(rcount, 0);

    _push("42 f", 0);
    e4t__ASSERT_OK(e4__evaluate(task, ">NUMBER", -1, 0));
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    _pop();
    e4t__ASSERT_EQ(result, 42);
    e4t__ASSERT_EQ(rcount, 2);
    e4t__ASSERT(!e4__mem_strncasecmp(remaining, " f", rcount));

    _push("-10", 50);
    e4t__ASSERT_OK(e4__evaluate(task, ">NUMBER", -1, 0));
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    _pop();
    e4t__ASSERT_EQ(result, 500);
    e4t__ASSERT_EQ(rcount, 3);
    e4t__ASSERT(!e4__mem_strncasecmp(remaining, "-10", rcount));

    e4__DEREF(base) = (e4__cell)16;
    _push("ff-3", 0);
    e4t__ASSERT_OK(e4__evaluate(task, ">NUMBER", -1, 0));
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    _pop();
    e4t__ASSERT_EQ(result, 255);
    e4t__ASSERT_EQ(rcount, 2);
    e4t__ASSERT(!e4__mem_strncasecmp(remaining, "-3", rcount));
    e4__DEREF(base) = (e4__cell)10;

    _push("1000", 4);
    e4t__ASSERT_OK(e4__evaluate(task, ">NUMBER", -1, 0));
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    _pop();
    e4t__ASSERT_EQ(result, 1040);
    e4t__ASSERT_EQ(rcount, 0);

    _push("'a'", 0);
    e4t__ASSERT_OK(e4__evaluate(task, ">NUMBER", -1, 0));
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    _pop();
    e4t__ASSERT_EQ(result, 0);
    e4t__ASSERT_EQ(rcount, 3);
    e4t__ASSERT(!e4__mem_strncasecmp(remaining, "'a'", rcount));

    #undef _pop
    #undef _push
}

/* Covers REFILL WORD */
static void e4t__test_builtin_parseword(void)
{
    struct e4__task *task = e4t__transient_task();
    char *res;
    e4__u8 len;

    e4t__term_ibuf_feed("first second third", -1);

    e4__builtin_exec(task, e4__B_REFILL);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ((e4__usize)e4__stack_pop(task), e4__BF_TRUE);

    e4__builtin_exec(task, e4__B_WORD, (e4__usize)' ');
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 5);
    e4t__ASSERT(!e4__mem_strncasecmp(res, "first", len));

    e4__builtin_exec(task, e4__B_WORD, (e4__usize)' ');
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 6);
    e4t__ASSERT(!e4__mem_strncasecmp(res, "second", len));

    e4__builtin_exec(task, e4__B_WORD, (e4__usize)' ');
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 5);
    e4t__ASSERT(!e4__mem_strncasecmp(res, "third", len));

    e4__builtin_exec(task, e4__B_WORD, (e4__usize)' ');
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 0);
}

/* Covers CLEAR .S DROP DUP OVER ROT SWAP TUCK ROLL QUIT */
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

    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 quit 4 5", -1, 0), e4__E_QUIT);
    e4t__ASSERT_OK(e4__evaluate(task, ".s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 1 2 3");
}

void e4t__test_builtin(void)
{
    e4t__test_builtin_forget();
    e4t__test_builtin_parsenum();
    e4t__test_builtin_parseword();
    e4t__test_builtin_stackmanip();
}
