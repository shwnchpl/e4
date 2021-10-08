/* System word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static void e4__builtin_ABORTQ(struct e4__task *task, e4__cell user)
{
    register int i;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    /* Set abortq fields as appropriate. */
    task->abortq_msg_len = (e4__usize)e4__stack_pop(task);
    task->abortq_msg = (const char *)e4__stack_pop(task);

    /* Try to just throw an exception. */
    e4__exception_throw(task, e4__E_ABORTQ);

    /* Okay, exceptions aren't enabled, meaning we definitely aren't
       running within the context of quit. Attempt to display the
       message and manually perform the core ABORT semantics outlined in
       6.1.0670 by manually clearing the data stack and filling the
       return stack with immediate return thunks. */
    /* XXX: While these calls may fail, we already know that we can't
       raise an exception if they do, so simply ignore any errors. */
    e4__io_type(task, " ", 1);
    e4__io_type(task, task->abortq_msg, task->abortq_msg_len);
    e4__io_type(task, "\n", 1);

    task->abortq_msg = NULL;
    task->abortq_msg_len = 0;

    e4__stack_clear(task);

    for (i = 1; &task->rp[i] <= task->r0; ++i)
        task->rp[i] = _e4__BUILTIN_RETURN_THUNK;

    e4__execute_ret(task);
}

static void e4__builtin_BRANCH(struct e4__task *task, e4__cell user)
{
    register e4__usize offset;

    /* Since we're manipulating rp directly rather than calling
       e4__execute_ret, ensure that we aren't going to underflow. */
    _e4__BUILTIN_EXPECT_RDEPTH(task, 1);

    offset = (e4__usize)e4__DEREF2(++task->rp);
    task->ip = e4__DEREF(task->rp) + offset;

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_BRANCH0(struct e4__task *task, e4__cell user)
{
    register e4__usize offset;

    /* Since we're manipulating rp directly rather than calling
       e4__execute_ret, ensure that we aren't going to underflow. */
    _e4__BUILTIN_EXPECT_RDEPTH(task, 1);
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    offset = (e4__usize)e4__DEREF2(++task->rp);
    task->ip = e4__DEREF(task->rp) + (!e4__stack_pop(task) ? offset : 1);

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_CLEAR(struct e4__task *task, e4__cell user)
{
    e4__stack_clear(task);
    e4__execute_ret(task);
}

static void e4__builtin_LIT_CELL(struct e4__task *task, e4__cell user)
{
    /* Since we're manipulating rp directly rather than calling
       e4__execute_ret, ensure that we aren't going to underflow. */
    _e4__BUILTIN_EXPECT_RDEPTH(task, 1);

    e4__stack_push(task, e4__DEREF2(++task->rp));
    task->ip = e4__DEREF(task->rp) + 1;

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_LIT_CSTR(struct e4__task *task, e4__cell user)
{
    register e4__usize len;
    register e4__cell str;

    /* Since we're manipulating rp directly rather than calling
       e4__execute_ret, ensure that we aren't going to underflow. */
    _e4__BUILTIN_EXPECT_RDEPTH(task, 1);

    str = e4__DEREF(++task->rp);
    len = (e4__usize)*((char *)str) + 1;

    /* Advance ip beyond the count and the string. This is similar
       to BRANCH, but the length of the string is in characters rather
       than cells so we need to account for that and align. */
    task->ip = str + (len / sizeof(e4__cell));
    if (len % sizeof(e4__cell))
        ++task->ip;

    /* With that out of the way, push the address of the beginning
       of the string and the length of the string onto the stack. */
    e4__stack_push(task, str);

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_LIT_DOES(struct e4__task *task, e4__cell user)
{
    if (task->dict->flags & e4__F_BUILTIN) {
        e4__exception_throw(task, e4__E_INVBUILTINMUT);
        e4__execute_ret(task);
        return;
    }

    if (e4__task_compiling(task)) {
        /* Looks like we're compiling. Bail out. */
        e4__exception_throw(task, e4__E_NESTEDCOMPILE);
        e4__execute_ret(task);
    }

    /* Return from this function. */
    e4__execute_ret(task);

    /* Make the execution semantics of the most recent definition
       the threaded code following LIT_DOES. */
    task->dict->xt->code = e4__execute_doesthunk;
    task->dict->xt->user = task->ip;

    /* Set the instruction pointer to a return thunk so that we can
       return as we would have at the end of threaded_execute of the
       word containing LIT_DOES. */
    task->ip = (e4__cell)_e4__BUILTIN_RETURN_THUNK;

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_LIT_STR(struct e4__task *task, e4__cell user)
{
    register e4__usize len;
    register e4__cell str;

    /* Since we're manipulating rp directly rather than calling
       e4__execute_ret, ensure that we aren't going to underflow. */
    _e4__BUILTIN_EXPECT_RDEPTH(task, 1);

    len = (e4__usize)e4__DEREF2(++task->rp);
    str = e4__DEREF(task->rp) + 1;

    /* Advance ip beyond the string. This is similar to BRANCH, but
       the length of the string is in characters rather than cells
       so we need to account for that and align. */
    task->ip = str + (len / sizeof(e4__cell));
    if (len % sizeof(e4__cell))
        ++task->ip;

    /* With that out of the way, push the address of the beginning
       of the string and the length of the string onto the stack. */
    e4__stack_push(task, str);
    e4__stack_push(task, (e4__cell)len);

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_RESTRICT(struct e4__task *task, e4__cell user)
{
    if (task->dict->flags & e4__F_BUILTIN)
        e4__exception_throw(task, e4__E_INVBUILTINMUT);
    else
        task->dict->flags |= e4__F_COMPONLY;

    e4__execute_ret(task);
}

static void e4__builtin_SENTINEL(struct e4__task *task, e4__cell user)
{
    /* XXX: This function's sole purpose is to act as a unique
       identifier and noop. The standard guarantees that this is okay,
       as any compiler optimization that would cause its address to
       compare equally to the address of some other function would be
       a violation of the as-if clause. */
    e4__execute_ret(task);
}

/* vim: set ft=c: */
