#include "e4-debug.h"
#include "e4.h"
#include "e4-builtin.h"
#include "e4-task.h"

/* FIXME: Let this support "compile time" flags, etc. */
#define _e4__BUILTIN_DELC()     \
    _e4__BUILTIN_PROC(ABORT)    \
    _e4__BUILTIN_PROC(LIT)      \
    _e4__BUILTIN_PROC(RET)      \
    _e4__BUILTIN_PROC(SKIP)     \
    _e4__BUILTIN_PROC(WORD)

#define _e4__BUILTIN_PROC(p)    \
    const struct e4__execute_token e4__BUILTIN_##p = {e4__builtin_##p, NULL};

_e4__BUILTIN_DELC();

#undef _e4__BUILTIN_PROC
#define _e4__BUILTIN_PROC(p)    \
    {#p, sizeof(#p), &e4__BUILTIN_##p},

const struct e4__builtin e4__BUILTIN_TABLE[] =
{
    _e4__BUILTIN_DELC()
    {NULL},
};

#undef _e4_BUILTIN_PROC
#undef _e4_BUILTIN_DECL

/* TODO: Add some sort of e4__builtin_thunk which basically just calls
   e4__execute on whatever is pointed to by the *user field. This could
   allow for builtins implemented in Forth that don't need to be
   compiled or have their code components live in the user dictionary. */
/* FIXME: Don't expose abort. We need quit instead? */

void e4__builtin_ABORT(struct e4__task *task, void *user)
{
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

void e4__builtin_RET(struct e4__task *task, void *user)
{
    task->ip = e4__DEREF(++task->rp);
    printf("Returning to %p\n", (void*)task->ip);
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
