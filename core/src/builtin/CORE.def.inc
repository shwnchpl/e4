/* Core word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static void e4__builtin_ABORT(struct e4__task *task, void *user)
{
    register int i;

    /* First, try to just throw an abort exception. */
    e4__exception_throw(task, e4__E_ABORT);

    /* Okay, exceptions aren't enabled, meaning we definitely aren't
       running within the context of quit. Attempt to perform the
       core ABORT semantics outlined in 6.1.0670 by manually clearing
       the data stack and filling the return stack with immediate
       return thunks. */

    e4__stack_clear(task);

    for (i = 1; &task->rp[i] <= task->r0; ++i)
        task->rp[i] = _e4__BUILTIN_RETURN_THUNK;

    e4__execute_ret(task);
}

static const void *e4__builtin_ALIGN[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_HERE],
    &e4__BUILTIN_XT[e4__B_DUP],
    &e4__BUILTIN_XT[e4__B_ALIGNED],
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_MINUS],
    &e4__BUILTIN_XT[e4__B_ALLOT],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_ALIGNED(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    e4__stack_push(task,
            (e4__cell)e4__mem_aligned((e4__usize)e4__stack_pop(task)));

    e4__execute_ret(task);
}

static void e4__builtin_ALLOT(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    task->here = (e4__cell)((e4__u8 *)task->here +
            (e4__usize)e4__stack_pop(task));
    e4__execute_ret(task);
}

static void e4__builtin_AND(struct e4__task *task, void *user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    e4__stack_push(task, (e4__cell)(l & r));

    e4__execute_ret(task);
}

static void e4__builtin_BRACKET_TICK(struct e4__task *task, void *user)
{
    const register e4__usize depth = e4__stack_depth(task);

    /* Call interpretation time tick. */
    e4__builtin_exec(task, e4__B_TICK);

    if (e4__stack_depth(task) != depth + 1) {
        /* TICK failed and exceptions are disabled. Nothing we can do
           here but just return. */
        e4__execute_ret(task);
        return;
    }

    /* Compile xt from the top of the stack into the current
       function. */
    e4__compile_literal(task, e4__stack_pop(task));

    e4__execute_ret(task);
}

static void e4__builtin_C_COMMA(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    e4__compile_char(task, e4__stack_pop(task));
    e4__execute_ret(task);
}

static void e4__builtin_C_FETCH(struct e4__task *task, void *user)
{
    register e4__u8 *c;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    c = (e4__u8 *)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)((e4__usize)*c));

    e4__execute_ret(task);
}

static void e4__builtin_C_STORE(struct e4__task *task, void *user)
{
    register e4__u8 *c, v;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    c = (e4__u8 *)e4__stack_pop(task);
    v = (e4__u8)((e4__usize)e4__stack_pop(task) & 0xff);
    *c = v;

    e4__execute_ret(task);
}

static const void *e4__builtin_CELLS[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)sizeof(e4__cell),
    &e4__BUILTIN_XT[e4__B_STAR],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void *e4__builtin_CELL_PLUS[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)sizeof(e4__cell),
    &e4__BUILTIN_XT[e4__B_PLUS],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void *e4__builtin_CHARS[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)sizeof(e4__u8),
    &e4__BUILTIN_XT[e4__B_STAR],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void *e4__builtin_CHAR_PLUS[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)sizeof(e4__u8),
    &e4__BUILTIN_XT[e4__B_PLUS],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_COLON(struct e4__task *task, void *user)
{
    register const char *word;
    register e4__u8 len;

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    e4__dict_entry(task, word, len, e4__F_COMPILING, NULL, NULL);
    e4__compile_start(task, task->dict->xt, e4__COMP_COLON);

    e4__execute_ret(task);
}

static void e4__builtin_COMMA(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    e4__compile_cell(task, e4__stack_pop(task));
    e4__execute_ret(task);
}

static void e4__builtin_CONSTANT(struct e4__task *task, void *user)
{
    register const char *word;
    register e4__u8 len;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    e4__dict_entry(task, word, len, e4__F_CONSTANT, e4__execute_value, NULL);
    e4__builtin_exec(task, e4__B_COMMA);

    e4__execute_ret(task);
}

