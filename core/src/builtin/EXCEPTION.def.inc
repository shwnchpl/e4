/* Exception word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static void e4__builtin_CATCH(struct e4__task *task, void *user)
{
    register e4__usize ex;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    ex = e4__exception_catch(task, e4__stack_pop(task));
    e4__stack_push(task, (e4__cell)ex);

    e4__execute_ret(task);
}

static void e4__builtin_THROW(struct e4__task *task, void *user)
{
    register e4__usize ex;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    ex = (e4__usize)e4__stack_pop(task);
    e4__exception_throw(task, ex);

    e4__execute_ret(task);
}

/* vim: set ft=c: */
