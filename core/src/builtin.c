#include "e4-debug.h"
#include "e4.h"
#include "e4-task.h"
#include <string.h>

/* FIXME: Let this support "compile time" flags, etc. */
#define _e4__BUILTIN_DECL() \
    _e4__BUILTIN_PROC_FIRST(RET)  \
    _e4__BUILTIN_PROC(ABORT)    \
    _e4__BUILTIN_PROC_NAMED(GTNUMBER, ">NUMBER")    \
    _e4__BUILTIN_PROC(LIT)  \
    _e4__BUILTIN_PROC(SKIP) \
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
    printf("Returning to %p\n", (void *)task->ip);
}

void e4__builtin_ABORT(struct e4__task *task, void *user)
{
    /* FIXME: Ensure the behavior of this function is correct. */

    register int i;
    static const void *RETURN[] =
    {
        e4__builtin_RET
    };

    printf("ABORTING ALL EXECUTION...\n");
    for (i = 1; &task->rp[i] <= task->r0; ++i)
        task->rp[i] = RETURN;
    task->ip = e4__DEREF(++task->rp);
}

void e4__builtin_GTNUMBER(struct e4__task *task, void *user)
{
    register unsigned long initial;
    register const char *buf;
    register unsigned long length;
    register unsigned long consumed;
    unsigned long long result;

    /* FIXME: Should task struct fields just be used here rather than
       the C stack API? */
    if (e4__stack_depth(task) < 4) {
        /* FIXME: Underflow. */
    }

    length = (unsigned long)e4__stack_pop(task);
    buf = (const char*)e4__stack_pop(task);

    /* FIXME: Correctly handle double cell integers! */
    e4__stack_pop(task);

    initial = (unsigned long)e4__stack_pop(task);
    initial *= (unsigned long)task->base;

    result = 0;
    consumed = e4__mem_number(buf, length, (unsigned long)task->base, 0,
            &result);
    result += initial;

    e4__stack_push(task, (e4__cell)result);

    /* FIXME: Actually handle double cell integers. */
    e4__stack_push(task, (e4__cell)0);

    e4__stack_push(task, (e4__cell)(buf + consumed));
    e4__stack_push(task, (e4__cell)(length - consumed));

    e4__builtin_RET(task, NULL);
}

void e4__builtin_LIT(struct e4__task *task, void *user)
{
    *task->sp-- = e4__DEREF2(++task->rp);
    task->ip = e4__DEREF(task->rp) + 1;
}

void e4__builtin_SKIP(struct e4__task *task, void *user)
{
    task->ip = e4__DEREF(++task->rp) + 1;
}

void e4__builtin_WORD(struct e4__task *task, void *user)
{
    register char delim;
    register const char *word;
    register char clamped_length;
    unsigned long length;

    /* FIXME: Should task struct fields just be used here rather than
       the C stack API? */
    if (e4__stack_depth(task) < 1) {
        /* FIXME: Underflow. */
    }

    /* Parse word. */
    delim = (long int)e4__stack_pop(task);
    word = (const char *)task->io_src.buffer + (unsigned long)task->io_src.in;

    if ((unsigned long)task->io_src.length > (unsigned long)task->io_src.in)
        length = (unsigned long)task->io_src.length -
                (unsigned long)task->io_src.in;
    else
        length = 0;

    word = e4__mem_parse(word, delim, length, e4__F_SKIP_LEADING, &length);

    /* XXX: Ambiguous condition. We take only the first 255 bytes of
       a parsed string that is longer than this, but we advance >IN
       to the actual end of the word (beyond the encountered delim),
       as per the specified delim. */

    /* Write word to HERE. */
    /* FIXME: Check that there is space in HERE before writing
       to it. */
    clamped_length = length < 256 ? (unsigned char)length : 255;
    *((unsigned char*)task->here) = clamped_length;
    memcpy((unsigned char*)task->here + 1, word, clamped_length);

    /* Update offset and push result. */
    task->io_src.in = (e4__cell)(word + length + 1 -
            (const char*)task->io_src.buffer);
    e4__stack_push(task, task->here);

    e4__builtin_RET(task, NULL);
}