static void e4__builtin_CR(struct e4__task *task, void *user)
{
    e4__usize io_res;

    /* FIXME: Should this actually output \r\n? */
    if ((io_res = e4__io_type(task, "\n", 1)))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

static void e4__builtin_CREATE(struct e4__task *task, void *user)
{
    register const char *word;
    register e4__u8 len;

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    e4__dict_entry(task, word, len, 0, e4__execute_variable, NULL);
    e4__execute_ret(task);
}

static const void *e4__builtin_DECIMAL[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)10,
    &e4__BUILTIN_XT[e4__B_BASE],
    &e4__BUILTIN_XT[e4__B_STORE],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_DEPTH(struct e4__task *task, void *user)
{
    e4__stack_push(task, (e4__cell)e4__stack_depth(task));
    e4__execute_ret(task);
}

static void e4__builtin_DO(struct e4__task *task, void *user)
{
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_SWAP]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_TO_R]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_TO_R]);
    e4__compile_literal(task, (e4__cell)0);

    e4__stack_push(task, task->here);

    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_INVERT]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH0]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_SENTINEL]);

    e4__execute_ret(task);
}

static void e4__builtin_DOES(struct e4__task *task, void *user)
{
    /* XXX: Section 6.1.1250 of the standard specifies that DOES> has
       undefined interpretation semantics but specifies that its
       compilation semantics are simply to copy its run-time semantics
       into the definition being compiled. This means that it should
       be okay to comply with the standard by compiling in other words
       so long as those word achieve the correct semantics. Which means
       it should also be okay to allow DOES> to have interpretation
       semantics that match those compiled run-time semantics, even if
       these interpretation semantics are achieved via a different
       mechanism from the compiled run-time semantics. */
    if (e4__task_compiling(task))
        e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_DLITERAL]);
    else if (task->dict->flags & e4__F_BUILTIN)
        /* Invalid attempt to mutate a builtin. */
        e4__exception_throw(task, e4__E_INVBUILTINMUT);
    else
        e4__compile_start(task, task->dict->xt, e4__COMP_DOES);

    e4__execute_ret(task);
}

static void e4__builtin_DOT(struct e4__task *task, void *user)
{
    /* FIXME: Once pictured numeric output has been implemented, use
       that instead of this implementation. */
    register e4__usize io_res;
    register char *num;
    register char *buf;
    register e4__usize len;
    register e4__usize n;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    buf = (char *)task->here;
    n = (e4__usize)e4__stack_pop(task);

    num = e4__num_format(n, task->base, e4__F_SIGNED, buf, 130);
    len = &buf[130] - num;
    num[len++] = ' ';

    if ((io_res = e4__io_type(task, num, len)))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

static void e4__builtin_DROP(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    e4__stack_drop(task);
    e4__execute_ret(task);
}

static void e4__builtin_DUP(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    e4__stack_dup(task);
    e4__execute_ret(task);
}

static void e4__builtin_ELSE(struct e4__task *task, void *user)
{
    register e4__cell ref;

    _e4__BUILTIN_EXPECT_REF(task, 1);

    /* Branch unconditionally to new forward ref and point old forward
       ref to the instruction immediately following (thereby skipping
       the unconditional branch). */
    ref = e4__stack_pop(task);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH]);
    e4__stack_push(task, task->here++);
    e4__DEREF(ref) = (e4__cell)(task->here - ref);

    e4__execute_ret(task);
}

static void e4__builtin_EMIT(struct e4__task *task, void *user)
{
    char c;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    c = (char)(e4__usize)e4__stack_pop(task);
    e4__io_type(task, &c, 1);

    e4__execute_ret(task);
}

static void e4__builtin_EQUALS(struct e4__task *task, void *user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    e4__stack_push(task, (e4__cell)(r == l ? e4__BF_TRUE : e4__BF_FALSE));

    e4__execute_ret(task);
}

static void e4__builtin_EXECUTE(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    e4__execute(task, e4__stack_pop(task));
    e4__execute_ret(task);
}

static void e4__builtin_EXIT(struct e4__task *task, void *user)
{
    /* XXX: This function doesn't actually do anything. It's address
       is simply understood by the threaded interpreter to indicate
       that a return should occur. The standard guarantees that this
       is okay, as any compiler optimization that would cause its
       address to compare equally to the address of some other function
       would be a violation of the as-if clause.

       Since this function is compile-only, is not immediate, and is
       intercepted by executed_threaded, is should be entirely
       unreachable. */
    e4__exception_throw(task, e4__E_BUG);
    e4__execute_ret(task);
}

static void e4__builtin_FETCH(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    e4__stack_push(task, e4__DEREF(e4__stack_pop(task)));
    e4__execute_ret(task);
}

static const void *e4__builtin_GREATER_THAN[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_LESS_THAN],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_IF(struct e4__task *task, void *user)
{
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH0]);
    e4__stack_push(task, task->here++);

    e4__execute_ret(task);
}

