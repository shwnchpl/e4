#include "e4.h"
#include <stdio.h>
#include <string.h>

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
