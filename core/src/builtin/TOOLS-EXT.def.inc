/* Tools Extensions word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static void e4__builtin_BYE(struct e4__task *task, e4__cell user)
{
    e4__exception_throw(task, e4__E_BYE);

    /* If exceptions aren't enabled, just return. */
    e4__execute_ret(task);
}

static void e4__builtin_FORGET(struct e4__task *task, e4__cell user)
{
    register const char *word;
    register e4__u8 len;
    register e4__usize res;

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    if ((res = e4__dict_forget(task, word, len)))
        e4__exception_throw(task, res);

    e4__execute_ret(task);
}

/* vim: set ft=c: */
