/* File word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static const void * const e4__builtin_BIN[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)e4__F_BIN,
    &e4__BUILTIN_XT[e4__B_OR],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_CLOSE_FILE(struct e4__task *task, e4__cell user)
{
    register e4__usize fd, io_res;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    fd = (e4__usize)e4__stack_pop(task);

    if ((io_res = e4__io_file_close(task, fd))) {
        if (e4__E_HAS_PLATFORM_IOR(io_res)) {
            e4__stack_push(task, (e4__cell)e4__task_ior(task, 0));
        } else {
            e4__exception_throw(task, io_res);
        }
    } else
        e4__stack_push(task, (e4__cell)0);

    e4__execute_ret(task);
}

static void e4__builtin_INCLUDED(struct e4__task *task, e4__cell user)
{
    register e4__usize res, u;
    register const char *addr;

    /* XXX: It would be nice to implement this as a thunk, but
       unfortunately we need to throw an exception and THROW is not
       necessarily available if this wordlist is being included but
       the exception wordlist is not. */

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    u = (e4__usize)e4__stack_pop(task);
    addr = (const char *)e4__stack_pop(task);

    if ((res = e4__evaluate_path(task, addr, u)))
        e4__exception_throw(task, res);
    else if (task->fex.ex)
        e4__exception_throw(task, task->fex.ex);

    e4__execute_ret(task);
}

static void e4__builtin_INCLUDE_FILE(struct e4__task *task, e4__cell user)
{
    register e4__usize fd, res;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    fd = (e4__usize)e4__stack_pop(task);
    if ((res = e4__evaluate_file(task, fd)))
        e4__exception_throw(task, res);
    else if (task->fex.ex)
        e4__exception_throw(task, task->fex.ex);

    e4__execute_ret(task);
}

static void e4__builtin_OPEN_FILE(struct e4__task *task, e4__cell user)
{
    register e4__usize io_res, u, fam;
    register const char *addr;
    e4__usize fd;

    _e4__BUILTIN_EXPECT_DEPTH(task, 3);

    fam = (e4__usize)e4__stack_pop(task);
    u = (e4__usize)e4__stack_pop(task);
    addr = (const char *)e4__stack_pop(task);

    if ((io_res = e4__io_file_open(task, addr, u, fam, &fd))) {
        if (e4__E_HAS_PLATFORM_IOR(io_res)) {
            e4__stack_push(task, (e4__cell)0);
            e4__stack_push(task, (e4__cell)e4__task_ior(task, 0));
        } else {
            e4__exception_throw(task, io_res);
        }
    } else {
        e4__stack_push(task, (e4__cell)fd);
        e4__stack_push(task, (e4__cell)0);
    }

    e4__execute_ret(task);
}

/* vim: set ft=c: */
