#include "e4-debug.h"
#include <stdio.h>

static void print_anything(struct e4__vm_state *state, void *next);

void *PRINT_HELLO[] =
{
    print_anything,
    (void*)"What's up?"
};

void *ABORT[] =
{
    e4__abort,
};

void *SKIP[] =
{
    e4__skip,
};

void *HELLO1[] =
{
    e4__execute_threaded,
    PRINT_HELLO,
    RETURN_
};

void *HELLO2[] =
{
    e4__execute_threaded,
    HELLO1,
    SKIP,
    HELLO1,
    HELLO1,
    ABORT,
    HELLO1,
    RETURN_
};

void (***RETURN_STACK[32])(struct e4__vm_state *, void *) = {0,};

void print_anything(struct e4__vm_state *state, void *next)
{
    printf("\nHello\n");
    printf("Payload: %s\n", *((char**)next));
    state->ip = *--state->r;
    printf("Returning to %p\n", state->ip);

    return;
}

int main(void)
{
    struct e4__vm_state state;

    printf("Trying print hello...\n");

    state.ip = NULL;
    state.r = RETURN_STACK;
    state.r_base = RETURN_STACK;

    /* e4__execute_threaded(&state, &HELLO2[1]); */
    e4__execute(&state, HELLO2);
    /* e4__execute(&state, PRINT_HELLO); */

    return 0;
}
