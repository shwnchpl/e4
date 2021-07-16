#include "e4-debug.h"
#include "e4.h"
#include "e4-builtin.h"
#include "e4-task.h"

/* FIXME: Let this support "compile time" flags, etc. */
#define _e4__BUILTIN_DELC()     \
    _e4__BUILTIN_PROC(ABORT)    \
    _e4__BUILTIN_PROC(LIT)      \
    _e4__BUILTIN_PROC(RET)      \
    _e4__BUILTIN_PROC(SKIP)

#define _e4__BUILTIN_PROC(p)    \
    const struct e4__dict_footer e4__BUILTIN_##p = {NULL, e4__builtin_##p,};

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

void e4__builtin_ABORT(struct e4__task *task, void *next)
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

void e4__builtin_LIT(struct e4__task *task, void *next)
{
    *task->sp-- = e4__DEREF2(++task->rp);
    task->ip = e4__DEREF(task->rp) + 1;
}

void e4__builtin_RET(struct e4__task *task, void *next)
{
    task->ip = e4__DEREF(++task->rp);
    printf("Returning to %p\n", (void*)task->ip);
}

void e4__builtin_SKIP(struct e4__task *task, void *next)
{
    task->ip = e4__DEREF(++task->rp) + 1;
}
