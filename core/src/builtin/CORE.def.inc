/* Core word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static void e4__builtin_ABORT(struct e4__task *task, e4__cell user)
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

static void e4__builtin_ABORT_QUOTE(struct e4__task *task, e4__cell user)
{
    const char *str;
    register e4__usize len;
    register e4__cell fref;

    len = e4__io_parse(task, '"', 0, &str);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH0]);
    fref = task->here++;
    e4__compile_str(task, str, len);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_ABORTQ]);
    e4__DEREF(fref) = (e4__cell)(task->here - fref);

    e4__execute_ret(task);
}

static const void * const e4__builtin_ABS[] = {
    &e4__BUILTIN_XT[e4__B_DUP],
    &e4__BUILTIN_XT[e4__B_ZERO_LESS],
    &e4__BUILTIN_XT[e4__B_BRANCH0],
    (void *)2,
    &e4__BUILTIN_XT[e4__B_NEGATE],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_ACCEPT(struct e4__task *task, e4__cell user)
{
    register char *addr;
    register e4__usize io_res;
    e4__usize sz;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    sz = (e4__usize)e4__stack_pop(task);
    addr = (char *)e4__stack_pop(task);

    if ((io_res = e4__io_accept(task, addr, &sz)))
        e4__exception_throw(task, io_res);

    e4__stack_push(task, (e4__cell)sz);

    e4__execute_ret(task);
}

static const void * const e4__builtin_ALIGN[] = {
    &e4__BUILTIN_XT[e4__B_HERE],
    &e4__BUILTIN_XT[e4__B_DUP],
    &e4__BUILTIN_XT[e4__B_ALIGNED],
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_MINUS],
    &e4__BUILTIN_XT[e4__B_ALLOT],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_ALIGNED(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    e4__stack_push(task,
            (e4__cell)e4__mem_aligned((e4__usize)e4__stack_pop(task)));

    e4__execute_ret(task);
}

static void e4__builtin_ALLOT(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    e4__task_allot(task, (e4__usize)e4__stack_pop(task));
    e4__execute_ret(task);
}

static void e4__builtin_AND(struct e4__task *task, e4__cell user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    e4__stack_push(task, (e4__cell)(l & r));

    e4__execute_ret(task);
}

static const void * const e4__builtin_BRACKET_CHAR[] = {
    &e4__BUILTIN_XT[e4__B_CHAR],
    &e4__BUILTIN_XT[e4__B_LITERAL],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void * const e4__builtin_BRACKET_TICK[] = {
    &e4__BUILTIN_XT[e4__B_TICK],
    &e4__BUILTIN_XT[e4__B_LITERAL],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_C_COMMA(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    e4__compile_char(task, e4__stack_pop(task));
    e4__execute_ret(task);
}

static void e4__builtin_C_FETCH(struct e4__task *task, e4__cell user)
{
    register e4__u8 *c;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    c = (e4__u8 *)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)((e4__usize)*c));

    e4__execute_ret(task);
}

static void e4__builtin_FIND(struct e4__task *task, e4__cell user)
{
    register const char *caddr;
    register const struct e4__dict_header *header;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    caddr = (const char *)e4__stack_pop(task);

    _e4__BUILTIN_EXPECT_AVAIL(task, 1);

    header = e4__dict_lookup(task, caddr + 1, (e4__u8)*caddr);
    if (!header) {
        e4__stack_push(task, (e4__cell)caddr);
        e4__stack_push(task, 0);
    } else {
        e4__stack_push(task, (e4__cell)header->xt);
        e4__stack_push(task, (e4__cell)(e4__usize)
                ((header->flags & e4__F_IMMEDIATE) ? 1 : -1));
    }

    e4__execute_ret(task);
}

static void e4__builtin_C_STORE(struct e4__task *task, e4__cell user)
{
    register e4__u8 *c, v;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    c = (e4__u8 *)e4__stack_pop(task);
    v = (e4__u8)((e4__usize)e4__stack_pop(task) & 0xff);
    *c = v;

    e4__execute_ret(task);
}

static const void * const e4__builtin_CELLS[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)sizeof(e4__cell),
    &e4__BUILTIN_XT[e4__B_STAR],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void * const e4__builtin_CELL_PLUS[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)sizeof(e4__cell),
    &e4__BUILTIN_XT[e4__B_PLUS],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_CHAR(struct e4__task *task, e4__cell user)
{
    register const char *word;
    register e4__u8 len;

    /* XXX: This will throw e4__E_ZLNAME on if there is no next word in
       the input buffer from which to take the first character. This
       is a slightly odd exception to throw, but the standard currently
       does not specify what should be done in this scenario (or even
       that it is an ambiguous conditon). */
    _e4__BUILTIN_LOOKAHEAD(task, word, len);
    _e4__BUILTIN_EXPECT_AVAIL(task, 1);

    e4__stack_push(task, (e4__cell)(e4__usize)word[0]);

    e4__execute_ret(task);
}

