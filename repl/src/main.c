#include "e4.h"
#include <stdio.h>
#include <string.h>

#include "e4-debug.h"

/* FIXME: This is cool, but it doesn't actually work. longjmp down
   the stackframe is undefined behavior since longjmp out blows the
   stack. */

static void print_anything(struct e4__task *task, void *next);

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

void print_anything(struct e4__task *task, void *next)
{
    printf("\nHello\n");
    printf("Payload: %s\n", *((char**)next));

    e4__builtin_return(task, next);
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

    e4__task_load_builtins(task);

    /* TODO: Replace this test code.
    tmp = task->here;
    task->here = e4__dict_entry(task->here, task->dict,
            "test-func", 9, NULL, NULL, 0);
    task->dict = tmp;

    tmp = task->here;
    task->here = e4__dict_entry(task->here, task->dict,
            "test-func2", 10, NULL, NULL, 0);
    task->dict = tmp;

    tmp = task->here;
    task->here = e4__dict_entry(task->here, task->dict,
            "test-func3", 10, NULL, NULL, 0);
    task->dict = tmp;

    printf("%p : %p\n", task->here, e4__dict_lookup(task->dict, "test-func3",
                strlen("test-func3"))->footer->data);
    */

    do {
        e4__cell cells[6];
        unsigned long depth0, depth1;

        printf("Stack and return stack:\n");

        e4__stack_push(task, (void*)2);
        e4__stack_push(task, (void*)4);
        e4__stack_push(task, (void*)8);
        e4__stack_push(task, (void*)16);
        e4__stack_push(task, (void*)32);

        depth0 = e4__stack_depth(task);
        cells[0] = e4__stack_peek(task);
        cells[1] = e4__stack_pop(task);
        cells[2] = e4__stack_pop(task);
        cells[3] = e4__stack_pop(task);
        cells[4] = e4__stack_pop(task);
        cells[5] = e4__stack_pop(task);
        depth1 = e4__stack_depth(task);

        printf("(start-depth: %lu) (peek: %p) %p %p %p %p %p (end-depth: %lu)\n",
            depth0,
            cells[0],
            cells[1],
            cells[2],
            cells[3],
            cells[4],
            cells[5],
            depth1);

        e4__stack_rpush(task, (void*)3);
        e4__stack_rpush(task, (void*)5);
        e4__stack_rpush(task, (void*)9);
        e4__stack_rpush(task, (void*)17);
        e4__stack_rpush(task, (void*)33);

        cells[0] = e4__stack_rpeek(task);
        cells[1] = e4__stack_rpop(task);
        cells[2] = e4__stack_rpop(task);
        cells[3] = e4__stack_rpop(task);
        cells[4] = e4__stack_rpop(task);
        cells[5] = e4__stack_rpop(task);

        printf("(peek: %p) %p %p %p %p %p\n",
            cells[0],
            cells[1],
            cells[2],
            cells[3],
            cells[4],
            cells[5]);
    } while (0);

    printf("Attempting to call key without defining it returns: %d\n",
            e4__io_key(task, NULL));

    return 0;
}
