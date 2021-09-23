#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

#include <string.h>

static void e4t__test_execute_badret_nret(struct e4__task *task,
        e4__cell user);

static void e4t__test_execute_badret(void)
{
    struct e4__execute_tuple nret_xt = {
        e4t__test_execute_badret_nret,
        (void *)2
    };
    const void *bad_ret[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)5,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)10,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    struct e4__task *task = e4t__transient_task();
    bad_ret[4] = &nret_xt;

    /* Check that returning twice "aborts" and doesn't end up reaching
       the 10 literal. */
    e4__execute(task, bad_ret);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    /* Check that over-returning many many times has the same
       behavior. */
    nret_xt.user = (void *)100;
    e4__execute(task, bad_ret);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    /* Check that this works correctly when exceptions are enabled as
       well. */
    e4__dict_entry(task, "badret2", 7, 0, e4t__test_execute_badret_nret,
            (void *)2);
    e4__dict_entry(task, "badret100", 9, 0, e4t__test_execute_badret_nret,
            (void *)100);

    e4t__ASSERT_EQ(e4__evaluate(task, "badret2", -1), e4__E_RSTKUNDERFLOW);
    e4t__ASSERT_EQ(e4__evaluate(task, "badret100", -1), e4__E_RSTKUNDERFLOW);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo badret2 ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "foo", -1), e4__E_RSTKUNDERFLOW);
    e4t__ASSERT_OK(e4__evaluate(task, ": bar badret100 ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "bar", -1), e4__E_RSTKUNDERFLOW);
}

static void e4t__test_execute_badret_nret(struct e4__task *task,
        e4__cell user)
{
    e4__usize i;

    for (i = 0; i < (e4__usize)e4__DEREF(user); ++i)
        e4__execute_ret(task);
}

static void e4t__test_execute_branch(void)
{
    static const void *branch_skip[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)1,
        &e4__BUILTIN_XT[e4__B_BRANCH],
        (void *)5,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)2,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)3,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)4,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *branch_conditional[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)1,
        &e4__BUILTIN_XT[e4__B_BRANCH0],
        (void *)5,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)0,
        &e4__BUILTIN_XT[e4__B_BRANCH0],
        (void *)3,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)10,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)20,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *branch_negative[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_BRANCH],
        (void *)6,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)1,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)2,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)3,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)4,
        &e4__BUILTIN_XT[e4__B_BRANCH],
        (void *)e4__USIZE_NEGATE(10),
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    struct e4__task *task = e4t__transient_task();

    e4__execute(task, branch_skip);
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);

    e4__stack_clear(task);
    e4__execute(task, branch_conditional);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 20);

    e4__stack_clear(task);
    e4__execute(task, branch_negative);
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);
}

static void e4t__test_execute_data(void)
{
    static const void *constant_77[] = {
        (void *)(e4__usize)e4__execute_value,
        NULL,
        (void *)77
    };
    static const void *variable_33[] = {
        (void *)(e4__usize)e4__execute_variable,
        NULL,
        (void *)33
    };
    static const void *uservar_base[] = {
        (void *)(e4__usize)e4__execute_uservar,
        (void *)e4__UV_BASE
    };
    static const void *userval_c[] = {
        (void *)(e4__usize)e4__execute_userval,
        (void *)((e4__usize)'c')
    };
    struct e4__task *task = e4t__transient_task();

    e4__execute(task, constant_77);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 77);

    e4__execute(task, variable_33);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_peek(task), &variable_33[2]);
    e4t__ASSERT_EQ(e4__DEREF(e4__stack_pop(task)), 33);

    e4__execute(task, uservar_base);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_peek(task), e4__task_uservar(task, e4__UV_BASE));
    e4t__ASSERT_EQ(e4__DEREF(e4__stack_pop(task)), 10);

    e4__execute(task, userval_c);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 'c');
}