static const void * const e4__builtin_CHAR_PLUS[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)sizeof(e4__u8),
    &e4__BUILTIN_XT[e4__B_PLUS],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void * const e4__builtin_CHARS[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)sizeof(e4__u8),
    &e4__BUILTIN_XT[e4__B_STAR],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_COLON(struct e4__task *task, e4__cell user)
{
    register const char *word;
    register e4__u8 len;
    register e4__usize res;

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    e4__dict_entry(task, word, len, e4__F_COMPILING, NULL, NULL);
    if ((res = e4__compile_start(task, task->dict->xt, e4__COMP_COLON)))
        e4__exception_throw(task, res);

    e4__execute_ret(task);
}

static void e4__builtin_COMMA(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    e4__compile_cell(task, e4__stack_pop(task));
    e4__execute_ret(task);
}

static void e4__builtin_CONSTANT(struct e4__task *task, e4__cell user)
{
    register const char *word;
    register e4__u8 len;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    e4__dict_entry(task, word, len, e4__F_CONSTANT, e4__execute_value, NULL);
    e4__builtin_exec(task, e4__B_COMMA);

    e4__execute_ret(task);
}

static const void * const e4__builtin_COUNT[] = {
    &e4__BUILTIN_XT[e4__B_DUP],
    &e4__BUILTIN_XT[e4__B_CHAR_PLUS],
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_C_FETCH],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_CR(struct e4__task *task, e4__cell user)
{
    register e4__usize io_res;

    /* FIXME: Should this actually output \r\n? */
    if ((io_res = e4__io_type(task, "\n", 1)))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

static void e4__builtin_CREATE(struct e4__task *task, e4__cell user)
{
    register const char *word;
    register e4__u8 len;

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    e4__dict_entry(task, word, len, 0, e4__execute_variable, NULL);
    e4__execute_ret(task);
}

static const void * const e4__builtin_DECIMAL[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)10,
    &e4__BUILTIN_XT[e4__B_BASE],
    &e4__BUILTIN_XT[e4__B_STORE],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_DEPTH(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_AVAIL(task, 1);
    e4__stack_push(task, (e4__cell)e4__stack_depth(task));
    e4__execute_ret(task);
}

static void e4__builtin_DO(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_AVAIL(task, 1);

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

static void e4__builtin_DOES(struct e4__task *task, e4__cell user)
{
    register e4__usize res;

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
        e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_LIT_DOES]);
    else if (task->dict->flags & e4__F_BUILTIN)
        /* Invalid attempt to mutate a builtin. */
        e4__exception_throw(task, e4__E_INVBUILTINMUT);
    else if ((res = e4__compile_start(task, task->dict->xt, e4__COMP_DOES))) {
        /* XXX: This should never happen. We've already checked that
           we're not compiling. */
        e4__exception_throw(task, res);
    }

    e4__execute_ret(task);
}

static void e4__builtin_DOT(struct e4__task *task, e4__cell user)
{
    register e4__usize io_res;
    char *num;
    e4__usize len;
    struct e4__double d = {0};

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    d = e4__usize_todouble((e4__usize)e4__stack_pop(task));
    e4__io_pno_start(task);

    if ((io_res = e4__io_pno_hold(task, ' ')) ||
            (io_res = e4__io_pno_digits(task, &d, e4__F_SIGNED)) ||
            (io_res = e4__io_pno_finish(task, &num, &len)) ||
            (io_res = e4__io_type(task, num, len)))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

static void e4__builtin_DOT_QUOTE(struct e4__task *task, e4__cell user)
{
    const char *str;
    register e4__usize len;

    len = e4__io_parse(task, '"', 0, &str);

    if (e4__task_compiling(task)) {
        e4__compile_str(task, str, len);
        e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_TYPE]);
    } else {
        register const e4__usize io_res = e4__io_type(task, str, len);
        if (io_res)
            e4__exception_throw(task, io_res);
    }

    e4__execute_ret(task);
}

static void e4__builtin_DROP(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    e4__stack_drop(task);
    e4__execute_ret(task);
}

static void e4__builtin_DUP(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    _e4__BUILTIN_EXPECT_AVAIL(task, 1);
    e4__stack_dup(task);
    e4__execute_ret(task);
}

static void e4__builtin_ELSE(struct e4__task *task, e4__cell user)
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

