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
        if (io_res == e4__E_FILEIO) {
            e4__stack_push(task, (e4__cell)e4__task_ior(task, 0));
        } else {
            e4__exception_throw(task, io_res);
        }
    } else
        e4__stack_push(task, (e4__cell)0);

    e4__execute_ret(task);
}

static const void * const e4__builtin_INCLUDED[] = {
    &e4__BUILTIN_XT[e4__B_R_SLASH_O],
    &e4__BUILTIN_XT[e4__B_OPEN_FILE],
    &e4__BUILTIN_XT[e4__B_BRANCH0],
    (void *)7,
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)e4__E_OPEN_FILE,
    &e4__BUILTIN_XT[e4__B_THROW],
    &e4__BUILTIN_XT[e4__B_DROP],
    &e4__BUILTIN_XT[e4__B_BRANCH],
    (void *)1,
    &e4__BUILTIN_XT[e4__B_INCLUDE_FILE],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_INCLUDE_FILE(struct e4__task *task, e4__cell user)
{
    register e4__usize fd, res;
    struct e4__file_exception fex;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    fd = (e4__usize)e4__stack_pop(task);
    if ((res = e4__evaluate_file(task, fd, &fex))) {
        if (res == e4__E_FILEIO) {
            /* Eat the system specific IO error; there is no meaningful
               way to let it percolate up at this point. */
            e4__task_ior(task, 0);
            e4__exception_throw(task, e4__E_CLOSE_FILE);
        } else
            e4__exception_throw(task, res);
    } else if (fex.ex) {
        if (fex.ex != e4__E_BYE) {
            /* FIXME: Print exception info? */
        }
        e4__exception_throw(task, fex.ex);
    }

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
        if (io_res == e4__E_FILEIO) {
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
