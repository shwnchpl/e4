/* Facility Extensions word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static void e4__builtin_MS(struct e4__task *task, e4__cell user)
{
    register e4__usize io_res;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    if ((io_res = e4__io_ms(task, (e4__usize)e4__stack_pop(task))))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

static void e4__builtin_TIME_AND_DATE(struct e4__task *task, e4__cell user)
{
    register e4__usize io_res;
    e4__usize t = 0;
    struct e4__gmt gmt = {0};

    if ((io_res = e4__io_unixtime(task, &t))) {
        e4__exception_throw(task, io_res);
        e4__execute_ret(task);
        return;
    }

    e4__usize_togmt(t, &gmt);

    _e4__BUILTIN_EXPECT_AVAIL(task, 6);

    e4__stack_push(task, (e4__cell)(e4__usize)gmt.sec);
    e4__stack_push(task, (e4__cell)(e4__usize)gmt.min);
    e4__stack_push(task, (e4__cell)(e4__usize)gmt.hour);
    e4__stack_push(task, (e4__cell)(e4__usize)gmt.mday);
    e4__stack_push(task, (e4__cell)(e4__usize)gmt.mon);
    e4__stack_push(task, (e4__cell)gmt.year);

    e4__execute_ret(task);
}

/* vim: set ft=c: */