static void e4__builtin_EMIT(struct e4__task *task, e4__cell user)
{
    register e4__usize io_res;
    char c;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    c = (char)(e4__usize)e4__stack_pop(task);
    if ((io_res = e4__io_type(task, &c, 1)))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

static void e4__builtin_ENVIRONMENT_QUERY(struct e4__task *task, e4__cell user)
{
    register const char *str;
    register e4__usize len;
    e4__usize u = 0;
    struct e4__double d = {0,};
    register e4__usize known = (e4__usize)-1;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    len = (e4__usize)e4__stack_pop(task);
    str = (const char *)e4__stack_pop(task);

    #define _match(_s) \
        ((len == (sizeof(_s) - 1)) && !e4__mem_strncasecmp(str, _s, len))

    /* CORE environmental queries */
    if (_match("/COUNTED-STRING")) {
        e4__task_eq(task, e4__EQ_MAXCSTRSZ, &u);
        e4__stack_push(task, (e4__cell)u);
    } else if (_match("/HOLD")) {
        e4__task_eq(task, e4__EQ_HOLDSZ, &u);
        e4__stack_push(task, (e4__cell)u);
    } else if (_match("/PAD")) {
        e4__task_eq(task, e4__EQ_PADSZ, &u);
        e4__stack_push(task, (e4__cell)u);
    } else if (_match("ADDRESS-UNIT-BITS")) {
        e4__task_eq(task, e4__EQ_ADDRUNITBITS, &u);
        e4__stack_push(task, (e4__cell)u);
    } else if (_match("FLOORED")) {
        e4__task_eq(task, e4__EQ_FLOORED, &u);
        e4__stack_push(task, (e4__cell)u);
    } else if (_match("MAX-CHAR")) {
        e4__task_eq(task, e4__EQ_MAXCHAR, &u);
        e4__stack_push(task, (e4__cell)u);
    } else if (_match("MAX-D")) {
        e4__task_eq(task, e4__EQ_MAXDOUBLE, &d);
        e4__stack_push(task, (e4__cell)d.low);
        e4__stack_push(task, (e4__cell)d.high);
    } else if (_match("MAX-N")) {
        e4__task_eq(task, e4__EQ_MAXINT, &u);
        e4__stack_push(task, (e4__cell)u);
    } else if (_match("MAX-U")) {
        e4__task_eq(task, e4__EQ_MAXUINT, &u);
        e4__stack_push(task, (e4__cell)u);
    } else if (_match("MAX-UD")) {
        e4__task_eq(task, e4__EQ_MAXUDOUBLE, &d);
        e4__stack_push(task, (e4__cell)d.low);
        e4__stack_push(task, (e4__cell)d.high);
    } else if (_match("RETURN-STACK-CELLS")) {
        e4__task_eq(task, e4__EQ_RSTACKCELLS, &u);
        e4__stack_push(task, (e4__cell)u);
    } else if (_match("STACK-CELLS")) {
        e4__task_eq(task, e4__EQ_STACKCELLS, &u);
        e4__stack_push(task, (e4__cell)u);
    } else {
        known = 0;
    }

    #undef _match

    /* We know it was okay to push two elements onto the stack because
       we popped two off, but we may have already pushed two elements
       on. As such, it's necessary to check that there's space for at
       lease one more. */
    _e4__BUILTIN_EXPECT_AVAIL(task, 1);
    e4__stack_push(task, (e4__cell)known);

    e4__execute_ret(task);
}

static void e4__builtin_EQUALS(struct e4__task *task, e4__cell user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    e4__stack_push(task, (e4__cell)(r == l ? e4__BF_TRUE : e4__BF_FALSE));

    e4__execute_ret(task);
}

static void e4__builtin_EVALUATE(struct e4__task *task, e4__cell user)
{
    register e4__usize len;
    register const char *str;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    len = (e4__usize)e4__stack_pop(task);
    str = (const char *)(e4__cell)e4__stack_pop(task);

    /* No need to worry about the return value from this. If exceptions
       are enabled, they'll be thrown from beneath evaluate and caught
       by whatever handler is installed above us. If exceptions aren't
       enabled, there's nothing for us to do anyhow. */
    e4__evaluate(task, str, len);

    e4__execute_ret(task);
}

static void e4__builtin_EXECUTE(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    e4__execute(task, e4__stack_pop(task));
    e4__execute_ret(task);
}

static void e4__builtin_EXIT(struct e4__task *task, e4__cell user)
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

static void e4__builtin_FETCH(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    e4__stack_push(task, e4__DEREF(e4__stack_pop(task)));
    e4__execute_ret(task);
}

static void e4__builtin_FILL(struct e4__task *task, e4__cell user)
{
    register e4__cell dest;
    register e4__usize count;
    register e4__u8 val;

    _e4__BUILTIN_EXPECT_DEPTH(task, 3);

    val = (e4__u8)(e4__usize)e4__stack_pop(task);
    count = (e4__usize)e4__stack_pop(task);
    dest = e4__stack_pop(task);

    memset(dest, val, (size_t)count);

    e4__execute_ret(task);
}

static void e4__builtin_FM_SLASH_MOD(struct e4__task *task, e4__cell user)
{
    register e4__usize res, n;
    register struct e4__double d;
    e4__usize q, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 3);

    n = (e4__usize)e4__stack_pop(task);
    d.high = (e4__usize)e4__stack_pop(task);
    d.low = (e4__usize)e4__stack_pop(task);

    if ((res = e4__double_ndiv(d, n, e4__F_SIGNED | e4__F_FLOORDIV, &q, &r))) {
        e4__exception_throw(task, res);
        e4__execute_ret(task);
    }

    e4__stack_push(task, (e4__cell)r);
    e4__stack_push(task, (e4__cell)q);

    e4__execute_ret(task);
}

