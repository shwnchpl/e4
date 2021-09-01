/* Core Extensions word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static void e4__builtin_AGAIN(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_REF(task, 1);

    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH]);
    e4__compile_cell(task, (e4__cell)(e4__stack_pop(task) - task->here));

    e4__execute_ret(task);
}

static const void *e4__builtin_BACKSLASH[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)((e4__usize)'\n'),
    &e4__BUILTIN_XT[e4__B_WORD],
    &e4__BUILTIN_XT[e4__B_DROP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void *e4__builtin_BUFFER_COLON[] = {
    &e4__BUILTIN_XT[e4__B_CREATE],
    &e4__BUILTIN_XT[e4__B_ALLOT],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_COLON_NONAME(struct e4__task *task, e4__cell user)
{
    register struct e4__execute_token *xt =
            (struct e4__execute_token *)task->here;
    register e4__usize unaligned;
    register e4__usize res;

    task->here = (e4__cell)((e4__u8 *)task->here + sizeof(*xt) -
            sizeof(xt->data));

    /* FIXME: This probably shouldn't be possible. Confirm that it
       isn't with some kind of compile time assert rather than doing
       this. */
    if ((unaligned = (e4__usize)task->here % sizeof(e4__cell)))
        task->here = (e4__cell)((e4__u8 *)task->here + sizeof(e4__cell)
                - unaligned);

    if ((res = e4__compile_start(task, xt, e4__COMP_NONAME)))
        e4__exception_throw(task, res);

    e4__execute_ret(task);
}

static void e4__builtin_DEFER(struct e4__task *task, e4__cell user)
{
    register const char *word;
    register e4__u8 len;

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    e4__dict_entry(task, word, len, 0, e4__execute_deferthunk,
            (e4__cell)&e4__BUILTIN_XT[e4__B_ABORT]);

    e4__execute_ret(task);
}

static void e4__builtin_DEFER_FETCH(struct e4__task *task, e4__cell user)
{
    struct e4__execute_token *xt;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    xt = (struct e4__execute_token *)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)xt->user);

    e4__execute_ret(task);
}

static void e4__builtin_DEFER_STORE(struct e4__task *task, e4__cell user)
{
    struct e4__execute_token *xt;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    xt = (struct e4__execute_token *)e4__stack_pop(task);
    xt->user = e4__stack_pop(task);

    e4__execute_ret(task);
}

static void e4__builtin_DOT_PAREN(struct e4__task *task, e4__cell user)
{
    const char *str;
    register e4__usize len;
    register e4__usize io_res;

    len = e4__io_parse(task, ')', 0, &str);
    if ((io_res = e4__io_type(task, str, len)))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

static const void *e4__builtin_ERASE[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)0,
    &e4__BUILTIN_XT[e4__B_FILL],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void *e4__builtin_HEX[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)16,
    &e4__BUILTIN_XT[e4__B_BASE],
    &e4__BUILTIN_XT[e4__B_STORE],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_MARKER(struct e4__task *task, e4__cell user)
{
    register const char *word;
    register e4__u8 len;

    /* XXX: The standard doesn't explicitly allow for interpret only
       words, however in this implementation there is no meaningful
       way to execute a marker from the context of a word defined after
       that marker (which would be any word to which the marker would be
       available in the first place) without pulling the rug out from
       under yourself. Technically things should be mostly okay so long
       as no further allocations are made in any word evaluated before
       returning into words/code that is still allocated, but this is
       by and large a footgun with no reasonable application.

       If you want code that doesn't crash, do *NOT* try to invoke
       markers either when compiling or from compiled words. */

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    e4__dict_entry(task, word, len, 0, e4__execute_marker, task->here);

    e4__execute_ret(task);
}

static const void *e4__builtin_NIP[] = {
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_DROP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void *e4__builtin_NOT_EQUALS[] = {
    &e4__BUILTIN_XT[e4__B_EQUALS],
    &e4__BUILTIN_XT[e4__B_INVERT],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_PICK(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    _e4__BUILTIN_EXPECT_DEPTH(task, (e4__usize)e4__stack_peek(task) + 2);
    e4__stack_push(task, e4__stack_pick(task, (e4__usize)e4__stack_pop(task)));
    e4__execute_ret(task);
}

static void e4__builtin_QUESTION_DO(struct e4__task *task, e4__cell user)
{
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_TWO_DUP]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_TWO_TO_R]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_EQUALS]);

    e4__stack_push(task, task->here);

    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_INVERT]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH0]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_SENTINEL]);

    e4__execute_ret(task);
}

static void e4__builtin_REFILL(struct e4__task *task, e4__cell user)
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

static void e4__builtin_ROLL(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    _e4__BUILTIN_EXPECT_DEPTH(task, (e4__usize)e4__stack_peek(task) + 2);
    e4__stack_roll(task);
    e4__execute_ret(task);
}

static void e4__builtin_S_BACKSLASH_QUOTE(struct e4__task *task, e4__cell user)
{
    const char *str;
    register e4__usize len;

    len = e4__io_parse(task, '"', e4__F_IGNORE_ESC, &str);

    if (e4__task_compiling(task)) {
        e4__compile_estr(task, str, len);
    }

    /* FIXME: Handle runtime semantics. */

    e4__execute_ret(task);
}

static void e4__builtin_TO(struct e4__task *task, e4__cell user)
{
    register const char *word;
    register e4__u8 len;
    register const struct e4__dict_header *header;

    /* XXX: This implementation would not produce sensible results if it
       were baked into a definition (IMMEDIATE or not) manually via ',
       ['], or any other mechanism. According to section 6.2.2295 of the
       standard, this is fine. */

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

static void e4__builtin_TUCK(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 2);
    e4__stack_tuck(task);
    e4__execute_ret(task);
}

static void e4__builtin_TWO_R_FETCH(struct e4__task *task, e4__cell user)
{
    register e4__cell l, r;

    _e4__BUILTIN_EXPECT_RDEPTH(task, 3);

    /* The top of the return stack is the place to which *we* must
       return, so consume that value before doing anything else. */
    e4__execute_ret(task);

    r = e4__stack_rpop(task);
    l = e4__stack_rpop(task);

    e4__stack_push(task, l);
    e4__stack_push(task, r);

    e4__stack_rpush(task, l);
    e4__stack_rpush(task, r);

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_TWO_R_FROM(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_RDEPTH(task, 3);

    /* The top of the return stack is the place to which *we* must
       return, so consume that value before doing anything else. */
    e4__execute_ret(task);

    e4__stack_push(task, e4__stack_rpop(task));
    e4__stack_push(task, e4__stack_rpop(task));
    e4__stack_swap(task);

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_TWO_TO_R(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    /* The top of the return stack is the place to which *we* must
       return, so consume that value before doing anything else. */
    e4__execute_ret(task);

    e4__stack_swap(task);
    e4__stack_rpush(task, e4__stack_pop(task));
    e4__stack_rpush(task, e4__stack_pop(task));

    /* e4__execute_ret intentionally omitted. */
}

static const void *e4__builtin_U_GREATER_THAN[] = {
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_U_LESS_THAN],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_UNUSED(struct e4__task *task, e4__cell user)
{
    e4__stack_push(task, (e4__cell)e4__task_unused(task));
    e4__execute_ret(task);
}

static void e4__builtin_VALUE(struct e4__task *task, e4__cell user)
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
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)0,
    &e4__BUILTIN_XT[e4__B_GREATER_THAN],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void *e4__builtin_ZERO_NOT_EQUALS[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)0,
    &e4__BUILTIN_XT[e4__B_NOT_EQUALS],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

/* vim: set ft=c: */