static void e4__builtin_INVERT(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    e4__stack_push(task, (e4__cell)(~(e4__usize)e4__stack_pop(task)));

    e4__execute_ret(task);
}

static void e4__builtin_LEAVE(struct e4__task *task, void *user)
{
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_SENTINEL]);

    e4__execute_ret(task);
}

static void e4__builtin_LESS_THAN(struct e4__task *task, void *user)
{
    register e4__usize l, r;
    register e4__bool f;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);
    f = e4__USIZE_IS_NEGATIVE(r) == e4__USIZE_IS_NEGATIVE(l) ?
        l < r : e4__USIZE_IS_NEGATIVE(r) < e4__USIZE_IS_NEGATIVE(l);

    e4__stack_push(task, (e4__cell)(f ? e4__BF_TRUE : e4__BF_FALSE));

    e4__execute_ret(task);
}

static void e4__builtin_LITERAL(struct e4__task *task, void *user)
{
    /* Since we're manipulating rp directly rather than calling
       e4__execute_ret, ensure that we aren't going to underflow. */
    _e4__BUILTIN_EXPECT_RDEPTH(task, 1);

    /* FIXME: Add compilation semantics for this word. */
    e4__stack_push(task, e4__DEREF2(++task->rp));
    task->ip = e4__DEREF(task->rp) + 1;

    /* e4__execute_ret intentionally omitted. */
}

static const void *e4__builtin_LOOP[] = {
    e4__execute_threaded,
    NULL,
    /* FIXME: Use COMPILE, or actually correctly implemented LITERAL
       instead here. */
    &e4__BUILTIN_XT[e4__B_LITERAL],
    &e4__BUILTIN_XT[e4__B_LITERAL],
    &e4__BUILTIN_XT[e4__B_COMMA],
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)1,
    &e4__BUILTIN_XT[e4__B_COMMA],
    &e4__BUILTIN_XT[e4__B_PLUS_LOOP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_LSHIFT(struct e4__task *task, void *user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)(l << r));

    e4__execute_ret(task);
}

static void e4__builtin_MAX(struct e4__task *task, void *user)
{
    register e4__usize l, r;
    register e4__bool f;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);
    f = e4__USIZE_IS_NEGATIVE(r) == e4__USIZE_IS_NEGATIVE(l) ?
        l < r : e4__USIZE_IS_NEGATIVE(r) < e4__USIZE_IS_NEGATIVE(l);

    e4__stack_push(task, (e4__cell)(f ? r : l));

    e4__execute_ret(task);
}

static void e4__builtin_MIN(struct e4__task *task, void *user)
{
    register e4__usize l, r;
    register e4__bool f;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);
    f = e4__USIZE_IS_NEGATIVE(r) == e4__USIZE_IS_NEGATIVE(l) ?
        l < r : e4__USIZE_IS_NEGATIVE(r) < e4__USIZE_IS_NEGATIVE(l);

    e4__stack_push(task, (e4__cell)(f ? l : r));

    e4__execute_ret(task);
}

static void e4__builtin_MINUS(struct e4__task *task, void *user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)(l - r));

    e4__execute_ret(task);
}

static const void *e4__builtin_MOD[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_SLASH_MOD],
    &e4__BUILTIN_XT[e4__B_DROP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_NEGATE(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    e4__stack_push(task,
            (e4__cell)e4__USIZE_NEGATE((e4__usize)e4__stack_pop(task)));

    e4__execute_ret(task);
}

static const void *e4__builtin_ONE_MINUS[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)1,
    &e4__BUILTIN_XT[e4__B_MINUS],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void *e4__builtin_ONE_PLUS[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)1,
    &e4__BUILTIN_XT[e4__B_PLUS],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_OR(struct e4__task *task, void *user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    e4__stack_push(task, (e4__cell)(r | l));

    e4__execute_ret(task);
}

static void e4__builtin_OVER(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 2);
    e4__stack_over(task);
    e4__execute_ret(task);
}

static const void *e4__builtin_PAREN[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)((e4__usize)')'),
    &e4__BUILTIN_XT[e4__B_WORD],
    &e4__BUILTIN_XT[e4__B_DROP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_PLUS(struct e4__task *task, void *user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)(l + r));

    e4__execute_ret(task);
}

