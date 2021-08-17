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
    struct e4__dict_header *header;

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
    e4t__ASSERT_EQ(header->xt->data[0], &e4__BUILTIN_XT[e4__B_LITERAL]);
    e4t__ASSERT_EQ(header->xt->data[1], 2);
    e4t__ASSERT_EQ(header->xt->data[2], &e4__BUILTIN_XT[e4__B_LITERAL]);
    e4t__ASSERT_EQ(header->xt->data[3], 5);
    e4t__ASSERT_EQ(header->xt->data[4], &e4__BUILTIN_XT[e4__B_PLUS]);
    e4t__ASSERT_EQ(header->xt->data[5], &e4__BUILTIN_XT[e4__B_EXIT]);
    e4t__ASSERT_EQ(header->xt->data[6], &e4__BUILTIN_XT[e4__B_SENTINEL]);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));
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
    e4t__test_compile_noname();
    e4t__test_compile_recursive();
    e4t__test_compile_while_loop();
}
