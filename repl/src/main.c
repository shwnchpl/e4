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

    printf("Attempting to call key without defining it returns: %d\n",
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

    printf("\nstrncasecmp(foobar, fooba, 5): %d\n",
            e4__mem_strncasecmp("foobar", "fooba", 5));

    printf("strncasecmp(foobar, fooba, 6): %d\n",
            e4__mem_strncasecmp("foobar", "fooba", 6));

    printf("strncasecmp(FOObaR, fooBa, 6): %d\n",
            e4__mem_strncasecmp("FOObaR", "fooBa", 6));

    printf("strncasecmp(FOOBAR, foobar, 90): %d\n",
            e4__mem_strncasecmp("FOOBAR", "foobar", 90));

    do {
        e4__usize num = 0;
        e4__usize parsed;

        printf("\nNUMERIC PARSING\n");

        #define _test_NUM_FLAGS \
            e4__F_CHAR_LITERAL | e4__F_NEG_PREFIX | e4__F_BASE_PREFIX
        #define _test_NUM(n, base, flags)   \
            parsed = e4__mem_number(n, sizeof(n) - 1, base, flags, &num);   \
            printf("Parsed \"%s\" (base: %d, digits: %lu): %ld\n", n,  \
                    base, parsed, (long)num)
        _test_NUM("0xac", 10, _test_NUM_FLAGS);
        _test_NUM("ac", 16, _test_NUM_FLAGS);
        _test_NUM("ac", 5, _test_NUM_FLAGS);
        _test_NUM("32ac", 5, _test_NUM_FLAGS);
        _test_NUM("$ac", 5, _test_NUM_FLAGS);
        _test_NUM("-$ac", 5, _test_NUM_FLAGS);
        _test_NUM("%011011", 5, _test_NUM_FLAGS);
        _test_NUM("0b011011", 5, _test_NUM_FLAGS);
        _test_NUM("0o777xx", 5, _test_NUM_FLAGS);
        _test_NUM("#777   ", 5, _test_NUM_FLAGS);
        _test_NUM("'a'jieoag", 5, _test_NUM_FLAGS);
        _test_NUM("zOo", 36, _test_NUM_FLAGS);
        _test_NUM("zOo", 99, _test_NUM_FLAGS);
        _test_NUM("000", 10, 0);
        _test_NUM("-5", 10, 0);
        _test_NUM("$5", 10, 0);
        _test_NUM("'5'", 10, 0);
        #undef _test_NUM
        #undef _test_NUM_FLAGS
    } while (0);

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

    do {
        char buf[30];
        char *res;

        printf("\nTesting numeric formatting:\n");

        #define _test_FMTDIGIT(n, b, f) \
            res = e4__num_format(n, b, f, buf, sizeof(buf));    \
            printf("For %d (%u) base %u flags %x got: %.*s\n", \
                n, n, b, f, (int)(&buf[32] - res), res)
        _test_FMTDIGIT(531, 10, 0);
        _test_FMTDIGIT(0xf3, 16, 0);
        _test_FMTDIGIT(-0xf3, 16, 0);
        _test_FMTDIGIT(-0xf3, 16, e4__F_SIGNED);
        _test_FMTDIGIT(46655, 36, e4__F_SIGNED);
        _test_FMTDIGIT(-50, 2, e4__F_SIGNED);
        _test_FMTDIGIT(0, 2, e4__F_SIGNED);
        #undef _test_FMTDIGIT
    } while (0);

    printf("\n5 is positive, %ld is negative (%u)\n",
            e4__USIZE_NEGATE((e4__usize)5),
            e4__USIZE_IS_NEGATIVE((e4__usize)-5));

    printf("\n-10/3 = %ld, 10/-3 = %ld -10/-3 = %ld, 10/3 = %ld\n",
            e4__num_sdiv((e4__usize)-10, (e4__usize)3),
            e4__num_sdiv((e4__usize)10, (e4__usize)-3),
            e4__num_sdiv((e4__usize)-10, (e4__usize)-3),
            e4__num_sdiv((e4__usize)10, (e4__usize)3));

#if 0 /* FIXME: Remove this code. */
    do {
        printf("\nTesting stack manipulation builtin utils:\n");
        #define _test_PUSH3()   \
            do {    \
                e4__stack_push(task, (e4__cell)1);  \
                e4__stack_push(task, (e4__cell)2);  \
                e4__stack_push(task, (e4__cell)3);  \
            } while (0)
        #define _test_DUMP_STACK()  \
                do {    \
                    e4__cell s = task->s0; \
                    printf("(%lu) ", e4__stack_depth(task));    \
                    while (task->sp < s)  \
                        printf("%lu ", (e4__usize)e4__DEREF(s--));   \
                    e4__stack_clear(task);  \
                    printf("\n");   \
                } while (0)
        #define _test_STACK_WORD(w) \
            do {    \
                _test_PUSH3();  \
                e4__stack_##w(task);    \
                printf("Pushed 1 2 3. After %s, stack dump: ", #w); \
                _test_DUMP_STACK(); \
            } while (0)
        _test_STACK_WORD(clear);
        _test_STACK_WORD(drop);
        _test_STACK_WORD(dup);
        _test_STACK_WORD(over);
        _test_STACK_WORD(rot);
        _test_STACK_WORD(swap);
        _test_STACK_WORD(tuck);

        _test_PUSH3();
        e4__stack_push(task, (e4__cell)4);
        e4__stack_push(task, (e4__cell)5);
        e4__stack_push(task, (e4__cell)4);
        printf("Pushed 1 2 3 4 5 4. After roll, stack dump: ");
        e4__stack_roll(task);
        _test_DUMP_STACK();
        #undef _test_STACK_WORD
        #undef _test_DUMP_STACK
        #undef _test_PUSH3
    } while (0);
#endif

    return 0;
}