static void e4__builtin_PLUS_LOOP(struct e4__task *task, void *user)
{
    register e4__cell ref;

    _e4__BUILTIN_EXPECT_REF(task, 1);

    /* FIXME: Replace with 2DUP, and 2DROP when available? */
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_R_FROM]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_PLUS]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_R_FROM]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_OVER]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_OVER]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_TO_R]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_TO_R]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_SWAP]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_EQUALS]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH]);

    ref = e4__stack_pop(task);
    e4__compile_cell(task, (e4__cell)(ref - task->here));

    /* Handle intermediary LEAVE and branch on positive break
       condition. */
    while (ref < task->here) {
        if (*ref == (e4__cell)&e4__BUILTIN_XT[e4__B_SENTINEL])
            *ref = (e4__cell)(task->here - ref);
        ++ref;
    }

    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_R_FROM]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_R_FROM]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_DROP]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_DROP]);

    e4__execute_ret(task);

}

static void e4__builtin_QUIT(struct e4__task *task, void *user)
{
    e4__evaluate_quit(task);
    e4__execute_ret(task);
}

static void e4__builtin_R_FETCH(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_RDEPTH(task, 2);

    /* The top of the return stack is the place to which *we* must
       return, so consume that value before doing anything else. */
    e4__execute_ret(task);

    e4__stack_push(task, e4__stack_rpeek(task));

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_R_FROM(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_RDEPTH(task, 2);

    /* The top of the return stack is the place to which *we* must
       return, so consume that value before doing anything else. */
    e4__execute_ret(task);

    e4__stack_push(task, e4__stack_rpop(task));

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_ROT(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 3);
    e4__stack_rot(task);
    e4__execute_ret(task);
}

static void e4__builtin_RSHIFT(struct e4__task *task, void *user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)(l >> r));

    e4__execute_ret(task);
}

static void e4__builtin_RECURSE(struct e4__task *task, void *user)
{
    /* XXX: Ambiguous behavior--in a DOES> clause that exists within
       a colon definition, RECURSE refers to the function being defined
       by the colon definition, *not* to the code that is part of the
       DOES> clause. This does not throw an exception. */
    e4__compile_cell(task, (e4__cell)task->compile.xt);
    e4__execute_ret(task);
}

static void e4__builtin_REPEAT(struct e4__task *task, void *user)
{
    register e4__cell ref;

    _e4__BUILTIN_EXPECT_REF(task, 2);

    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH]);
    e4__compile_cell(task, (e4__cell)(e4__stack_pop(task) - task->here));

    ref = e4__stack_pop(task);
    e4__DEREF(ref) = (e4__cell)(task->here - ref);

    e4__execute_ret(task);
}

static void e4__builtin_SEMICOLON(struct e4__task *task, void *user)
{
    e4__usize res;

    if ((res = e4__compile_finish(task)))
        e4__exception_throw(task, res);

    e4__execute_ret(task);
}

static const void *e4__builtin_SLASH[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_SLASH_MOD],
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_DROP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_SLASH_MOD(struct e4__task *task, void *user)
{
    register e4__usize n, d;

    /* XXX: Implementation defined behavior--if you're using a C89
       compiler, system division and modulo are either symmetric or
       floored, depending on what your compiler does. If you're using
       a C99 compiler, they're symmetric. */

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    d = (e4__usize)e4__stack_pop(task);
    n = (e4__usize)e4__stack_pop(task);

    if (!d) {
        e4__exception_throw(task, e4__E_DIVBYZERO);
        e4__execute_ret(task);
        return;
    }

    e4__stack_push(task, (e4__cell)e4__num_smod(n, d));
    e4__stack_push(task, (e4__cell)e4__num_sdiv(n, d));

    e4__execute_ret(task);
}

static void e4__builtin_STAR(struct e4__task *task, void *user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    /* XXX: Multiplication can just be done directly since, per section
       3.2.2.2 of the standard, the results after arithmetic overflows
       and/or underflows are implementation defined. In this
       implementation, any multiplication operation, whether signed or
       unsigned, that can accurately be represented in the number of
       bits used to store an e4__cell, will have an accurate result.
       In the event of an overflow, the result will be truncated with
       high bits discarded and low bits kept per the unsigned
       multiplication implementation on the host system. */
    e4__stack_push(task, (e4__cell)(r * l));

    e4__execute_ret(task);
}

static void e4__builtin_STORE(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 2);
    e4__DEREF(e4__stack_pop(task)) = e4__stack_pop(task);
    e4__execute_ret(task);
}

static void e4__builtin_SWAP(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 2);
    e4__stack_swap(task);
    e4__execute_ret(task);
}

static void e4__builtin_THEN(struct e4__task *task, void *user)
{
    register e4__cell ref;

    _e4__BUILTIN_EXPECT_REF(task, 1);

    ref = e4__stack_pop(task);
    e4__DEREF(ref) = (e4__cell)(task->here - ref);

    e4__execute_ret(task);
}

