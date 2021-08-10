/* Core Extensions word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static void e4__builtin_COLON_NONAME(struct e4__task *task, void *user)
{
    register struct e4__execute_token *xt =
            (struct e4__execute_token *)task->here;
    register e4__usize unaligned;

    task->here = (e4__cell)((e4__u8 *)task->here + sizeof(*xt) -
            sizeof(xt->data));

    /* FIXME: This probably shouldn't be possible. Confirm that it
       isn't with some kind of compile time assert rather than doing
       this. */
    if ((unaligned = (e4__usize)task->here % sizeof(e4__cell)))
        task->here = (e4__cell)((e4__u8 *)task->here + sizeof(e4__cell)
                - unaligned);

    e4__compile_start(task, xt, e4__COMP_NONAME);

    e4__execute_ret(task);
}

static void e4__builtin_DEFER(struct e4__task *task, void *user)
{
    register const char *word;
    register e4__u8 len;

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    e4__dict_entry(task, word, len, 0, e4__execute_deferthunk,
            (e4__cell)&e4__BUILTIN_XT[e4__B_ABORT]);

    e4__execute_ret(task);
}

static void e4__builtin_DEFER_FETCH(struct e4__task *task, void *user)
{
    struct e4__execute_token *xt;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    xt = (struct e4__execute_token *)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)xt->user);

    e4__execute_ret(task);
}

static void e4__builtin_DEFER_STORE(struct e4__task *task, void *user)
{
    struct e4__execute_token *xt;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    xt = (struct e4__execute_token *)e4__stack_pop(task);
    xt->user = e4__stack_pop(task);

    e4__execute_ret(task);
}

static void e4__builtin_REFILL(struct e4__task *task, void *user)
{
    register e4__usize io_res;

    /* Unless SOURCE-ID is user input device, just return false. */
    if (task->io_src.sid) {
        e4__stack_push(task, (e4__cell)e4__BF_FALSE);
        return;
    }

    /* Attempt to read the entire buffer. */
    task->io_src.length = task->io_src.sz;
    io_res = e4__io_accept(task, (char *)task->io_src.buffer,
            &task->io_src.length);
    if (io_res) {
        /* We've failed. Say the buffer is empty. */
        task->io_src.length = 0;
        e4__exception_throw(task, io_res);

        /* If we happen to be here, exceptions aren't enabled, so we
           we may as well return false. */
        e4__stack_push(task, (e4__cell)e4__BF_FALSE);
        e4__execute_ret(task);
        return;
    }

    e4__stack_push(task, (e4__cell)e4__BF_TRUE);

    e4__execute_ret(task);
}

static void e4__builtin_ROLL(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    _e4__BUILTIN_EXPECT_DEPTH(task, (e4__usize)e4__stack_peek(task) + 1);
    e4__stack_roll(task);
    e4__execute_ret(task);
}

static void e4__builtin_TO(struct e4__task *task, void *user)
{
    register const char *word;
    register e4__u8 len;
    register struct e4__dict_header *header;

    if (!e4__task_compiling(task))
        _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    if (!(header = e4__dict_lookup(task, word, len))) {
        e4__exception_throw(task, e4__E_UNDEFWORD);
        e4__execute_ret(task);
    } else if ((header->flags & e4__F_CONSTANT) ||
            header->xt->code != e4__execute_value) {
        e4__exception_throw(task, e4__E_INVNAMEARG);
        e4__execute_ret(task);
    }

    if (e4__task_compiling(task)) {
        e4__compile_literal(task, (e4__cell)&header->xt->data[0]);
        e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_STORE]);
    } else
        header->xt->data[0] = e4__stack_pop(task);

    e4__execute_ret(task);
}

static void e4__builtin_TUCK(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 2);
    e4__stack_tuck(task);
    e4__execute_ret(task);
}

static void e4__builtin_VALUE(struct e4__task *task, void *user)
{
    register const char *word;
    register e4__u8 len;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    e4__dict_entry(task, word, len, 0, e4__execute_value, NULL);
    e4__builtin_exec(task, e4__B_COMMA);

    e4__execute_ret(task);
}

/* vim: set ft=c: */
