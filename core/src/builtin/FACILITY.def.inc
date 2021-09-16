/* Facility word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static const void *e4__builtin_AT_XY[] = {
    &e4__BUILTIN_XT[e4__B_S_TO_D],
    &e4__BUILTIN_XT[e4__B_ROT],
    &e4__BUILTIN_XT[e4__B_S_TO_D],
    &e4__BUILTIN_XT[e4__B_LESS_NUMBER_SIGN],
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)(e4__usize)'H',
    &e4__BUILTIN_XT[e4__B_HOLD],
    &e4__BUILTIN_XT[e4__B_NUMBER_SIGN_S],
    &e4__BUILTIN_XT[e4__B_TWO_DROP],
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)(e4__usize)';',
    &e4__BUILTIN_XT[e4__B_HOLD],
    &e4__BUILTIN_XT[e4__B_NUMBER_SIGN_S],
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)(e4__usize)'[',
    &e4__BUILTIN_XT[e4__B_HOLD],
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)27,
    &e4__BUILTIN_XT[e4__B_HOLD],
    &e4__BUILTIN_XT[e4__B_NUMBER_SIGN_GREATER],
    &e4__BUILTIN_XT[e4__B_TYPE],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_KEY_QUESTION(struct e4__task *task, e4__cell user)
{
    register e4__usize io_res;
    e4__usize bflag;

    if ((io_res = e4__io_keyq(task, &bflag)))
        e4__exception_throw(task, io_res);
    else
        e4__stack_push(task, (e4__cell)bflag);

    e4__execute_ret(task);
}

static const void *e4__builtin_PAGE[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)"\033[2J\033[0;0H",
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)10,
    &e4__BUILTIN_XT[e4__B_TYPE],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

/* vim: set ft=c: */
