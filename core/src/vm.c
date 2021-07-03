#include "e4-debug.h"

void e4__execute(struct e4__vm_state *state, void *next)
{
    void **routine = next;
    void (*entry)(struct e4__vm_state *, void *) = *routine;
    entry(state, routine + 1);
}

void e4__execute_threaded(struct e4__vm_state *state, void *next)
{
    state->ip = next;

    printf("In e4__execute_threaded...: %p\n", e4__execute_threaded);

    while (*state->ip != RETURN_)
    {
        *state->r++ = state->ip + 1;
        printf("Branching to %p, return addr: %p\n", **state->ip, *state->ip + 1);
        e4__execute(state, *state->ip);
    }

    state->ip = *(--state->r);
}

void e4__abort(struct e4__vm_state *state, void *next);

void e4__skip(struct e4__vm_state *state, void *next)
{
    state->ip = *--state->r + 1;
}

void (**RETURN__[])(struct e4__vm_state *, void *) =
{
    RETURN_
};

void e4__abort(struct e4__vm_state *state, void *next)
{
    int i;
    printf("ABORTING ALL EXECUTION...\n");
    for (i = -1; &state->r[i] >= state->r_base; --i)
        state->r[i] = RETURN__;
    state->ip = *--state->r;
}
