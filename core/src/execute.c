#include "e4-debug.h"

void e4__execute(struct e4__task *state, void *next)
{
    void **code = next;
    void (*entry)(struct e4__task *, void *) = *code;

    e4__DEREF(state->rp--) = state->ip + 1;
    entry(state, code + 1);
}

void e4__execute_threaded(struct e4__task *state, void *next)
{
    register int depth = 1;

    state->ip = next;

    printf("In e4__execute_threaded...: %p\n", e4__execute_threaded);

    while (depth)
    {
        printf("Branching (%d) to %p, return addr: %p\n",
            depth, e4__DEREF2(state->ip), e4__DEREF(state->ip) + 1);
        if (e4__DEREF2(state->ip) == (e4__void)e4__execute_threaded) {
            depth += 1;
            e4__DEREF(state->rp--) = state->ip + 1;
            state->ip = e4__DEREF(state->ip) + 1;
        }
        else if (e4__DEREF(state->ip) == (e4__void)e4__builtin_return) {
            depth -= 1;
            state->ip = e4__DEREF(++state->rp);
            printf("Inline returning to %p\n", state->ip);
        } else {
            printf("Executing...%p\n", state->ip);
            e4__execute(state, e4__DEREF(state->ip));
        }
    }
}
