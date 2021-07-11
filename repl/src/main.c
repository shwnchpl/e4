#include "e4-debug.h"
#include <stdio.h>

static void print_anything(struct e4__task *state, void *next);

void *PRINT_HELLO[] =
{
    print_anything,
    (void*)"What's up?"
};

void *ABORT[] =
{
    e4__builtin_abort,
};

void *SKIP[] =
{
    e4__builtin_skip,
};

void *HELLO1[] =
{
    e4__execute_threaded,
    PRINT_HELLO,
    e4__builtin_return
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
    e4__builtin_return
};

void print_anything(struct e4__task *state, void *next)
{
    printf("\nHello\n");
    printf("Payload: %s\n", *((char**)next));

    e4__builtin_return(state, next);
}

int main(void)
{
    static unsigned char buffer[4096];

    struct e4__task *task;

    printf("Trying print hello...\n");

    task = e4__task_create(buffer, sizeof(buffer));

    /* e4__execute_threaded(task, &HELLO2[1]); */
    e4__execute(task, HELLO2);
    /* e4__execute(task, PRINT_HELLO); */

    return 0;
}
