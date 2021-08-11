/* System word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static void e4__builtin_BRANCH(struct e4__task *task, void *user)
{
    const e4__usize offset = (e4__usize)e4__DEREF2(++task->rp);

    task->ip = e4__DEREF(task->rp) + offset;

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_BRANCH0(struct e4__task *task, void *user)
{
    e4__usize offset;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    offset = (e4__usize)e4__DEREF2(++task->rp);
    task->ip = e4__DEREF(task->rp) + (!e4__stack_pop(task) ? offset : 1);

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_CLEAR(struct e4__task *task, void *user)
{
    e4__stack_clear(task);
    e4__execute_ret(task);
}

static void e4__builtin_DLITERAL(struct e4__task *task, void *user)
{
    e4__usize res;

    if (task->dict->flags & e4__F_BUILTIN) {
        e4__exception_throw(task, e4__E_INVBUILTINMUT);
        e4__execute_ret(task);
        return;
    }

    e4__compile_start(task, task->dict->xt, e4__COMP_DOES);

    /* Return from this function. */
    e4__execute_ret(task);

    /* Advance the instruction pointer until SENTINEL has been
       encountered, compiling each word along the way. */
    do {
        e4__compile_cell(task, e4__DEREF(task->ip++));
    } while (e4__DEREF(task->ip) != (e4__cell)&e4__BUILTIN_XT[e4__B_SENTINEL]);

    if ((res = e4__compile_finish(task)))
        /* We haven't touched the stack. This should absolutely never
           happen, but if it does, pass the exception along. */
        e4__exception_throw(task, res);

    /* Set the instruction pointer to a return thunk so that we can
       return as we would have at the end of threaded_execute of the
       word containing DLITERAL. */
    task->ip = (e4__cell)_e4__BUILTIN_RETURN_THUNK;

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_SENTINEL(struct e4__task *task, void *user)
{
    /* XXX: This function's sole purpose is to act as a unique
       identifier. The standard guarantees that this is okay, as any
       compiler optimization that would cause its address to compare
       equally to the address of some other function would be
       a violation of the as-if clause. */
    e4__execute_ret(task);
}

/* vim: set ft=c: */