static const void * const e4__builtin_GREATER_THAN[] = {
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_LESS_THAN],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_HOLD(struct e4__task *task, e4__cell user)
{
    register char c;
    register e4__usize res;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    c = (char)(e4__usize)e4__stack_pop(task);
    if ((res = e4__io_pno_hold(task, c)))
        e4__exception_throw(task, res);

    e4__execute_ret(task);
}

static void e4__builtin_IF(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_AVAIL(task, 1);

    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH0]);
    e4__stack_push(task, task->here++);

    e4__execute_ret(task);
}

static void e4__builtin_IMMEDIATE(struct e4__task *task, e4__cell user)
{
    if (task->dict->flags & e4__F_BUILTIN)
        e4__exception_throw(task, e4__E_INVBUILTINMUT);
    else
        task->dict->flags |= e4__F_IMMEDIATE;

    e4__execute_ret(task);
}

static void e4__builtin_INVERT(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    e4__stack_push(task, (e4__cell)(~(e4__usize)e4__stack_pop(task)));

    e4__execute_ret(task);
}

static void e4__builtin_J(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_RDEPTH(task, 5);
    _e4__BUILTIN_EXPECT_AVAIL(task, 1);

    /* The top of the return stack is the place to which *we* must
       return, so consume that value before doing anything else. */
    e4__execute_ret(task);

    e4__stack_push(task, task->rp[3]);

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_KEY(struct e4__task *task, e4__cell user)
{
    register e4__usize io_res;
    char k;

    if ((io_res = e4__io_key(task, &k)))
        e4__exception_throw(task, io_res);

    _e4__BUILTIN_EXPECT_AVAIL(task, 1);

    e4__stack_push(task, (e4__cell)(e4__usize)k);

    e4__execute_ret(task);
}

static void e4__builtin_LEAVE(struct e4__task *task, e4__cell user)
{
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_SENTINEL]);

    e4__execute_ret(task);
}

static void e4__builtin_LEFT_BRACKET(struct e4__task *task, e4__cell user)
{
    e4__compile_suspend(task);
    e4__execute_ret(task);
}

static void e4__builtin_LESS_NUMBER_SIGN(struct e4__task *task, e4__cell user)
{
    e4__io_pno_start(task);
    e4__execute_ret(task);
}

static void e4__builtin_LESS_THAN(struct e4__task *task, e4__cell user)
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

static void e4__builtin_LITERAL(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    e4__compile_literal(task, e4__stack_pop(task));

    e4__execute_ret(task);
}

static const void * const e4__builtin_LOOP[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)1,
    &e4__BUILTIN_XT[e4__B_LITERAL],
    &e4__BUILTIN_XT[e4__B_PLUS_LOOP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_LSHIFT(struct e4__task *task, e4__cell user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)(l << r));

    e4__execute_ret(task);
}

static void e4__builtin_M_STAR(struct e4__task *task, e4__cell user)
{
    register e4__usize l, r;
    register struct e4__double d;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    d = e4__usize_mul(l, r, e4__F_SIGNED);

    e4__stack_push(task, (e4__cell)d.low);
    e4__stack_push(task, (e4__cell)d.high);

    e4__execute_ret(task);
}

static void e4__builtin_MAX(struct e4__task *task, e4__cell user)
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

static void e4__builtin_MIN(struct e4__task *task, e4__cell user)
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

static void e4__builtin_MINUS(struct e4__task *task, e4__cell user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)(l - r));

    e4__execute_ret(task);
}

