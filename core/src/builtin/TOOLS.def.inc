/* Tools word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static void e4__builtin_DOT_S(struct e4__task *task, e4__cell user)
{
    register e4__cell s = task->s0;
    register e4__usize n = e4__stack_depth(task);
    register e4__usize io_res;
    struct e4__double d;
    char *num;
    e4__usize len;

    d = e4__usize_todouble(n);

    e4__io_pno_start(task);
    if ((io_res = e4__io_pno_holds(task, "> ", 2)) ||
            (io_res = e4__io_pno_digits(task, &d, e4__F_SIGNED)) ||
            (io_res = e4__io_pno_hold(task, '<')) ||
            (io_res = e4__io_pno_finish(task, &num, &len)) ||
            (io_res = e4__io_type(task, num, len))) {
        e4__exception_throw(task, io_res);
        e4__execute_ret(task);
        return;
    }

    while (task->sp < s) {
        d = e4__usize_todouble((e4__usize)e4__DEREF(s--));

        e4__io_pno_start(task);
        if ((io_res = e4__io_pno_hold(task, ' ')) ||
                (io_res = e4__io_pno_digits(task, &d, e4__F_SIGNED)) ||
                (io_res = e4__io_pno_finish(task, &num, &len)) ||
                (io_res = e4__io_type(task, num, len))) {
            e4__exception_throw(task, io_res);
            e4__execute_ret(task);
            return;
        }
    }

    e4__execute_ret(task);
}

static void e4__builtin_DUMP(struct e4__task *task, e4__cell user)
{
    register e4__cell addr;
    register e4__usize len;
    register e4__usize io_res;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    len = (e4__usize)e4__stack_pop(task);
    addr = e4__stack_pop(task);

    if ((io_res = e4__io_dump(task, addr, len)))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

static const void * const e4__builtin_QUESTION[] = {
    &e4__BUILTIN_XT[e4__B_FETCH],
    &e4__BUILTIN_XT[e4__B_DOT],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL],
};

static void e4__builtin_SEE(struct e4__task *task, e4__cell user)
{
    register const char *word;
    register e4__usize len, io_res;

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    if ((io_res = e4__io_see(task, word, len)))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

static void e4__builtin_WORDS(struct e4__task *task, e4__cell user)
{
    struct e4__dict_header *e = task->dict;
    register e4__usize io_res = e4__E_OK;
    e4__usize written = 0;

    while (e) {
        written += e->nbytes + 1;
        if (written > 79) {
            if ((io_res = e4__io_type(task, "\n", 1)))
                break;
            written = e->nbytes + 1;
        }
        if ((io_res = e4__io_type(task, e->name, e->nbytes)))
            break;
        if ((io_res = e4__io_type(task, " ", 1)))
            break;
        e = e->link;
    }

    if (io_res)
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

/* vim: set ft=c: */
