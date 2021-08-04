#include "e4.h"
#include "e4-task.h"
#include <string.h>

/* FIXME: Let this support "compile time" flags, etc. */
#define _e4__BUILTIN_DECL() \
    _e4__BUILTIN_PROC_FIRST(RET)  \
    _e4__BUILTIN_PROC(ABORT)    \
    _e4__BUILTIN_PROC(CLEAR)    \
    _e4__BUILTIN_PROC(DEPTH)    \
    _e4__BUILTIN_PROC(DROP) \
    _e4__BUILTIN_PROC(DUP)  \
    _e4__BUILTIN_PROC_NAMED(GTNUMBER, ">NUMBER")    \
    _e4__BUILTIN_PROC(LIT)  \
    _e4__BUILTIN_PROC(OVER) \
    _e4__BUILTIN_PROC_NAMED(PRINTSTACK, ".S")   \
    _e4__BUILTIN_PROC(REFILL)   \
    _e4__BUILTIN_PROC(ROT)  \
    _e4__BUILTIN_PROC(ROLL) \
    _e4__BUILTIN_PROC(SKIP) \
    _e4__BUILTIN_PROC(SWAP) \
    _e4__BUILTIN_PROC(TUCK) \
    _e4__BUILTIN_PROC(WORD)

#define _e4__BUILTIN_PROC(s)    \
    _e4__BUILTIN_PROC_NAMED(s, #s)

/* Declare builtin functions. */
#define _e4__BUILTIN_PROC_FIRST(w)  \
    void e4__builtin_##w(struct e4__task *task, void *user);
#define _e4__BUILTIN_PROC_NAMED(w, n)   \
    static void e4__builtin_##w(struct e4__task *task, void *user);

_e4__BUILTIN_DECL();

#undef _e4__BUILTIN_PROC_NAMED
#undef _e4__BUILTIN_PROC_FIRST

/* Define builtin header table. */
#define _e4__BUILTIN_PROC_HEADER(w, link, n)    \
    {   \
        link,   \
        (struct e4__execute_token *)&e4__BUILTIN_XT[e4__B_##w],     \
        e4__F_BUILTIN,  \
        sizeof(n) - 1, \
        n, \
    },
#define _e4__BUILTIN_PROC_FIRST(w)  \
    _e4__BUILTIN_PROC_HEADER(w, NULL, #w)
#define _e4__BUILTIN_PROC_NAMED(w, n)   \
    _e4__BUILTIN_PROC_HEADER(w, \
            (struct e4__dict_header *)&e4__BUILTIN_HEADER[e4__B_##w - 1],   \
            n)

const struct e4__dict_header e4__BUILTIN_HEADER[e4__BUILTIN_COUNT] =
{
    _e4__BUILTIN_DECL()
};

#undef _e4__BUILTIN_PROC_NAMED
#undef _e4__BUILTIN_PROC_FIRST
#undef _e4__BUILTIN_PROC_HEADER

/* Define builtin execution token table. */
#define _e4__BUILTIN_PROC_FIRST(w)  _e4__BUILTIN_PROC(w)
#define _e4__BUILTIN_PROC_NAMED(w, n)   \
    {e4__builtin_##w, NULL},

const struct e4__execute_token e4__BUILTIN_XT[e4__BUILTIN_COUNT] =
{
    _e4__BUILTIN_DECL()
};

#undef _e4__BUILTIN_PROC_NAMED
#undef _e4__BUILTIN_PROC_FIRST
#undef _e4__BUILTIN_PROC
#undef _e4__BUILTIN_DECL

/* TODO: Add some sort of e4__builtin_thunk which basically just calls
   e4__execute on whatever is pointed to by the *user field. This could
   allow for builtins implemented in Forth that don't need to be
   compiled or have their code components live in the user dictionary. */

void e4__builtin_RET(struct e4__task *task, void *user)
{
    task->ip = e4__DEREF(++task->rp);
}

static void e4__builtin_ABORT(struct e4__task *task, void *user)
{
    /* FIXME: Ensure the behavior of this function is correct. */
    register int i;
    static const void *RETURN[] =
    {
        e4__builtin_RET
    };

    for (i = 1; &task->rp[i] <= task->r0; ++i)
        task->rp[i] = RETURN;
    task->ip = e4__DEREF(++task->rp);
}

static void e4__builtin_CLEAR(struct e4__task *task, void *user)
{
    e4__stack_clear(task);
    e4__builtin_RET(task, NULL);
}

static void e4__builtin_DEPTH(struct e4__task *task, void *user)
{
    e4__stack_push(task, (e4__cell)e4__stack_depth(task));
    e4__builtin_RET(task, NULL);
}

static void e4__builtin_DROP(struct e4__task *task, void *user)
{
    e4__stack_drop(task);
    e4__builtin_RET(task, NULL);
}

static void e4__builtin_DUP(struct e4__task *task, void *user)
{
    e4__stack_dup(task);
    e4__builtin_RET(task, NULL);
}

static void e4__builtin_GTNUMBER(struct e4__task *task, void *user)
{
    register e4__usize initial;
    register const char *buf;
    register e4__usize length;
    register e4__usize consumed;
    e4__usize result;

    /* FIXME: Should task struct fields just be used here rather than
       the C stack API? */
    if (e4__stack_depth(task) < 4) {
        /* FIXME: Underflow. */
    }

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

    e4__builtin_RET(task, NULL);
}

static void e4__builtin_LIT(struct e4__task *task, void *user)
{
    *task->sp-- = e4__DEREF2(++task->rp);
    task->ip = e4__DEREF(task->rp) + 1;
}

static void e4__builtin_OVER(struct e4__task *task, void *user)
{
    e4__stack_over(task);
    e4__builtin_RET(task, NULL);
}

 /* XXX: From the Programming-Tools word set. */
static void e4__builtin_PRINTSTACK(struct e4__task *task, void *user)
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
    len += 2;
    *--num = '<';
    num[len - 1] = '>';

    if ((io_res = e4__io_type(task, num, len))) {
        e4__exception_throw(task, io_res);
        e4__builtin_RET(task, NULL);
        return;
    }

    while (task->sp < s) {
        n = (e4__usize)e4__DEREF(s--);
        num = e4__num_format(n, task->base, e4__F_SIGNED, &buf[1], 130);
        len = &buf[131] - num;
        len += 1;
        *--num = ' ';

        if ((io_res = e4__io_type(task, num, len))) {
            e4__exception_throw(task, io_res);
            e4__builtin_RET(task, NULL);
            return;
        }
    }

    e4__builtin_RET(task, NULL);
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
        e4__builtin_RET(task, NULL);
        return;
    }

    e4__stack_push(task, (e4__cell)e4__BF_TRUE);
}

static void e4__builtin_ROT(struct e4__task *task, void *user)
{
    e4__stack_rot(task);
    e4__builtin_RET(task, NULL);
}

static void e4__builtin_ROLL(struct e4__task *task, void *user)
{
    e4__stack_roll(task);
    e4__builtin_RET(task, NULL);
}

static void e4__builtin_SKIP(struct e4__task *task, void *user)
{
    task->ip = e4__DEREF(++task->rp) + 1;
}

static void e4__builtin_SWAP(struct e4__task *task, void *user)
{
    e4__stack_swap(task);
    e4__builtin_RET(task, NULL);
}

static void e4__builtin_TUCK(struct e4__task *task, void *user)
{
    e4__stack_tuck(task);
    e4__builtin_RET(task, NULL);
}

static void e4__builtin_WORD(struct e4__task *task, void *user)
{
    register char delim;
    register e4__usize length;
    register e4__u8 clamped_length;
    const char *word = NULL;

    /* FIXME: Should task struct fields just be used here rather than
       the C stack API? */
    if (e4__stack_depth(task) < 1) {
        /* FIXME: Underflow. */
    }

    /* Parse word. */
    delim = (e4__usize)e4__stack_pop(task);
    word = (const char *)task->io_src.buffer + (e4__usize)task->io_src.in;

    if ((e4__usize)task->io_src.length > (e4__usize)task->io_src.in)
        length = (e4__usize)task->io_src.length - (e4__usize)task->io_src.in;
    else
        length = 0;

    length = e4__mem_parse(word, delim, length, e4__F_SKIP_LEADING, &word);

    /* XXX: Ambiguous condition. We take only the first 255 bytes of
       a parsed string that is longer than this, but we advance >IN
       to the actual end of the word (beyond the encountered delim),
       as per the specified delim. */

    /* Write word to HERE. */
    /* FIXME: Check that there is space in HERE before writing
       to it. */
    clamped_length = length < 256 ? (e4__u8)length : 255;
    *((e4__u8 *)task->here) = clamped_length;
    memcpy((e4__u8 *)task->here + 1, word, clamped_length);

    /* Update offset and push result. */
    task->io_src.in = word + length + 1 - (const char *)task->io_src.buffer;
    e4__stack_push(task, task->here);

    e4__builtin_RET(task, NULL);
}