static const void * const e4__builtin_MOD[] = {
    &e4__BUILTIN_XT[e4__B_SLASH_MOD],
    &e4__BUILTIN_XT[e4__B_DROP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_MOVE(struct e4__task *task, e4__cell user)
{
    register e4__cell src, dest;
    register e4__usize count;

    _e4__BUILTIN_EXPECT_DEPTH(task, 3);

    count = (e4__usize)e4__stack_pop(task);
    dest = e4__stack_pop(task);
    src = e4__stack_pop(task);

    memmove(dest, src, (size_t)count);

    e4__execute_ret(task);
}

static void e4__builtin_NEGATE(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    e4__stack_push(task,
            (e4__cell)e4__USIZE_NEGATE((e4__usize)e4__stack_pop(task)));

    e4__execute_ret(task);
}

static void e4__builtin_NUMBER_SIGN(struct e4__task *task, e4__cell user)
{
    register e4__usize res;
    struct e4__double *d;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    d = (struct e4__double *)&task->sp[1];
    if ((res = e4__io_pno_digit(task, d)))
        e4__exception_throw(task, res);

    e4__execute_ret(task);
}

static void e4__builtin_NUMBER_SIGN_GREATER(struct e4__task *task,
        e4__cell user)
{
    register e4__usize res;
    char *str;
    e4__usize len;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    if ((res = e4__io_pno_finish(task, &str, &len))) {
        e4__exception_throw(task, res);
        e4__execute_ret(task);
        return;
    }

    e4__stack_drop(task);
    e4__stack_drop(task);

    e4__stack_push(task, (e4__cell)str);
    e4__stack_push(task, (e4__cell)len);

    e4__execute_ret(task);
}

static void e4__builtin_NUMBER_SIGN_S(struct e4__task *task, e4__cell user)
{
    register e4__usize res;
    struct e4__double *d;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    d = (struct e4__double *)&task->sp[1];
    if ((res = e4__io_pno_digits(task, d, 0)))
        e4__exception_throw(task, res);

    e4__execute_ret(task);
}

static const void * const e4__builtin_ONE_MINUS[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)1,
    &e4__BUILTIN_XT[e4__B_MINUS],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void * const e4__builtin_ONE_PLUS[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)1,
    &e4__BUILTIN_XT[e4__B_PLUS],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_OR(struct e4__task *task, e4__cell user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    e4__stack_push(task, (e4__cell)(r | l));

    e4__execute_ret(task);
}

static void e4__builtin_OVER(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 2);
    _e4__BUILTIN_EXPECT_AVAIL(task, 1);
    e4__stack_over(task);
    e4__execute_ret(task);
}

static void e4__builtin_POSTPONE(struct e4__task *task, e4__cell user)
{
    register const char *word;
    register e4__u8 len;
    register const struct e4__dict_header *header;

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    if (!(header = e4__dict_lookup(task, word, len))) {
        e4__exception_throw(task, e4__E_UNDEFWORD);
        e4__execute_ret(task);
    }

    e4__compile_cell(task, (e4__cell)header->xt);

    e4__execute_ret(task);
}

static const void * const e4__builtin_PAREN[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)((e4__usize)')'),
    &e4__BUILTIN_XT[e4__B_WORD],
    &e4__BUILTIN_XT[e4__B_DROP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_PLUS(struct e4__task *task, e4__cell user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)(l + r));

    e4__execute_ret(task);
}

static void e4__builtin_PLUS_LOOP(struct e4__task *task, e4__cell user)
{
    register e4__cell ref;

    _e4__BUILTIN_EXPECT_REF(task, 1);

    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_R_FROM]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_R_FETCH]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_LOOP_INC]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_TO_R]);
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

    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_UNLOOP]);

    e4__execute_ret(task);
}

static const void * const e4__builtin_PLUS_STORE[] = {
    &e4__BUILTIN_XT[e4__B_DUP],
    &e4__BUILTIN_XT[e4__B_FETCH],
    &e4__BUILTIN_XT[e4__B_ROT],
    &e4__BUILTIN_XT[e4__B_PLUS],
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_STORE],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void * const e4__builtin_QUESTION_DUP[] = {
    &e4__BUILTIN_XT[e4__B_DUP],
    &e4__BUILTIN_XT[e4__B_BRANCH0],
    (void *)2,
    &e4__BUILTIN_XT[e4__B_DUP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_QUIT(struct e4__task *task, e4__cell user)
{
    e4__evaluate_quit(task);
    e4__execute_ret(task);
}

static void e4__builtin_R_FETCH(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_RDEPTH(task, 2);
    _e4__BUILTIN_EXPECT_AVAIL(task, 1);

    /* The top of the return stack is the place to which *we* must
       return, so consume that value before doing anything else. */
    e4__execute_ret(task);

    e4__stack_push(task, e4__stack_rpeek(task));

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_R_FROM(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_RDEPTH(task, 2);
    _e4__BUILTIN_EXPECT_AVAIL(task, 1);

    /* The top of the return stack is the place to which *we* must
       return, so consume that value before doing anything else. */
    e4__execute_ret(task);

    e4__stack_push(task, e4__stack_rpop(task));

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_RIGHT_BRACKET(struct e4__task *task, e4__cell user)
{
    register e4__usize res;

    if ((res = e4__compile_resume(task)))
        e4__exception_throw(task, res);

    e4__execute_ret(task);
}

static void e4__builtin_ROT(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 3);
    e4__stack_rot(task);
    e4__execute_ret(task);
}

static void e4__builtin_RSHIFT(struct e4__task *task, e4__cell user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)(l >> r));

    e4__execute_ret(task);
}

static void e4__builtin_RECURSE(struct e4__task *task, e4__cell user)
{
    /* XXX: Ambiguous behavior--in a DOES> clause that exists within
       a colon definition, RECURSE refers to the function being defined
       by the colon definition, *not* to the code that is part of the
       DOES> clause. This does not throw an exception. */
    e4__compile_cell(task, (e4__cell)task->compile.xt);
    e4__execute_ret(task);
}

static void e4__builtin_REPEAT(struct e4__task *task, e4__cell user)
{
    register e4__cell ref;

    _e4__BUILTIN_EXPECT_REF(task, 2);

    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH]);
    e4__compile_cell(task, (e4__cell)(e4__stack_pop(task) - task->here));

    ref = e4__stack_pop(task);
    e4__DEREF(ref) = (e4__cell)(task->here - ref);

    e4__execute_ret(task);
}