static void e4t__test_execute_defer(void)
{
    static const void *yield_5[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)3,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)2,
        &e4__BUILTIN_XT[e4__B_PLUS],
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *yield_16[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)10,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)6,
        &e4__BUILTIN_XT[e4__B_PLUS],
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    void *defer_num[] = {
        (void *)(e4__usize)e4__execute_deferthunk,
        (void *)yield_5
    };
    struct e4__task *task = e4t__transient_task();

    e4__execute(task, defer_num);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    defer_num[1] = (void *)yield_16;

    e4__execute(task, defer_num);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 16);
}

static void e4t__test_execute_does(void)
{
    void *addtwo_const[] = {
        (void *)(e4__usize)e4__execute_doesthunk,
        NULL,
        (void *)10,
        (void *)&e4__BUILTIN_XT[e4__B_FETCH],
        (void *)&e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)2,
        (void *)&e4__BUILTIN_XT[e4__B_PLUS],
        (void *)&e4__BUILTIN_XT[e4__B_EXIT],
        (void *)&e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    struct e4__task *task = e4t__transient_task();

    addtwo_const[1] = &addtwo_const[3];
    e4__execute(task, addtwo_const);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 12);

    addtwo_const[2] = (void *)51;
    e4__execute(task, addtwo_const);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 53);
}

/* Covers ABORT and ABORTQ (exceptions disabled) LIT_CELL and nested
   execution. */
static void e4t__test_execute_nested(void)
{
    const char *msg = (const char *)0x1234;
    static const void *push_12345[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)0x12345,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *push_abcde[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)0xabcde,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *push_77777[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)0x77777,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *push_seq_exit[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        push_77777,
        push_abcde,
        &e4__BUILTIN_XT[e4__B_EXIT],
        push_12345,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *push_seq_abort[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        push_77777,
        push_abcde,
        &e4__BUILTIN_XT[e4__B_ABORT],
        push_12345,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *push_seq_abortq[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        push_77777,
        push_abcde,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        "Abort message.",
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)14,
        &e4__BUILTIN_XT[e4__B_ABORTQ],
        push_12345,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    struct e4__task *task = e4t__transient_task();

    e4t__term_obuf_consume();

    e4__execute(task, push_12345);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0x12345);
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    e4__execute(task, push_seq_exit);
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0xabcde);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0x77777);

    /* Test exception disabled ABORT behavior. */
    e4__execute(task, push_seq_abort);
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    /* Test exception disabled ABORTQ behavior. */
    e4__execute(task, push_seq_abortq);
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), " Abort message.\n");
    e4t__ASSERT_EQ(e4__task_last_abortq(task, &msg), 0);
    e4t__ASSERT_EQ(msg, NULL);
}

/* Covers string literals. */
static void e4t__test_execute_string(void)
{
    /* XXX: Parts of this test only work correctly on a 64 bit
       system. */

    static const char *hello = "'ello guv'nor";
    static const char *minimum = "\030rice and beans, at least";
    struct e4__task *task = e4t__transient_task();
    const void *push_hello5[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_STR],
        (void *)13,
        NULL, /* padding */
        NULL, /* padding */
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)5,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    const void *push_minimum7[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_CSTR],
        NULL, /* padding */
        NULL, /* padding */
        NULL, /* padding */
        NULL, /* padding */
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)7,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };

    /* Test that a regular string works as expected. */
    memcpy(&push_hello5[4], hello, 13);
    e4__execute(task, push_hello5);
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
    e4t__ASSERT_EQ(e4__stack_pop(task), 13);
    e4t__ASSERT(!e4__mem_strncasecmp((const char *)e4__stack_pop(task),
            hello, 13));

    /* Test that a counted string works as expected. */
    memcpy(&push_minimum7[3], minimum, 25);
    e4__execute(task, push_minimum7);
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 7);
    e4t__ASSERT(!memcmp(e4__stack_pop(task), minimum, 25));
}

static void e4t__test_execute_userfunc_setter(struct e4__task *task,
        e4__cell user)
{
    **((e4__usize **)user) = 207;
}

