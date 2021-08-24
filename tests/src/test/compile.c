#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

/* Covers ELSE IF THEN */
static void e4t__test_compile_conditional(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that any attempt to compile an unbalanced control
       structure fails. */
    e4t__ASSERT_EQ(e4__evaluate(task, ": foo if ;", -1), e4__E_CSMISMATCH);
    e4t__ASSERT(!e4__task_compiling(task));
    e4t__ASSERT_EQ(e4__evaluate(task, ": foo else", -1), e4__E_CSMISMATCH);
    e4t__ASSERT(!e4__task_compiling(task));
    e4t__ASSERT_EQ(e4__evaluate(task, ": foo then", -1), e4__E_CSMISMATCH);
    e4t__ASSERT(!e4__task_compiling(task));
    e4t__ASSERT_EQ(e4__evaluate(task, ": foo if else ;", -1),
            e4__E_CSMISMATCH);
    e4t__ASSERT(!e4__task_compiling(task));
    e4__stack_clear(task);

    /* Test that empty if/then and empty if/else/then sequences
       correctly do nothing. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo if then ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "5 true foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
    e4t__ASSERT_OK(e4__evaluate(task, "5 false foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    e4t__ASSERT_OK(e4__evaluate(task, ": foo if else then ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "5 true foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
    e4t__ASSERT_OK(e4__evaluate(task, "5 false foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    /* Test that IF throws the appropriate exception on underflow. */
    e4t__ASSERT_EQ(e4__evaluate(task, "foo", -1), e4__E_STKUNDERFLOW);

    /* Test that if/then works as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo if 100 then ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "true foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 100);
    e4t__ASSERT_OK(e4__evaluate(task, "false foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    /* Ensure that IF works correctly with any nonzero value. */
    e4t__ASSERT_OK(e4__evaluate(task, "0o3071250 foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 100);
    e4t__ASSERT_OK(e4__evaluate(task, "0b1100101 foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 100);
    e4t__ASSERT_OK(e4__evaluate(task, "-375 foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 100);

    /* Test that if/then/else works as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo if 100 else 50 then ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "true foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 100);
    e4t__ASSERT_OK(e4__evaluate(task, "false foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 50);

    /* Ensure that multiple layers of nesting works as expected. */
    e4t__ASSERT_OK(e4__evaluate(task,
                ": foo if drop 100 else if 50 else 25 then then ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "true true foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 100);
    e4t__ASSERT_OK(e4__evaluate(task, "true false foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 50);
    e4t__ASSERT_OK(e4__evaluate(task, "false false foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 25);
}

/* Covers DOES> (compile semantics) */
static void e4t__test_compile_does(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that DOES> behaves correctly at compile time. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo create , does> @ 10 + ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "5 foo bar bar", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);

    /* Test that double DOES> works as expected. */
    e4t__ASSERT_OK(e4__evaluate(task,
            ": foo create , does> @ 10 + does> @ 67 + ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "5 foo bar", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "create bas 33 , bas @", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 33);
    e4t__ASSERT_OK(e4__evaluate(task, "bar", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);
    e4t__ASSERT_OK(e4__evaluate(task, "bas", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 100);
}

/* Covers compilation failure do to stack mismatch and implicitly
   and explicit compile cancellation. */
static void e4t__test_compile_failure(void)
{
    struct e4__task *task = e4t__transient_task();
    e4__cell here;

    /* Test that a stack mismatch at compilation start and end throws
       the appropriate exception, cancels compilation, and restores
       HERE. */
    here = e4__task_uservar(task, e4__UV_HERE);
    e4__stack_push(task, (e4__cell)0x1234);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 2", -1));
    e4__stack_push(task, (e4__cell)0x5678);
    e4t__ASSERT_EQ(e4__evaluate(task, ";", -1), e4__E_CSMISMATCH);
    e4__stack_clear(task);
    e4t__ASSERT_EQ(e4__dict_lookup(task, "foo", 3), NULL);
    e4t__ASSERT_EQ(e4__task_uservar(task, e4__UV_HERE), here);

    /* Test that the above is true for :NONAME as well. */
    here = e4__task_uservar(task, e4__UV_HERE);
    e4t__ASSERT_OK(e4__evaluate(task, ":noname 2", -1));
    e4__stack_push(task, (e4__cell)0x5678);
    e4t__ASSERT_EQ(e4__evaluate(task, ";", -1), e4__E_CSMISMATCH);
    e4__stack_clear(task);
    e4t__ASSERT_EQ(e4__task_uservar(task, e4__UV_HERE), here);

    /* Test that the above is true when compiling with DOES> as well
       and additionally that the failure does not corrupt the existing
       semantics of the most recent dictionary definition. */
    e4t__ASSERT_OK(e4__evaluate(task, "create bar 46810 ,", -1));
    here = e4__task_uservar(task, e4__UV_HERE);
    e4t__ASSERT_OK(e4__evaluate(task, "does> @ 2", -1));
    e4__stack_push(task, (e4__cell)0x5678);
    e4t__ASSERT_EQ(e4__evaluate(task, ";", -1), e4__E_CSMISMATCH);
    e4__stack_clear(task);
    e4t__ASSERT_EQ(e4__task_uservar(task, e4__UV_HERE), here);
    e4t__ASSERT_OK(e4__evaluate(task, "bar @", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 46810);

    /* Test that all of the above are true even when compilation is
       canceled while suspended. */
    here = e4__task_uservar(task, e4__UV_HERE);

    e4t__ASSERT_OK(e4__evaluate(task, ": foo 324 [", -1));
    e4__compile_cancel(task);
    e4__stack_clear(task);
    e4t__ASSERT_EQ(e4__task_uservar(task, e4__UV_HERE), here);

    e4t__ASSERT_OK(e4__evaluate(task, ":noname 59 [", -1));
    e4__compile_cancel(task);
    e4__stack_clear(task);
    e4t__ASSERT_EQ(e4__task_uservar(task, e4__UV_HERE), here);

    e4t__ASSERT_OK(e4__evaluate(task, "create foo 71 , ", -1));
    here = e4__task_uservar(task, e4__UV_HERE);
    e4t__ASSERT_OK(e4__evaluate(task, "does> @ [", -1));
    e4__compile_cancel(task);
    e4__stack_clear(task);
    e4t__ASSERT_EQ(e4__task_uservar(task, e4__UV_HERE), here);

    /* Test that a dictionary entry is not available while it is being
       compiled. */
    here = e4__task_uservar(task, e4__UV_HERE);
    e4t__ASSERT_EQ(e4__evaluate(task, ": word-5738 word-5738", -1),
                e4__E_UNDEFWORD);
    e4__stack_clear(task);
    e4__compile_cancel(task);
    e4t__ASSERT_EQ(e4__evaluate(task, ": word-5738 ['] word-5738", -1),
                e4__E_UNDEFWORD);
    e4__stack_clear(task);
    e4__compile_cancel(task);
    e4t__ASSERT_EQ(e4__task_uservar(task, e4__UV_HERE), here);
}

/* Covers : ; and linear compilation semantics */
static void e4t__test_compile_linear(void)
{
    struct e4__task *task = e4t__transient_task();
    const struct e4__dict_header *header;

    /* Test that attempting to define a word with no name
       throws an exception. */
    e4t__ASSERT_EQ(e4__evaluate(task, ":", -1), e4__E_ZLNAME);
    e4t__ASSERT_EQ(e4__evaluate(task, ":   ", -1), e4__E_ZLNAME);

    /* Test that compiled words are added to the dictionary. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo ;", -1));
    e4t__ASSERT(e4__dict_lookup(task, "foo", 3));
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));

    /* Test that simply numeric compilation words. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 17 ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 17);

    /* Test that compiling over a word works as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 43 ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 43);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 17);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));

    /* Test that compilation state is preserved across multiple
       evaluate calls. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "2", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "3", -1));
    e4t__ASSERT(e4__task_compiling(task));
    e4t__ASSERT_OK(e4__evaluate(task, "4 ;", -1));
    e4t__ASSERT_EQ(e4__task_compiling(task), 0);
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));

    /* Test that simple linear compilation is possible. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 2 5 + ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": bar dup 1 + swap ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 7);
    e4t__ASSERT_OK(e4__evaluate(task, "foo bar", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 7);
    e4t__ASSERT_EQ(e4__stack_pop(task), 8);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));

    /* Test that compiled code is as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 2 5 + ;", -1));
    e4t__ASSERT((header = e4__dict_lookup(task, "foo", 3)));
    e4t__ASSERT_EQ(header->xt->code, e4__execute_threaded);
    e4t__ASSERT_EQ(header->xt->data[0], &e4__BUILTIN_XT[e4__B_LIT_CELL]);
    e4t__ASSERT_EQ(header->xt->data[1], 2);
    e4t__ASSERT_EQ(header->xt->data[2], &e4__BUILTIN_XT[e4__B_LIT_CELL]);
    e4t__ASSERT_EQ(header->xt->data[3], 5);
    e4t__ASSERT_EQ(header->xt->data[4], &e4__BUILTIN_XT[e4__B_PLUS]);
    e4t__ASSERT_EQ(header->xt->data[5], &e4__BUILTIN_XT[e4__B_EXIT]);
    e4t__ASSERT_EQ(header->xt->data[6], &e4__BUILTIN_XT[e4__B_SENTINEL]);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));
}

static void e4t__test_compile_literal(void)
{
    struct e4__task *task = e4t__transient_task();

    e4t__ASSERT_OK(e4__evaluate(task, ": foo", -1));
    e4__stack_push(task, (e4__cell)2);
    e4t__ASSERT_OK(e4__evaluate(task, "literal 3 + ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
}

static void e4t__test_compile_noname(void)
{
    struct e4__task *task = e4t__transient_task();

    e4t__ASSERT_OK(e4__evaluate(task, ":noname 2 5 + ;", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_OK(e4__evaluate(task, "execute", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 7);
}

static void e4t__test_compile_pponeimmed(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that IMMEDIATE on a builtin doesn't work. */
    e4t__ASSERT_EQ(e4__evaluate(task, "immediate", -1), e4__E_INVBUILTINMUT);

    /* Test that IMMEDIATE causes a word to execute at compile time. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 2 ; immediate", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": bar foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_peek(task), 2);
    e4t__ASSERT_OK(e4__evaluate(task, "literal ; bar", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_peek(task), 2);

    /* Test that POSTPONE on an undefined word throws an exception. */
    e4t__ASSERT_EQ(e4__evaluate(task, ": foo postpone not-a-word", -1),
            e4__E_UNDEFWORD);
    e4__stack_clear(task);
    e4__compile_cancel(task);

    /* Test that POSTPONE compiles the xt of a word, immediate
       or not. */
    e4t__ASSERT_OK(e4__evaluate(task, ": // postpone \\ ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "5 // 1 2 3 4", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    e4t__ASSERT_OK(e4__evaluate(task, ": plus postpone + ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "5 10 plus", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);

    /* Test that POSTPONE and IMMEDIATE play nicely together. */
    e4t__ASSERT_OK(e4__evaluate(task, ": endif postpone then ; immediate",
            -1));
    e4t__ASSERT_OK(e4__evaluate(task,
            ": is-zero? 0= if true else false endif ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "5 is-zero?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "0 is-zero?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);

    /* Test that the RESTRICT word will not modify a builtin. */
    task = e4t__transient_task();
    e4t__ASSERT_EQ(e4__evaluate(task, "restrict", -1), e4__E_INVBUILTINMUT);

    /* Test that the RESTRICT word makes a word compile only. */
    e4t__ASSERT_OK(e4__evaluate(task, ": minus - ; restrict", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "10 5 minus", -1), e4__E_COMPONLYWORD);
    e4t__ASSERT_OK(e4__evaluate(task, ": bas 10 5 minus ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "bas", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
}

static void e4t__test_compile_recursive(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that RECURSE works correctly in colon definitions. */
    e4t__ASSERT_OK(e4__evaluate(task, ": factorial "
            "dup 2 < if drop 1 exit then dup 1 - recurse * ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "5 factorial", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 120);
    e4t__ASSERT_OK(e4__evaluate(task, "10 factorial", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3628800);

    /* Test that RECURSE works correctly in :NONAME definitions. */
    e4t__ASSERT_OK(e4__evaluate(task, ":noname "
            "dup 2 < if drop 1 exit then dup 1 - recurse + ; constant s", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "10 s execute", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 55);
    e4t__ASSERT_OK(e4__evaluate(task, "15 s execute", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 120);
}

/* Covers ." C" S" compile time semantics */
static void e4t__test_compile_string(void)
{
    struct e4__task *task = e4t__transient_task();
    const char *str;

    e4t__term_obuf_consume();

    /* Test that string compilation is possible. */
    e4t__ASSERT_OK(e4__evaluate(task,
            ": counted-foo c\" Some counted string.\" ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task,
            ": string-foo s\" Some normal string.\" ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task,
            ": printed-foo .\" Some printed string.\" ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task,
            ": escaped-foo s\\\" Some escaped string.\" ;", -1));

    /* Test that the results of that compilation are as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, "counted-foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    str = (const char *)e4__stack_pop(task);
    e4t__ASSERT_EQ(*str++, 20);
    e4t__ASSERT(!e4__mem_strncasecmp(str, "Some counted string.", 20));

    e4t__ASSERT_OK(e4__evaluate(task, "string-foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 19);
    str = (const char *)e4__stack_pop(task);
    e4t__ASSERT(!e4__mem_strncasecmp(str, "Some normal string.", 19));

    e4t__ASSERT_OK(e4__evaluate(task, "printed-foo", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "Some printed string.");

    e4t__ASSERT_OK(e4__evaluate(task, "escaped-foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 20);
    str = (const char *)e4__stack_pop(task);
    e4t__ASSERT(!e4__mem_strncasecmp(str, "Some escaped string.", 20));

    /* Test that zero length strings behave correctly. */
    e4t__ASSERT_OK(e4__evaluate(task, "forget counted-foo", -1));

    e4t__ASSERT_OK(e4__evaluate(task, ": foo c\" \" ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": bar s\" \" ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": bas .\" \" ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": quux s\\\" \" ;", -1));

    e4t__ASSERT_OK(e4__evaluate(task, "foo count", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4__stack_clear(task);

    e4t__ASSERT_OK(e4__evaluate(task, "bar", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4__stack_clear(task);

    e4t__ASSERT_OK(e4__evaluate(task, "bas", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "");

    e4t__ASSERT_OK(e4__evaluate(task, "quux", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4__stack_clear(task);

    /* Test that escaped strings are escaped correctly. */
    e4t__ASSERT_OK(e4__evaluate(task,
            ": escaped-str s\\\" This is an \\\"escaped\\\" string\\r\\n"
            "\\x4F\\x48 YEAH!\" ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "escaped-str", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 37);
    str = (const char *)e4__stack_pop(task);
    e4t__ASSERT(!e4__mem_strncasecmp(str,
            "This is an \"escaped\" string\r\nOH YEAH!", 37));
}

static void e4t__test_compile_suspendresume(void)
{
    struct e4__task *task = e4t__transient_task();
    e4__cell here;

    e4t__term_obuf_consume();

    /* Test that suspending and resuming works correctly in trivial
       cases. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo [ 2 2 + ] literal ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);

    e4t__ASSERT_OK(e4__evaluate(task, ":noname [ 50 ] literal ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "execute", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 50);

    e4t__ASSERT_OK(e4__evaluate(task, "create foo 70 , does> [ 1234 . ] @ ;",
            -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1234 ");
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "");
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 70);

    /* Test that it is possible to finish compilation while
       suspended, no matter what type of compilation is occurring. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 324 [", -1));
    e4t__ASSERT_OK(e4__compile_finish(task));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 324);

    e4t__ASSERT_OK(e4__evaluate(task, ":noname 59 [", -1));
    e4t__ASSERT_OK(e4__compile_finish(task));
    e4t__ASSERT_OK(e4__evaluate(task, "execute", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 59);

    e4t__ASSERT_OK(e4__evaluate(task, "create bar 71 , does> @ [", -1));
    e4t__ASSERT_OK(e4__compile_finish(task));
    e4t__ASSERT_OK(e4__evaluate(task, "bar", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 71);

    /* Test that anonymous compilation resume works correctly. */
    here = e4__task_uservar(task, e4__UV_HERE);
    e4t__ASSERT_OK(e4__evaluate(task, "] 2 2 + [", -1));
    e4t__ASSERT_EQ(e4__task_uservar(task, e4__UV_HERE), &here[5]);
    e4t__ASSERT_EQ(&e4__BUILTIN_XT[e4__B_LIT_CELL], here[0]);
    e4t__ASSERT_EQ(2, here[1]);
    e4t__ASSERT_EQ(&e4__BUILTIN_XT[e4__B_LIT_CELL], here[2]);
    e4t__ASSERT_EQ(2, here[3]);
    e4t__ASSERT_EQ(&e4__BUILTIN_XT[e4__B_PLUS], here[4]);
}

/* Covers ?DO +LOOP DO I J LEAVE LOOP UNLOOP and various other
   builtins */
static void e4t__test_compile_do_loop(void)
{
    struct e4__task *task = e4t__transient_task();

    e4t__term_obuf_consume();

    /* Test that simple loops work as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 5 0 do r@ . loop ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 1 2 3 4 ");

    /* Test that LEAVE works correctly. */
    e4t__ASSERT_OK(e4__evaluate(task,
            ": foo do "
                "i dup . dup "
                "5 = if "
                    "drop leave "
                "else 12 = if "
                    "leave "
                "then then "
                "loop "
                ";", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "7 3 foo", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "3 4 5 ");
    e4t__ASSERT_OK(e4__evaluate(task, "15 10 foo", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "10 11 12 ");

    /* Test that ?DO loops work correctly. */
    e4t__ASSERT_OK(e4__evaluate(task,
            ": foo do i  dup . 10 = if leave then loop ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "8 8 foo", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "8 9 10 ");
    e4t__ASSERT_OK(e4__evaluate(task,
            ": foo ?do i dup . 10 = if leave then loop ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "8 8 foo", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "");

    /* Test that +LOOP works as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 10 0 do i . 2 +loop ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 2 4 6 8 ");

    /* Test that nested do loops work correctly. */
    e4t__ASSERT_OK(e4__evaluate(task,
            ": foo 5 0 do i . ':' emit bl emit 5 i do i . j . loop cr loop ;",
            -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(),
        "0 : 0 0 1 0 2 0 3 0 4 0 \n"
        "1 : 1 1 2 1 3 1 4 1 \n"
        "2 : 2 2 3 2 4 2 \n"
        "3 : 3 3 4 3 \n"
        "4 : 4 4 \n");

    /* Test that UNLOOP is necessary and behaves as expected. */
    e4t__ASSERT_OK(e4__evaluate(task,
            ": foo 5 swap do i 5 > if exit then i . loop 500 . ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "2 foo", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "2 3 4 500 ");
    e4t__ASSERT_EQ(e4__evaluate(task, "9 foo", -1), e4__E_RSTKIMBALANCE);
    e4t__term_obuf_consume();

    e4t__ASSERT_OK(e4__evaluate(task, "forget foo forget foo forget foo", -1));
    e4t__ASSERT_OK(e4__evaluate(task,
            ": foo 5 swap do i 5 > if unloop exit then i . loop 500 . ;",
            -1));
    e4t__ASSERT_OK(e4__evaluate(task, "2 foo", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "2 3 4 500 ");
    e4t__ASSERT_OK(e4__evaluate(task, "9 foo", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "");
}

/* Covers AGAIN BEGIN UNTIL REPEAT WHILE and various other builtins */
static void e4t__test_compile_while_loop(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that looping words do nothing when balanced incorrectly. */
    e4t__ASSERT_EQ(e4__evaluate(task, ": foo begin ;", -1), e4__E_CSMISMATCH);
    e4t__ASSERT_EQ(e4__evaluate(task, ": foo again", -1), e4__E_CSMISMATCH);
    e4t__ASSERT_EQ(e4__evaluate(task, ": foo until", -1), e4__E_CSMISMATCH);
    e4t__ASSERT_EQ(e4__evaluate(task, ": foo while", -1), e4__E_STKUNDERFLOW);
    e4t__ASSERT(e4__task_compiling(task));
    e4__compile_cancel(task);
    e4t__ASSERT_EQ(e4__evaluate(task, ": foo repeat", -1), e4__E_CSMISMATCH);
    e4t__ASSERT_EQ(e4__evaluate(task, ": foo begin while ;", -1),
            e4__E_CSMISMATCH);
    e4t__ASSERT_EQ(e4__evaluate(task, "5 : foo while repeat ;", -1),
            e4__E_CSMISMATCH);
    e4__stack_clear(task);

    e4t__term_obuf_consume();

    /* Test that non-nested loops function correctly. */
    e4t__ASSERT_OK(e4__evaluate(task,
            ": broken-range >r begin dup 1+ dup r@ 1- < invert "
            "until r> drop ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "0 5 broken-range .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<5> 0 1 2 3 4 ");
    e4t__ASSERT_OK(e4__evaluate(task, "0 0 broken-range .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<2> 0 1 ");

    e4t__ASSERT_OK(e4__evaluate(task,
            ": consume depth if begin . depth 0= until then ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "0 1 2 3 4 5 consume", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "5 4 3 2 1 0 ");

    e4t__ASSERT_OK(e4__evaluate(task,
            ": infinite-consume begin depth 0= if exit then . again ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "0 1 2 3 4 5 infinite-consume", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "5 4 3 2 1 0 ");

    e4t__ASSERT_OK(e4__evaluate(task,
            ": range >r begin dup r@ < while dup 1+ repeat r> drop drop ;",
            -1));
    e4t__ASSERT_OK(e4__evaluate(task, "0 5 range .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<5> 0 1 2 3 4 ");
    e4t__ASSERT_OK(e4__evaluate(task, "0 0 range .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<0> ");

    /* Test that nested loops work as expected. */
    e4t__ASSERT_OK(e4__evaluate(task,
            ": foo "
                "1 begin "
                    "dup . ':' emit bl emit "
                    "dup begin "
                        "dup . 1+ "
                        "dup 4 > "
                    "until "
                    "drop cr 1+ "
                    "dup 4 > "
                "until ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(),
            "1 : 1 2 3 4 \n"
            "2 : 2 3 4 \n"
            "3 : 3 4 \n"
            "4 : 4 \n");
}

void e4t__test_compile(void)
{
    /* FIXME: Add direct compilation API tests? */
    e4t__test_compile_conditional();
    e4t__test_compile_does();
    e4t__test_compile_failure();
    e4t__test_compile_linear();
    e4t__test_compile_literal();
    e4t__test_compile_noname();
    e4t__test_compile_pponeimmed();
    e4t__test_compile_recursive();
    e4t__test_compile_suspendresume();
    e4t__test_compile_string();
    e4t__test_compile_do_loop();
    e4t__test_compile_while_loop();
}