static void e4__builtin_S_TO_D(struct e4__task *task, e4__cell user)
{
    register struct e4__double d;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    _e4__BUILTIN_EXPECT_AVAIL(task, 1);

    d = e4__usize_todouble((e4__usize)e4__stack_pop(task));
    e4__stack_push(task, (e4__cell)d.low);
    e4__stack_push(task, (e4__cell)d.high);

    e4__execute_ret(task);
}

static void e4__builtin_S_QUOTE(struct e4__task *task, e4__cell user)
{
    const char *str;
    register e4__usize len;

    #if !defined(e4__INCLUDE_FILE)
        if (!e4__task_compiling(task)) {
            e4__exception_throw(task, e4__E_COMPONLYWORD);
            e4__execute_ret(task);
            return;
        }
    #endif

    len = e4__io_parse(task, '"', 0, &str);

    if (e4__task_compiling(task)) {
        e4__compile_str(task, str, len);
    } else {
        #if defined(e4__INCLUDE_FILE)
            if (!(str = e4__mem_cbuf_push(&task->tsb, str, len)) && len) {
                e4__exception_throw(task, e4__E_PSTROVERFLOW);
            } else {
                _e4__BUILTIN_EXPECT_AVAIL(task, 2);
                e4__stack_push(task, (e4__cell)str);
                e4__stack_push(task, (e4__cell)len);
            }
        #endif /* defined(e4__INCLUDE_FILE) */
    }

    e4__execute_ret(task);
}

static void e4__builtin_SEMICOLON(struct e4__task *task, e4__cell user)
{
    e4__usize res;

    if ((res = e4__compile_finish(task)))
        e4__exception_throw(task, res);

    e4__execute_ret(task);
}

static void e4__builtin_SIGN(struct e4__task *task, e4__cell user)
{
    register e4__usize res;
    register e4__usize n;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    n = (e4__usize)e4__stack_pop(task);
    if (e4__USIZE_IS_NEGATIVE(n) &&
            (res = e4__io_pno_hold(task, '-')))
        e4__exception_throw(task, res);

    e4__execute_ret(task);

}

static const void * const e4__builtin_SLASH[] = {
    &e4__BUILTIN_XT[e4__B_SLASH_MOD],
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_DROP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_SLASH_MOD(struct e4__task *task, e4__cell user)
{
    register e4__usize n, d, q;
    e4__usize r;

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

    q = e4__usize_sdiv(n, d, &r);
    e4__stack_push(task, (e4__cell)r);
    e4__stack_push(task, (e4__cell)q);

    e4__execute_ret(task);
}

static void e4__builtin_SM_SLASH_REM(struct e4__task *task, e4__cell user)
{
    register e4__usize res, n;
    register struct e4__double d;
    e4__usize q, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 3);

    n = (e4__usize)e4__stack_pop(task);
    d.high = (e4__usize)e4__stack_pop(task);
    d.low = (e4__usize)e4__stack_pop(task);

    if ((res = e4__double_ndiv(d, n, e4__F_SIGNED, &q, &r))) {
        e4__exception_throw(task, res);
        e4__execute_ret(task);
    }

    e4__stack_push(task, (e4__cell)r);
    e4__stack_push(task, (e4__cell)q);

    e4__execute_ret(task);
}

static void e4__builtin_SOURCE(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_AVAIL(task, 2);

    e4__stack_push(task, (e4__cell)task->io_src.buffer);
    e4__stack_push(task, (e4__cell)task->io_src.length);

    e4__execute_ret(task);
}

