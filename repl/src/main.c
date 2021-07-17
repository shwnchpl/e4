#include "e4.h"
#include <stdio.h>
#include <string.h>

#include "e4-debug.h"

static void print_anything(struct e4__task *task, void *user);

const void *PRINT_HELLO[] =
{
    print_anything,
    (void*)"What's up?"
};

const void *HELLO1[] =
{
    e4__execute_threaded,
    NULL,
    PRINT_HELLO,
    e4__builtin_RET
};

const void *HELLO2[] =
{
    e4__execute_threaded,
    NULL,
    HELLO1,
    &e4__BUILTIN_SKIP,
    HELLO1,
    HELLO1,
    &e4__BUILTIN_ABORT,
    HELLO1,
    e4__builtin_RET
};

const void *PUSH_NUM[] =
{
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_LIT,
    (void*)0x12345,
    e4__builtin_RET
};

void print_anything(struct e4__task *task, void *user)
{
    printf("\nHello\n");
    printf("Payload: %s\n", *((char**)user));

    e4__builtin_RET(task, NULL);
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
    do {
        void *tmp;

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
    } while (0);
    */

    do {
        e4__cell cells[6];
        unsigned long depth0, depth1;

        printf("\nStack and return stack:\n");

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

    do {
        char len;
        char *word;

        printf("\nParsing some words now.\n\n");

        word = e4__mem_word(task, ' ', "   foo bar bas");
        len = *word++;
        printf("Got (%d): %.*s\n", len, len, word);

        word = e4__mem_word(task, ',', "   foo, bar,, bas");
        len = *word++;
        printf("Got (%d): %.*s\n", len, len, word);

        word = e4__mem_word(task, ' ', "         ");
        len = *word++;
        printf("Got (%d): %.*s\n", len, len, word);

        word = e4__mem_word(task, ' ', "  foo\nbar");
        len = *word++;
        printf("Got (%d): %.*s\n", len, len, word);

        word = e4__mem_word(task, ' ', "  foo.bar");
        len = *word++;
        printf("Got (%d): %.*s\n", len, len, word);
    } while (0);

    e4__execute(task, PUSH_NUM);
    printf("STACK TOP: %p\n", e4__stack_pop(task));

    return 0;
}