static void e4__builtin_TICK(struct e4__task *task, void *user)
{
    register const char *word;
    register e4__u8 len;
    register const struct e4__dict_header *header;

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    if (!(header = e4__dict_lookup(task, word, len))) {
        e4__exception_throw(task, e4__E_UNDEFWORD);
        e4__execute_ret(task);
    }

    e4__stack_push(task, (e4__cell)header->xt);

    e4__execute_ret(task);
}

static void e4__builtin_TO_BODY(struct e4__task *task, void *user)
{
    register struct e4__execute_token *tok;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    tok = (struct e4__execute_token *)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)&tok->data[0]);

    e4__execute_ret(task);
}

static void e4__builtin_TO_NUMBER(struct e4__task *task, void *user)
{
    register e4__usize initial;
    register const char *buf;
    register e4__usize length;
    register e4__usize consumed;
    e4__usize result;

    _e4__BUILTIN_EXPECT_DEPTH(task, 4);

    length = (e4__usize)e4__stack_pop(task);
    buf = (const char *)e4__stack_pop(task);

    /* FIXME: Correctly handle double cell integers! */
    e4__stack_pop(task);

    initial = (e4__usize)e4__stack_pop(task);
    initial *= (e4__usize)task->base;

    result = 0;
    consumed = e4__mem_number(buf, length, (e4__usize)task->base, 0, &result);
    result += initial;

    e4__stack_push(task, (e4__cell)result);

    /* FIXME: Actually handle double cell integers. */
    e4__stack_push(task, (e4__cell)0);

    e4__stack_push(task, (e4__cell)(buf + consumed));
    e4__stack_push(task, (e4__cell)(length - consumed));

    e4__execute_ret(task);
}

static void e4__builtin_TO_R(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    /* The top of the return stack is the place to which *we* must
       return, so consume that value before doing anything else. */
    e4__execute_ret(task);

    e4__stack_rpush(task, e4__stack_pop(task));

    /* e4__execute_ret intentionally omitted. */
}

static const void *e4__builtin_TWO_SLASH[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)1,
    &e4__BUILTIN_XT[e4__B_RSHIFT],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void *e4__builtin_TWO_STAR[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)1,
    &e4__BUILTIN_XT[e4__B_LSHIFT],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_U_DOT(struct e4__task *task, void *user)
{
    /* FIXME: Once pictured numeric output has been implemented, use
       that instead of this implementation. */
    register e4__usize io_res;
    register char *num;
    register char *buf;
    register e4__usize len;
    register e4__usize n;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    buf = (char *)task->here;
    n = (e4__usize)e4__stack_pop(task);

    num = e4__num_format(n, task->base, 0, buf, 130);
    len = &buf[130] - num;
    num[len++] = ' ';

    if ((io_res = e4__io_type(task, num, len)))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

static void e4__builtin_U_LESS_THAN(struct e4__task *task, void *user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    e4__stack_push(task, (e4__cell)(l < r ? e4__BF_TRUE : e4__BF_FALSE));

    e4__execute_ret(task);
}

static void e4__builtin_UNTIL(struct e4__task *task, void *user)
{
    _e4__BUILTIN_EXPECT_REF(task, 1);

    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH0]);
    e4__compile_cell(task, (e4__cell)(e4__stack_pop(task) - task->here));

    e4__execute_ret(task);
}

static void e4__builtin_VARIABLE(struct e4__task *task, void *user)
{
    register const char *word;
    register e4__u8 len;

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    e4__dict_entry(task, word, len, 0, e4__execute_variable, NULL);
    ++task->here;

    e4__execute_ret(task);
}

static const void *e4__builtin_WHILE[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_IF],
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_XOR(struct e4__task *task, void *user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    e4__stack_push(task, (e4__cell)(r ^ l));

    e4__execute_ret(task);
}

static void e4__builtin_WORD(struct e4__task *task, void *user)
{
    register char delim;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    /* FIXME: Verify that the behavior of this builtin is correct. */

    delim = (char)(e4__usize)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)e4__io_word(task, delim));

    e4__execute_ret(task);
}

static const void *e4__builtin_ZERO_EQUALS[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)0,
    &e4__BUILTIN_XT[e4__B_EQUALS],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void *e4__builtin_ZERO_LESS[] = {
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    (void *)0,
    &e4__BUILTIN_XT[e4__B_LESS_THAN],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

/* vim: set ft=c: */