static const void * const e4__builtin_SPACE[] = {
    &e4__BUILTIN_XT[e4__B_BL],
    &e4__BUILTIN_XT[e4__B_EMIT],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void * const e4__builtin_SPACES[] = {
    &e4__BUILTIN_XT[e4__B_DUP],
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)0,
    &e4__BUILTIN_XT[e4__B_GREATER_THAN],
    &e4__BUILTIN_XT[e4__B_BRANCH0],
    (void *)5,
    &e4__BUILTIN_XT[e4__B_SPACE],
    &e4__BUILTIN_XT[e4__B_ONE_MINUS],
    &e4__BUILTIN_XT[e4__B_BRANCH],
    (void *)e4__USIZE_NEGATE(9),
    &e4__BUILTIN_XT[e4__B_DROP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_STAR(struct e4__task *task, e4__cell user)
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

static void e4__builtin_STORE(struct e4__task *task, e4__cell user)
{
    register e4__cell c;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    c = e4__stack_pop(task);
    e4__DEREF(c) = e4__stack_pop(task);

    e4__execute_ret(task);
}

static void e4__builtin_STAR_SLASH(struct e4__task *task, e4__cell user)
{
    register e4__usize l, r, d, res;
    register struct e4__double p;
    e4__usize q;

    _e4__BUILTIN_EXPECT_DEPTH(task, 3);

    d = (e4__usize)e4__stack_pop(task);
    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    p = e4__usize_mul(l, r, e4__F_SIGNED);

    if ((res = e4__double_ndiv(p, d, e4__F_SIGNED, &q, NULL))) {
        e4__exception_throw(task, res);
        e4__execute_ret(task);
    }

    e4__stack_push(task, (e4__cell)q);

    e4__execute_ret(task);
}

static void e4__builtin_STAR_SLASH_MOD(struct e4__task *task, e4__cell user)
{
    register e4__usize l, r, d, res;
    register struct e4__double p;
    e4__usize q, rem;

    _e4__BUILTIN_EXPECT_DEPTH(task, 3);

    d = (e4__usize)e4__stack_pop(task);
    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    p = e4__usize_mul(l, r, e4__F_SIGNED);

    if ((res = e4__double_ndiv(p, d, e4__F_SIGNED, &q, &rem))) {
        e4__exception_throw(task, res);
        e4__execute_ret(task);
    }

    e4__stack_push(task, (e4__cell)rem);
    e4__stack_push(task, (e4__cell)q);

    e4__execute_ret(task);

}

static void e4__builtin_SWAP(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 2);
    e4__stack_swap(task);
    e4__execute_ret(task);
}

static void e4__builtin_THEN(struct e4__task *task, e4__cell user)
{
    register e4__cell ref;

    _e4__BUILTIN_EXPECT_REF(task, 1);

    ref = e4__stack_pop(task);
    e4__DEREF(ref) = (e4__cell)(task->here - ref);

    e4__execute_ret(task);
}

static void e4__builtin_TICK(struct e4__task *task, e4__cell user)
{
    register const char *word;
    register e4__u8 len;
    register const struct e4__dict_header *header;

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    if (!(header = e4__dict_lookup(task, word, len))) {
        e4__exception_throw(task, e4__E_UNDEFWORD);
        e4__execute_ret(task);
    }

    _e4__BUILTIN_EXPECT_AVAIL(task, 1);

    e4__stack_push(task, (e4__cell)header->xt);

    e4__execute_ret(task);
}

static void e4__builtin_TO_BODY(struct e4__task *task, e4__cell user)
{
    register struct e4__execute_token *tok;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    tok = (struct e4__execute_token *)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)&tok->data[0]);

    e4__execute_ret(task);
}

static void e4__builtin_TO_NUMBER(struct e4__task *task, e4__cell user)
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

static void e4__builtin_TO_R(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_DEPTH(task, 1);
    _e4__BUILTIN_EXPECT_RAVAIL(task, 1);

    /* The top of the return stack is the place to which *we* must
       return, so consume that value before doing anything else. */
    e4__execute_ret(task);

    e4__stack_rpush(task, e4__stack_pop(task));

    /* e4__execute_ret intentionally omitted. */
}

