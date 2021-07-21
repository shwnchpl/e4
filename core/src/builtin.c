#include "e4-debug.h"
#include "e4.h"
#include "e4-task.h"

/* FIXME: Let this support "compile time" flags, etc. */
#define _e4__BUILTIN_DECL() \
    _e4__BUILTIN_PROC_FIRST(RET)  \
    _e4__BUILTIN_PROC(ABORT)    \
    _e4__BUILTIN_PROC(LIT)  \
    _e4__BUILTIN_PROC(SKIP) \
    _e4__BUILTIN_PROC(WORD)

/* Declare builtin functions. */
#define _e4__BUILTIN_PROC_FIRST(w)  \
    void e4__builtin_##w(struct e4__task *task, void *user);
#define _e4__BUILTIN_PROC(w)    \
    static void e4__builtin_##w(struct e4__task *task, void *user);

_e4__BUILTIN_DECL();

#undef _e4__BUILTIN_PROC_FIRST
#undef _e4__BUILTIN_PROC

/* Define builtin header table. */
#define _e4__BUILTIN_PROC_HEADER(w, link) \
    {   \
        link,   \
        (struct e4__execute_token *)&e4__BUILTIN_XT[e4__B_##w],     \
        e4__F_BUILTIN,  \
        sizeof(#w) - 1, \
        #w, \
    },
#define _e4__BUILTIN_PROC_FIRST(w)  \
    _e4__BUILTIN_PROC_HEADER(w, NULL)
#define _e4__BUILTIN_PROC(w)  \
    _e4__BUILTIN_PROC_HEADER(w, \
            (struct e4__dict_header *)&e4__BUILTIN_HEADER[e4__B_##w - 1])

const struct e4__dict_header e4__BUILTIN_HEADER[e4__BUILTIN_COUNT] =
{
    _e4__BUILTIN_DECL()
};

#undef _e4__BUILTIN_PROC_HEADER
#undef _e4__BUILTIN_PROC_FIRST
#undef _e4__BUILTIN_PROC

/* Define builtin execution token table. */
#define _e4__BUILTIN_PROC_FIRST(w)  _e4__BUILTIN_PROC(w)
#define _e4__BUILTIN_PROC(w)  \
    {e4__builtin_##w, NULL},

const struct e4__execute_token e4__BUILTIN_XT[e4__BUILTIN_COUNT] =
{
    _e4__BUILTIN_DECL()
};

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
    register char *word;

    /* FIXME: Should task struct fields just be used here rather than
       the C stack API? */
    if (e4__stack_depth(task) < 1) {
        /* FIXME: Underflow. */
    }

    /* FIXME: Check for empty parse area. In reality, e4__mem_word
       should probably take some kind of length. */
    /* FIXME: Verify that WORD should update >IN. */
    /* FIXME: Make this work with leading delimiters. This will
       probably require a total refactor of e4__mem_word. */

    delim = (long int)e4__stack_pop(task);
    word = e4__mem_word(task, delim,
            (const char*)task->io_src.buffer + (unsigned long)task->io_src.in);
    task->io_src.in = (e4__cell)((const char*)task->io_src.in + *word + 1);
    e4__stack_push(task, word);

    e4__builtin_RET(task, NULL);
}