static void e4t__test_execute_threadedthunk(void)
{
    void *twoplusthree[] = {
        (void *)(e4__usize)e4__execute_threadedthunk,
        NULL,
        (void *)&e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)2,
        (void *)&e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)3,
        (void *)&e4__BUILTIN_XT[e4__B_PLUS],
        (void *)&e4__BUILTIN_XT[e4__B_EXIT],
        (void *)&e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    struct e4__task *task = e4t__transient_task();
    twoplusthree[1] = &twoplusthree[2];

    e4__execute(task, twoplusthree);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
}

static void e4t__test_execute_tr0coherent(void)
{
    struct e4__task *task = e4t__transient_task();
    static const void *corrupt_rstack_push[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_STR],
        (void *)13,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)5,
        &e4__BUILTIN_XT[e4__B_TO_R],
        &e4__BUILTIN_XT[e4__B_TO_R],
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *corrupt_rstack_pop[] = {
        (void *)(e4__usize)e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_R_FROM],
        &e4__BUILTIN_XT[e4__B_R_FROM],
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };

    /* Test that return stack depth limits are preserved across
       exceptions. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 1 >r throw 50 ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task,
            ": should-pass-helper 1 2 >r >r ['] foo catch r> r> 2drop drop ;",
            -1));
    e4t__ASSERT_OK(e4__evaluate(task,
            ": should-fail1-helper 1 2 >r >r ['] foo catch r> r> r> "
            "2drop 2drop ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task,
            ": should-fail2-helper 1 2 >r >r ['] foo catch r> 2drop ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task,
            ": should-pass 3 >r should-pass-helper r> drop ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task,
            ": should-fail1 3 >r should-fail1-helper abort ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task,
            ": should-fail2 3 >r should-fail2-helper abort ;", -1));

    e4t__ASSERT_OK(e4__evaluate(task, "should-pass", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "should-fail1", -1),
            e4__E_RSTKUNDERFLOW);
    e4t__ASSERT_EQ(e4__evaluate(task, "should-fail2", -1),
            e4__E_RSTKIMBALANCE);

    /* Test that tr0 is reset correctly even when exceptions are
       disabled. Manually overwriting a return address with a garbage
       value would still cause a failure, but if this works then at the
       very least there is a chance to recover. In practice, there is
       likely no reason why this would ever come up. */
    e4t__ASSERT_EQ(e4__stack_rdepth(task), 0);
    e4__stack_rpush(task, (void *)50);
    e4__execute(task, corrupt_rstack_push);
    e4t__ASSERT_EQ(e4__stack_rdepth(task), 1);
    e4t__ASSERT_EQ(e4__stack_rpop(task), 50);

    e4t__ASSERT_EQ(e4__stack_rdepth(task), 0);
    e4__stack_rpush(task, (void *)50);
    e4__execute(task, corrupt_rstack_pop);
    e4t__ASSERT_EQ(e4__stack_rdepth(task), 1);
    e4t__ASSERT_EQ(e4__stack_rpop(task), 50);
}

/* Covers user functions. */
static void e4t__test_execute_userfunc(void)
{
    void *set_var[] = {
        (void *)(e4__usize)e4t__test_execute_userfunc_setter,
        NULL
    };
    struct e4__task *task = e4t__transient_task();
    e4__usize foo = 0;
    e4__usize bar = 0;

    set_var[1] = &foo;
    e4__execute(task, set_var);
    e4t__ASSERT_EQ(foo, 207);

    set_var[1] = &bar;
    e4__execute(task, set_var);
    e4t__ASSERT_EQ(bar, 207);
}

void e4t__test_execute(void)
{
    e4t__test_execute_badret();
    e4t__test_execute_branch();
    e4t__test_execute_data();
    e4t__test_execute_defer();
    e4t__test_execute_does();
    e4t__test_execute_nested();
    e4t__test_execute_string();
    e4t__test_execute_threadedthunk();
    e4t__test_execute_tr0coherent();
    e4t__test_execute_userfunc();
}
