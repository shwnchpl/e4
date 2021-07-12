#include "e4-debug.h"

#define _e4_BUILTIN_DECL(name, func)    {name, sizeof(name) - 1, func}
/* TODO: Consider putting builtin footers into some other memory that
   doesn't even need to be loaded into the user provided space.
   TODO: Add some sort of e4__builtin_thunk which basically just calls
   e4__execute on whatever is pointed to by the *user field. This could
   allow for builtins implemented in Forth that don't need to be
   compiled or have their code components live in the user dictionary. */
struct e4__builtin e4__BUILTIN_TABLE[] =
{
    /* FIXME: Don't expose abort. We need quit instead. */
    _e4_BUILTIN_DECL("abort", e4__builtin_abort),
    _e4_BUILTIN_DECL("execute", e4__execute),
    _e4_BUILTIN_DECL("return", e4__builtin_return),
    _e4_BUILTIN_DECL("skip", e4__builtin_skip),
    {NULL}
};
#undef _e4_BUILTIN_DECL

void e4__builtin_abort(struct e4__task *task, void *next)
{
    register int i;
    static const void *RETURN[] =
    {
        e4__builtin_return
    };

    printf("ABORTING ALL EXECUTION...\n");
    for (i = 1; &task->rp[i] <= task->r0; ++i)
        task->rp[i] = RETURN;
    task->ip = e4__DEREF(++task->rp);
}

void e4__builtin_return(struct e4__task *task, void *next)
{
    task->ip = e4__DEREF(++task->rp);
    printf("Returning to %p\n", (void*)task->ip);
}

void e4__builtin_skip(struct e4__task *task, void *next)
{
    task->ip = e4__DEREF(++task->rp) + 1;
}
