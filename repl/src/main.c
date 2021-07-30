#include "e4.h"
#include <stdio.h>
#include <string.h>

#include "e4-debug.h"

/* FIXME: Remove this. Uncomment for internal tests. */
#if 0
#include "../../core/src/e4-task.h"
#endif

static void print_anything(struct e4__task *task, void *user);

const void *PRINT_HELLO[] =
{
    print_anything,
    (void *)"What's up?"
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
    &e4__BUILTIN_XT[e4__B_SKIP],
    HELLO1,
    HELLO1,
    &e4__BUILTIN_XT[e4__B_ABORT],
    HELLO1,
    e4__builtin_RET
};

const void *PUSH_NUM[] =
{
    e4__execute_threaded,
    NULL,
    &e4__BUILTIN_XT[e4__B_LIT],
    (void *)0x12345,
    e4__builtin_RET
};

void print_anything(struct e4__task *task, void *user)
{
    printf("\nHello\n");
    printf("Payload: %s\n", *((char **)user));

    e4__builtin_RET(task, NULL);
}

e4__usize printf_type(void *user, const char *buf, e4__usize n)
{
    printf("%.*s", (int)n, buf);
    return e4__E_OK;
}

int main(void)
{
    static e4__u8 buffer[4096];

    struct e4__task *task;

    printf("Trying print hello...\n");

    task = e4__task_create(buffer, sizeof(buffer));

    /* e4__execute_threaded(task, &HELLO2[1]); */
    e4__execute(task, HELLO2);
    /* e4__execute(task, PRINT_HELLO); */

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
                "TEST-funC3", 10, NULL, NULL, 0);
        task->dict = tmp;

        printf("%p : %p\n", task->here,
                e4__dict_lookup(task->dict, "test-func3",
                    strlen("test-func3"))->xt->data);

        printf("Looking up LIT (%p) : %p\n",
                &e4__BUILTIN_XT[e4__B_LIT],
                e4__dict_lookup(task->dict, "lit", strlen("lit"))->xt);
        printf("Looking up FLOMBOM : %p\n",
                e4__dict_lookup(task->dict, "flombom", strlen("flombom")));
    } while (0);
    */

    do {
        e4__cell cells[6];
        e4__usize depth0, depth1;

        printf("\nStack and return stack:\n");

        e4__stack_push(task, (void *)2);
        e4__stack_push(task, (void *)4);
        e4__stack_push(task, (void *)8);
        e4__stack_push(task, (void *)16);
        e4__stack_push(task, (void *)32);

        depth0 = e4__stack_depth(task);
        cells[0] = e4__stack_peek(task);
        cells[1] = e4__stack_pop(task);
        cells[2] = e4__stack_pop(task);
        cells[3] = e4__stack_pop(task);
        cells[4] = e4__stack_pop(task);
        cells[5] = e4__stack_pop(task);
        depth1 = e4__stack_depth(task);

        printf("(start-depth: %lu) (peek: %p) %p %p %p %p %p "
            "(end-depth: %lu)\n",
            depth0,
            cells[0],
            cells[1],
            cells[2],
            cells[3],
            cells[4],
            cells[5],
            depth1);

        e4__stack_rpush(task, (void *)3);
        e4__stack_rpush(task, (void *)5);
        e4__stack_rpush(task, (void *)9);
        e4__stack_rpush(task, (void *)17);
        e4__stack_rpush(task, (void *)33);

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

    printf("Attempting to call key without defining it returns: %lu\n",
            e4__io_key(task, NULL));

    do {
        e4__usize len;
        const char *word;

        printf("\nParsing some words now.\n\n");

        #define _test_PARSE(s, d, f)   \
            word = e4__mem_parse(s, d, strlen(s), f, &len); \
            printf("Got (%lu): %.*s\n", len, (int)len, word);
        _test_PARSE("   foo bar bas", ' ', e4__F_SKIP_LEADING);
        _test_PARSE("   foo, bar,, bas", ',', e4__F_SKIP_LEADING);
        _test_PARSE("         ", ' ', e4__F_SKIP_LEADING);
        _test_PARSE("  foo\nbar", ' ', e4__F_SKIP_LEADING);
        _test_PARSE("  foo.bar", ' ', e4__F_SKIP_LEADING);
        _test_PARSE("  foo.bar", ' ', 0);
        #undef _test_PARSE
    } while (0);

    e4__execute(task, PUSH_NUM);
    printf("STACK TOP: %p\n", e4__stack_pop(task));

/* FIXME: Remove this. Uncomment for WORD test. */
#if 0
    do {
        static const void *RUN_WORD[] = {
            e4__execute_threaded,
            NULL,
            &e4__BUILTIN_XT[e4__B_LIT],
            (void *)' ',
            &e4__BUILTIN_XT[e4__B_WORD],
            e4__builtin_RET
        };
        static const char *foo = "     foo   bar,,bas   \n";
        struct e4__io_src old_io_src = task->io_src;
        const char *res;
        char len;

        task->io_src.buffer = (e4__cell)foo;
        task->io_src.in = 0;
        task->io_src.length = (e4__cell)strlen(foo);

        do {
            e4__execute(task, RUN_WORD);

            res = (const char *)e4__stack_pop(task);
            len = *res++;
            printf("\nExecuted WORD (in: %p, len: %p), got (%d): %.*s\n",
                    task->io_src.in, task->io_src.length, len, len, res);
        } while (task->io_src.in < task->io_src.length);

        task->io_src = old_io_src;
    } while (0);
#endif

#if 0 /* FIXME: Removed/update this test. */
    do {
        printf("\nTesting >NUMBER builtin:\n");

        #define _test_GTNUMBER(s, b, n)    \
            do {    \
                long result;    \
                const char *remaining;  \
                int rcount; \
                e4__usize old_base = (e4__usize)task->base; \
                                                        \
                task->base = (e4__cell)b;   \
                e4__stack_push(task, (e4__cell)n);  \
                e4__stack_push(task, (e4__cell)0);  \
                e4__stack_push(task, (e4__cell)s);  \
                e4__stack_push(task, (e4__cell)(sizeof(s) - 1));    \
                e4__execute(task, (void *)&e4__BUILTIN_XT[e4__B_GTNUMBER]);  \
                rcount = (e4__usize)e4__stack_pop(task);    \
                remaining = (const char *)e4__stack_pop(task);  \
                e4__stack_pop(task);    \
                result = (e4__usize)e4__stack_pop(task);    \
                printf("TRIED \"%s\". GOT %ld (%d remaining: \"%.*s\")\n",   \
                    s, result, rcount, rcount, remaining);  \
                task->base = (e4__cell)old_base;    \
            } while (0)
        _test_GTNUMBER("42", 10, 0);
        _test_GTNUMBER("42", 16, 0);
        _test_GTNUMBER("z00 f", 36, 0);
        _test_GTNUMBER("-10", 10, 50);
        _test_GTNUMBER("1000", 2, 4);
        _test_GTNUMBER("'a'", 10, 0);
        #undef _test_GTNUMBER
    } while (0);
#endif

    return 0;
}
