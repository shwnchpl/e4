#include "e4-debug.h"

void e4__execute(struct e4__vm_state *state, void *next)
{
    register void **routine = next;
    register void (*entry)(struct e4__vm_state *, void *) = *routine;

    *state->r++ = state->ip + 1;
    entry(state, routine + 1);
}

void e4__return(struct e4__vm_state *state, void *next);

void e4__execute_threaded(struct e4__vm_state *state, void *next)
{
    register int depth = 1;

    state->ip = next;

    printf("In e4__execute_threaded...: %p\n", e4__execute_threaded);

    while (depth)
    {
        printf("Branching (%d) to %p, return addr: %p\n", depth, **state->ip, *state->ip + 1);
        if (**state->ip == (void*)e4__execute_threaded) {
            depth += 1;
            *state->r++ = state->ip + 1;
            state->ip = (void*)(*state->ip + 1);
        }
        else if (*state->ip == (void*)e4__return) {
            depth -= 1;
            state->ip = *--state->r;
            printf("Inline returning to %p\n", state->ip);
        } else
            e4__execute(state, *state->ip);
    }
}

void e4__skip(struct e4__vm_state *state, void *next)
{
    state->ip = *--state->r + 1;
}

void e4__return(struct e4__vm_state *state, void *next)
{
    state->ip = *--state->r;
    printf("Returning to %p\n", state->ip);
}

void e4__abort(struct e4__vm_state *state, void *next)
{
    register int i;
    static const void (**RETURN__[])(struct e4__vm_state *, void *) =
    {
        RETURN_
    };

    printf("ABORTING ALL EXECUTION...\n");
    for (i = -1; &state->r[i] >= state->r_base; --i)
        state->r[i] = (void*)RETURN__;
    state->ip = *--state->r;
}