static const void * const e4__builtin_TWO_DROP[] = {
    &e4__BUILTIN_XT[e4__B_DROP],
    &e4__BUILTIN_XT[e4__B_DROP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void * const e4__builtin_TWO_DUP[] = {
    &e4__BUILTIN_XT[e4__B_OVER],
    &e4__BUILTIN_XT[e4__B_OVER],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void * const e4__builtin_TWO_FETCH[] = {
    &e4__BUILTIN_XT[e4__B_DUP],
    &e4__BUILTIN_XT[e4__B_CELL_PLUS],
    &e4__BUILTIN_XT[e4__B_FETCH],
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_FETCH],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void * const e4__builtin_TWO_OVER[] = {
    &e4__BUILTIN_XT[e4__B_TO_R],
    &e4__BUILTIN_XT[e4__B_TO_R],
    &e4__BUILTIN_XT[e4__B_TWO_DUP],
    &e4__BUILTIN_XT[e4__B_R_FROM],
    &e4__BUILTIN_XT[e4__B_R_FROM],
    &e4__BUILTIN_XT[e4__B_TWO_SWAP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_TWO_SLASH(struct e4__task *task, e4__cell user)
{
    register e4__usize n;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    n = (e4__usize)e4__stack_pop(task);
    n = (n & ((e4__usize)1 << (e4__USIZE_BIT - 1))) | (n >> 1);
    e4__stack_push(task, (e4__cell)n);

    e4__execute_ret(task);
}

static void e4__builtin_TWO_STAR(struct e4__task *task, e4__cell user)
{
    register e4__usize n;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    n = (e4__usize)e4__stack_pop(task);
    n = (n & ((e4__usize)1 << (e4__USIZE_BIT - 1))) | (n << 1);
    e4__stack_push(task, (e4__cell)n);

    e4__execute_ret(task);
}

static const void * const e4__builtin_TWO_STORE[] = {
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_OVER],
    &e4__BUILTIN_XT[e4__B_STORE],
    &e4__BUILTIN_XT[e4__B_CELL_PLUS],
    &e4__BUILTIN_XT[e4__B_STORE],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void * const e4__builtin_TWO_SWAP[] = {
    &e4__BUILTIN_XT[e4__B_ROT],
    &e4__BUILTIN_XT[e4__B_TO_R],
    &e4__BUILTIN_XT[e4__B_ROT],
    &e4__BUILTIN_XT[e4__B_R_FROM],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_TYPE(struct e4__task *task, e4__cell user)
{
    register e4__cell addr, u;
    register e4__usize io_res;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    u = e4__stack_pop(task);
    addr = e4__stack_pop(task);

    if ((io_res = e4__io_type(task, (const char *)addr, (e4__usize)u)))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

static void e4__builtin_U_DOT(struct e4__task *task, e4__cell user)
{
    register e4__usize io_res;
    char *num;
    e4__usize len;
    struct e4__double d = {0};

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    /* We don't care about sign bits at all, so simply treat the number
       to be displayed as the lower half of a double. */
    d = e4__double_u((e4__usize)e4__stack_pop(task), 0);
    e4__io_pno_start(task);

    if ((io_res = e4__io_pno_hold(task, ' ')) ||
            (io_res = e4__io_pno_digits(task, &d, 0)) ||
            (io_res = e4__io_pno_finish(task, &num, &len)) ||
            (io_res = e4__io_type(task, num, len)))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);

}

static void e4__builtin_U_LESS_THAN(struct e4__task *task, e4__cell user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    e4__stack_push(task, (e4__cell)(l < r ? e4__BF_TRUE : e4__BF_FALSE));

    e4__execute_ret(task);
}

static void e4__builtin_UM_SLASH_MOD(struct e4__task *task, e4__cell user)
{
    register e4__usize res, n;
    register struct e4__double d;
    e4__usize q, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 3);

    n = (e4__usize)e4__stack_pop(task);
    d.high = (e4__usize)e4__stack_pop(task);
    d.low = (e4__usize)e4__stack_pop(task);

    if ((res = e4__double_ndiv(d, n, 0, &q, &r))) {
        e4__exception_throw(task, res);
        e4__execute_ret(task);
    }

    e4__stack_push(task, (e4__cell)r);
    e4__stack_push(task, (e4__cell)q);

    e4__execute_ret(task);
}

static void e4__builtin_UM_STAR(struct e4__task *task, e4__cell user)
{
    register e4__usize l, r;
    register struct e4__double d;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    d = e4__usize_mul(l, r, 0);

    e4__stack_push(task, (e4__cell)d.low);
    e4__stack_push(task, (e4__cell)d.high);

    e4__execute_ret(task);
}

static void e4__builtin_UNLOOP(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_RDEPTH(task, 3);

    /* The top of the return stack is the place to which *we* must
       return, so consume that value before doing anything else. */
    e4__execute_ret(task);

    /* Discard the top two return stack entries. */
    e4__stack_rpop(task);
    e4__stack_rpop(task);

    /* e4__execute_ret intentionally omitted. */
}

static void e4__builtin_UNTIL(struct e4__task *task, e4__cell user)
{
    _e4__BUILTIN_EXPECT_REF(task, 1);

    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_BRANCH0]);
    e4__compile_cell(task, (e4__cell)(e4__stack_pop(task) - task->here));

    e4__execute_ret(task);
}

static void e4__builtin_VARIABLE(struct e4__task *task, e4__cell user)
{
    register const char *word;
    register e4__u8 len;

    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    e4__dict_entry(task, word, len, 0, e4__execute_variable, NULL);
    ++task->here;

    e4__execute_ret(task);
}

static const void * const e4__builtin_WHILE[] = {
    &e4__BUILTIN_XT[e4__B_IF],
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_XOR(struct e4__task *task, e4__cell user)
{
    register e4__usize l, r;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    r = (e4__usize)e4__stack_pop(task);
    l = (e4__usize)e4__stack_pop(task);

    e4__stack_push(task, (e4__cell)(r ^ l));

    e4__execute_ret(task);
}

static void e4__builtin_WORD(struct e4__task *task, e4__cell user)
{
    register char delim;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    /* FIXME: Verify that the behavior of this builtin is correct. */

    delim = (char)(e4__usize)e4__stack_pop(task);
    e4__stack_push(task, (e4__cell)e4__io_word(task, delim));

    e4__execute_ret(task);
}

static const void * const e4__builtin_ZERO_EQUALS[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)0,
    &e4__BUILTIN_XT[e4__B_EQUALS],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static const void * const e4__builtin_ZERO_LESS[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)0,
    &e4__BUILTIN_XT[e4__B_LESS_THAN],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

/* vim: set ft=c: */
