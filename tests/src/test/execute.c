#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

/* Covers ABORT LIT SKIP and nested execution. */
static void e4t__test_execute_meta(void)
{
    static const void *push_12345[] = {
        e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT],
        (void *)0x12345,
        &e4__BUILTIN_XT[e4__B_RET]
    };
    static const void *push_abcde[] = {
        e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT],
        (void *)0xabcde,
        &e4__BUILTIN_XT[e4__B_RET]
    };
    static const void *push_77777[] = {
        e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_LIT],
        (void *)0x77777,
        &e4__BUILTIN_XT[e4__B_RET]
    };
    const void *push_seq[] = {
        e4__execute_threaded,
        NULL,
        &e4__BUILTIN_XT[e4__B_SKIP],
        push_12345,
        push_77777,
        push_abcde,
        &e4__BUILTIN_XT[e4__B_ABORT],
        push_12345,
        &e4__BUILTIN_XT[e4__B_RET]
    };

    struct e4__task *task = e4t__transient_task();

    e4__execute(task, push_12345);
    e4t__ASSERT_EQ((e4__usize)e4__stack_pop(task), 0x12345);
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    e4__execute(task, push_seq);
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ((e4__usize)e4__stack_pop(task), 0xabcde);
    e4t__ASSERT_EQ((e4__usize)e4__stack_pop(task), 0x77777);
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
    e4t__test_execute_meta();
    e4t__test_execute_userfunc();
}
