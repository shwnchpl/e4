#include "e4.h"
#include <stdio.h>
#include <string.h>

/* FIXME: Remove this. Uncomment for internal tests. */
#if 0
#include "../../core/src/e4-task.h"
#endif

int main(void)
{
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
