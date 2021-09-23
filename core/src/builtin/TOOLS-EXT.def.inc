/* Tools Extensions word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static void e4__builtin_BRACKET_ELSE(struct e4__task *task, e4__cell user)
{
    register e4__usize io_res;
    register e4__usize depth = 1;

    while (depth) {
        register char *word;
        register e4__u8 len;

        if (task->io_src.in >= task->io_src.length) {
            if ((io_res = e4__io_refill(task, NULL))) {
                e4__exception_throw(task, io_res);
                return;
            }
        }

        word = e4__io_word(task, ' ');
        len = (e4__u8)*word++;

        if (!len)
            continue;

        if (len == 4 && !e4__mem_strncasecmp(word, "[if]", 4))
            ++depth;
        else if (len == 6 && !e4__mem_strncasecmp(word, "[then]", 6))
            --depth;
    }

    e4__execute_ret(task);
}

static void e4__builtin_BRACKET_IF(struct e4__task *task, e4__cell user)
{
    register e4__usize f, io_res;
    register e4__usize depth = 1;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    f = (e4__usize)e4__stack_pop(task);
    if (!f) {
        while (depth) {
            register char *word;
            register e4__u8 len;

            if (task->io_src.in >= task->io_src.length) {
                if ((io_res = e4__io_refill(task, NULL))) {
                    e4__exception_throw(task, io_res);
                    return;
                }
            }

            word = e4__io_word(task, ' ');
            len = (e4__u8)*word++;

            if (!len)
                continue;

            if (len == 4 && !e4__mem_strncasecmp(word, "[if]", 4))
                ++depth;
            else if (len == 6 && !e4__mem_strncasecmp(word, "[else]", 6))
                depth -= (depth == 1);
            else if (len == 6 && !e4__mem_strncasecmp(word, "[then]", 6))
                --depth;
        }
    }

    e4__execute_ret(task);
}

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
