/* Tools word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static void e4__builtin_DOT_S(struct e4__task *task, void *user)
{
    register e4__cell s = task->s0;
    register e4__usize n = e4__stack_depth(task);
    register e4__usize io_res;
    register char *num;
    register char *buf = (char *)task->here;
    register e4__usize len;

    /* FIXME: Once pictured numeric output has been implemented, use
       that instead of these hacks, which may not quite be safe. */

    num = e4__num_format(n, task->base, e4__F_SIGNED, &buf[1], 130);
    len = &buf[131] - num;
    len += 3;
    *--num = '<';
    num[len - 2] = '>';
    num[len - 1] = ' ';

    if ((io_res = e4__io_type(task, num, len))) {
        e4__exception_throw(task, io_res);
        e4__execute_ret(task);
        return;
    }

    while (task->sp < s) {
        n = (e4__usize)e4__DEREF(s--);
        num = e4__num_format(n, task->base, e4__F_SIGNED, buf, 130);
        len = &buf[130] - num;
        num[len++] = ' ';

        if ((io_res = e4__io_type(task, num, len))) {
            e4__exception_throw(task, io_res);
            e4__execute_ret(task);
            return;
        }
    }

    e4__execute_ret(task);
}

static const void *e4__builtin_QUESTION[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_FETCH],
    &e4__BUILTIN_XT[e4__B_DOT],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL],
};

static void e4__builtin_WORDS(struct e4__task *task, void *user)
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
