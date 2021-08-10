#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

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
        NULL,
        (void *)e4__UV_BASE
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
}

static void e4t__test_execute_defer(void)
{
    static const void *yield_5[] = {
        e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LITERAL],
        (void *)3,
        &e4__BUILTIN_XT[e4__B_LITERAL],
        (void *)2,
        &e4__BUILTIN_XT[e4__B_PLUS],
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *yield_16[] = {
        e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LITERAL],
        (void *)10,
        &e4__BUILTIN_XT[e4__B_LITERAL],
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
        (void *)&e4__BUILTIN_XT[e4__B_LITERAL],
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

/* Covers ABORT (exceptions disabled) LIT SKIP and nested execution. */
static void e4t__test_execute_meta(void)
{
    static const void *push_12345[] = {
        e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LITERAL],
        (void *)0x12345,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *push_abcde[] = {
        e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LITERAL],
        (void *)0xabcde,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *push_77777[] = {
        e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LITERAL],
        (void *)0x77777,
        &e4__BUILTIN_XT[e4__B_EXIT],
        &e4__BUILTIN_XT[e4__B_SENTINEL]
    };
    static const void *push_seq_exit[] = {
        e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_SKIP],
        push_12345,
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
        &e4__BUILTIN_XT[e4__B_SKIP],
        push_12345,
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
    e4t__test_execute_data();
    e4t__test_execute_defer();
    e4t__test_execute_does();
    e4t__test_execute_meta();
    e4t__test_execute_userfunc();
}
