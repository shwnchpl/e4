#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

#include <string.h>

static void e4t__test_execute_branch(void)
{
    static const void *branch_skip[] = {
        e4__execute_threaded,
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
        e4__execute_threaded,
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
        e4__execute_threaded,
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
        e4__execute_value,
        NULL,
        (void *)77
    };
    static const void *variable_33[] = {
        e4__execute_variable,
        NULL,
        (void *)33
    };
    static const void *uservar_base[] = {
        e4__execute_uservar,
        (void *)e4__UV_BASE
    };
    static const void *userval_c[] = {
        e4__execute_userval,
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
        e4__execute_threaded,
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
        e4__execute_threaded,
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
        e4__execute_deferthunk,
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
        e4__execute_doesthunk,
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

/* Covers ABORT (exceptions disabled) LIT_CELL and nested execution. */
static void e4t__test_execute_nested(void)
{
    static const void *push_12345[] = {
        e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)0x12345,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *push_abcde[] = {
        e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)0xabcde,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *push_77777[] = {
        e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT_CELL],
        (void *)0x77777,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *push_seq_exit[] = {
        e4__execute_threaded,
        NULL,
        push_77777,
        push_abcde,
        &e4__BUILTIN_XT[e4__B_EXIT],
        push_12345,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *push_seq_abort[] = {
        e4__execute_threaded,
        NULL,
        push_77777,
        push_abcde,
        &e4__BUILTIN_XT[e4__B_ABORT],
        push_12345,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    struct e4__task *task = e4t__transient_task();

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
        e4__execute_threaded,
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
        e4__execute_threaded,
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
        void *user)
{
    **((e4__usize **)user) = 207;
}

/* Covers user functions. */
static void e4t__test_execute_userfunc(void)
{
    void *set_var[] = {
        e4t__test_execute_userfunc_setter,
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
    e4t__test_execute_branch();
    e4t__test_execute_data();
    e4t__test_execute_defer();
    e4t__test_execute_does();
    e4t__test_execute_nested();
    e4t__test_execute_string();
    e4t__test_execute_userfunc();
}
