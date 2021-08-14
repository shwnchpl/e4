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

static const void *e4__builtin_NOT_EQUALS[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_EQUALS],
    &e4__BUILTIN_XT[e4__B_INVERT],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_REFILL(struct e4__task *task, void *user)
{
    register e4__usize io_res;
    e4__usize bf;

    if ((io_res = e4__io_refill(task, &bf)))
        e4__exception_throw(task, io_res);

    /* If we happen to be here either all is well or exceptions aren't
        enabled. In either case, we may as well push the boolean flag
        result and return. */
    e4__stack_push(task, (e4__cell)bf);

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

static const void *e4__builtin_U_GREATER_THAN[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_U_LESS_THAN],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

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

static const void *e4__builtin_ZERO_GREATER[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)0,
    &e4__BUILTIN_XT[e4__B_GREATER_THAN],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void *e4__builtin_ZERO_NOT_EQUALS[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)0,
    &e4__BUILTIN_XT[e4__B_NOT_EQUALS],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

/* vim: set ft=c: */
