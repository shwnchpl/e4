#include "e4-debug.h"

void e4__builtin_abort(struct e4__task *state, void *next)
{
    register int i;
    static const void *RETURN[] =
    {
        e4__builtin_return
    };

    printf("ABORTING ALL EXECUTION...\n");
    for (i = -1; &state->rp[i] >= state->r0; --i)
        state->rp[i] = RETURN;
    state->ip = e4__DEREF(--state->rp);
}

void e4__builtin_return(struct e4__task *state, void *next)
{
    state->ip = e4__DEREF(--state->rp);
    printf("Returning to %p\n", (void*)state->ip);
}

void e4__builtin_skip(struct e4__task *state, void *next)
{
    state->ip = e4__DEREF(--state->rp) + 1;
}
