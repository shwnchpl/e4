#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

#include <string.h>

static void e4t__test_kernel_dict(void)
{
    struct e4__task *task = e4t__transient_task();

    #define _d(s, c)    e4__dict_entry(task, s, sizeof(s) - 1, 0, c, NULL)
    #define _l(s)       e4__dict_lookup(task, s, sizeof(s) - 1)
    #define _f(s)       e4__dict_forget(task, s, sizeof(s) - 1)

    _d("first-entry", (void *)0xabcde);
    e4t__ASSERT(_l("first-entry"));
    e4t__ASSERT_EQ((e4__usize)_l("first-entry")->xt->code, 0xabcde);

    _d("second-entry", (void *)0x12345);
    e4t__ASSERT(_l("second-entry"));
    e4t__ASSERT_EQ((e4__usize)_l("second-entry")->xt->code, 0x12345);

    e4t__ASSERT(_l("first-entry"));
    e4t__ASSERT_EQ((e4__usize)_l("first-entry")->xt->code, 0xabcde);
    e4t__ASSERT_EQ((e4__usize)_l("FIRST-entry")->xt->code, 0xabcde);

    /* Can't forget a word that isn't present or a builtin. */
    e4t__ASSERT_EQ((e4__usize)_l("not-present"), (e4__usize)NULL);
    e4t__ASSERT_EQ(_f("not-present"), e4__E_INVFORGET);
    e4t__ASSERT(_l("dup"));
    e4t__ASSERT_EQ(_f("dup"), e4__E_INVFORGET);

    /* FORGET forgets all words defined after a word. */
    e4t__ASSERT_OK(_f("first-entry"));
    e4t__ASSERT_EQ((e4__usize)_l("first-entry"), (e4__usize)NULL);
    e4t__ASSERT_EQ((e4__usize)_l("second-entry"), (e4__usize)NULL);

    #undef _f
    #undef _l
    #undef _d
}

static void e4t__test_kernel_exceptions_da(struct e4__task *task, void *user);

static void e4t__test_kernel_exceptions(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Install test "drop abort" words. */
    e4__dict_entry(task, "dropfail", 8, 0, e4t__test_kernel_exceptions_da,
            (void *)e4__USIZE_NEGATE(e4__E_FAILURE));
    e4__dict_entry(task, "dropquit", 8, 0, e4t__test_kernel_exceptions_da,
            (void *)e4__USIZE_NEGATE(e4__E_QUIT));
    e4__dict_entry(task, "dropbye", 7, 0, e4t__test_kernel_exceptions_da,
            (void *)e4__USIZE_NEGATE(e4__E_BYE));

    /* Sanity check to make sure those words went in. */
    e4t__ASSERT(e4__dict_lookup(task, "dropfail", 8));
    e4t__ASSERT(e4__dict_lookup(task, "dropquit", 8));
    e4t__ASSERT(e4__dict_lookup(task, "dropbye", 7));

    /* Test that regular exceptions are caught and the stack is
       restored correctly. */
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3", -1, 0));
    e4t__ASSERT_OK(e4__evaluate(task, "4 dropfail", -1, 0));
    e4t__term_obuf_consume();
    e4t__ASSERT_OK(e4__evaluate(task, ".s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 2 3 4 ");

    /* Test that QUIT exception actually percolates all the way up and
       *doesn't* restore sp. */
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3", -1, 0));
    e4t__ASSERT_EQ(e4__evaluate(task, "4 dropquit", -1, 0), e4__E_QUIT);
    e4t__ASSERT_OK(e4__evaluate(task, ".s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<1> 1 ");

    /* Test that BYE exception actually percolates all the way up and
       *does* restore sp. */
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3", -1, 0));
    e4t__ASSERT_EQ(e4__evaluate(task, "4 dropbye", -1, 0), e4__E_BYE);
    e4t__ASSERT_OK(e4__evaluate(task, ".s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 1 2 3 ");
}

static void e4t__test_kernel_exceptions_da(struct e4__task *task, void *user)
{
    const e4__usize ex = (e4__usize)e4__DEREF(user);

    if (!e4__USIZE_IS_NEGATIVE(ex)) {
        /* Set up another exception handler and re-call this
           function. */
        struct e4__execute_token xt = {
            e4t__test_kernel_exceptions_da,
            (e4__cell)e4__USIZE_NEGATE(ex),
        };
        e4__exception_catch(task, &xt);
        return;
    }

    e4__stack_drop(task);
    e4__stack_drop(task);
    e4__stack_drop(task);

    e4__exception_throw(task, ex);

    /* This should be unreachable. */
    e4__builtin_RET(task, NULL);
}

static void e4t__test_kernel_evaluate(void)
{
    struct e4__task *task = e4t__transient_task();

    /* FIXME: Add more explicit evaluate tests. */

    /* Empty strings or strings with only delimiters should be
       no-ops. */
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);
    e4t__ASSERT_EQ(e4__evaluate(task, "", -1, 0), e4__E_OK);
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    e4t__ASSERT_EQ(e4__stack_depth(task), 0);
    e4t__ASSERT_EQ(e4__evaluate(task, " ", -1, 0), e4__E_OK);
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    /* Undefined words should generate an exception and clear
       the stack. */
    e4__stack_clear(task);
    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 4 5 rll", -1, 0),
            e4__E_UNDEFWORD);
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "");
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);
}

static void e4t__test_kernel_io(void)
{
    struct e4__task *task = e4t__transient_task();
    struct e4__io_func io_func = {0,};

    /* FIXME: Add more IO tests. */

    /* Clear all test related IO handlers. */
    e4__task_io_init(task, &io_func);

    /* Test that IO functions return the appropriate exception code
       when no handler is available. */
    e4t__ASSERT_EQ(e4__io_key(task, NULL), e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__io_accept(task, NULL, 0), e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__io_type(task, NULL, 0), e4__E_UNSUPPORTED);
}

static void e4t__test_kernel_numformat(void)
{
    static char buf[31] = {0,};

    #define _f(n, b, f) e4__num_format(n, b, f, buf, sizeof(buf) - 1)
    e4t__ASSERT_MATCH(_f(531, 10, 0), "531");
    e4t__ASSERT_MATCH(_f(0xf3, 16, 0), "f3");
    e4t__ASSERT_MATCH(_f(-0xf3, 16, 0), "ffffffffffffff0d");
    e4t__ASSERT_MATCH(_f(-0xf3, 16, e4__F_SIGNED), "-f3");
    e4t__ASSERT_MATCH(_f(46655, 36, e4__F_SIGNED), "zzz");
    e4t__ASSERT_MATCH(_f(-50, 2, e4__F_SIGNED), "-110010");
    e4t__ASSERT_MATCH(_f(0, 2, e4__F_SIGNED), "0");
    #undef _f
}

static void e4t__test_kernel_numparse(void)
{
    /* XXX: Parts of this test only work correctly on a system that
       represents negative numbers using two's complement. */
    e4__usize num = 0;

    #define _p(n, b, f) e4__mem_number(n, sizeof(n) - 1, b, f, &num)
    #define _f  (e4__F_CHAR_LITERAL | e4__F_NEG_PREFIX | e4__F_BASE_PREFIX)

    e4t__ASSERT_EQ(_p("0xac", 10, _f), 4);
    e4t__ASSERT_EQ(num, 172);

    e4t__ASSERT_EQ(_p("ac", 16, _f), 2);
    e4t__ASSERT_EQ(num, 172);

    e4t__ASSERT_EQ(_p("ac", 5, _f), 0);
    e4t__ASSERT_EQ(num, 172);

    e4t__ASSERT_EQ(_p("32ac", 5, _f), 2);
    e4t__ASSERT_EQ(num, 17);

    e4t__ASSERT_EQ(_p("$ac", 5, _f), 3);
    e4t__ASSERT_EQ(num, 172);

    e4t__ASSERT_EQ(_p("-$ac", 5, _f), 4);
    e4t__ASSERT_EQ(num, -172);

    e4t__ASSERT_EQ(_p("%011011", 5, _f), 7);
    e4t__ASSERT_EQ(num, 27);

    e4t__ASSERT_EQ(_p("0b011011", 5, _f), 8);
    e4t__ASSERT_EQ(num, 27);

    e4t__ASSERT_EQ(_p("0o777xx", 5, _f), 5);
    e4t__ASSERT_EQ(num, 511);

    e4t__ASSERT_EQ(_p("#777   ", 5, _f), 4);
    e4t__ASSERT_EQ(num, 777);

    e4t__ASSERT_EQ(_p("'a'jieoag", 5, _f), 3);
    e4t__ASSERT_EQ(num, 97);

    e4t__ASSERT_EQ(_p("zOo", 36, _f), 3);
    e4t__ASSERT_EQ(num, 46248);

    e4t__ASSERT_EQ(_p("zOo", 99, _f), 3);
    e4t__ASSERT_EQ(num, 46248);

    e4t__ASSERT_EQ(_p("000", 10, 0), 3);
    e4t__ASSERT_EQ(num, 0);

    e4t__ASSERT_EQ(_p("-5", 10, 0), 0);
    e4t__ASSERT_EQ(num, 0);

    e4t__ASSERT_EQ(_p("$5", 10, 0), 0);
    e4t__ASSERT_EQ(num, 0);

    e4t__ASSERT_EQ(_p("'5'", 10, 0), 0);
    e4t__ASSERT_EQ(num, 0);

    #undef _f
    #undef _p
}

static void e4t__test_kernel_math(void)
{
    /* XXX: Parts of this test only work correctly on a system that
       represents negative numbers using two's complement. */
    e4t__ASSERT_EQ(e4__USIZE_NEGATE((e4__usize)5), -5);
    e4t__ASSERT_EQ(e4__USIZE_IS_NEGATIVE((e4__usize)-5), 1);

    e4t__ASSERT_EQ(e4__num_sdiv((e4__usize)-10, (e4__usize)3), -3);
    e4t__ASSERT_EQ(e4__num_sdiv((e4__usize)10, (e4__usize)-3), -3);
    e4t__ASSERT_EQ(e4__num_sdiv((e4__usize)-10, (e4__usize)-3), 3);
    e4t__ASSERT_EQ(e4__num_sdiv((e4__usize)10, (e4__usize)3), 3);
}

static void e4t__test_kernel_mem_dict(void)
{
    char buf[4096] = {0,};
    void *here = buf;
    e4__usize wrote;
    struct e4__dict_header *dict = NULL;

    #define _d(s, c)    \
        do {    \
            wrote = e4__mem_dict_entry(here, dict, s, sizeof(s) - 1, 0, c,  \
                    NULL);  \
            dict = here;    \
            here += wrote;  \
        } while (0)
    #define _l(s)   e4__mem_dict_lookup(dict, s, sizeof(s) - 1)

    _d("first-entry", (void *)0xabcde);
    e4t__ASSERT_EQ((e4__usize)_l("first-entry"), (e4__usize)dict);
    e4t__ASSERT_EQ((e4__usize)_l("first-entry")->xt->code, 0xabcde);
    e4t__ASSERT_EQ((e4__usize)here % sizeof(e4__cell), 0);

    _d("second-entry", (void *)0x12345);
    e4t__ASSERT_EQ((e4__usize)_l("second-entry"), (e4__usize)dict);
    e4t__ASSERT_EQ((e4__usize)_l("second-entry")->xt->code, 0x12345);

    e4t__ASSERT(_l("first-entry"));
    e4t__ASSERT_EQ((e4__usize)_l("first-entry")->xt->code, 0xabcde);
    e4t__ASSERT_EQ((e4__usize)_l("FIRST-entry")->xt->code, 0xabcde);

    e4t__ASSERT_EQ((e4__usize)_l("not-present"), (e4__usize)NULL);

    #undef _l
    #undef _d
}

static e4__usize e4t__test_kernel_quit_accept(void *user, char *buf,
        e4__usize *n);

struct e4t__test_kernel_quit_data {
    struct e4__task *task;
    e4__usize step;
};

static void e4t__test_kernel_quit(void)
{
    struct e4__io_func old_iof;
    struct e4__task *task = e4t__transient_task();
    struct e4t__test_kernel_quit_data test_data = {
        task,
        0
    };
    struct e4__io_func iof = {
        &test_data,
        (void *)e4t__test_kernel_quit_accept,
    };

    /* Hack. Normally this kind of thing isn't safe unless you know
       the old handlers will be using their user pointer the same as
       you, or not at all. In this case, we know the previous TYPE
       handler doesn't use the user pointer, so this is safe. */
    e4__task_io_get(task, &old_iof);
    iof.type = old_iof.type;

    e4__task_io_init(task, &iof);
    e4__evaluate_quit(task);
}

static e4__usize e4t__test_kernel_quit_accept(void *user, char *buf, e4__usize *n)
{
    struct e4t__test_kernel_quit_data *test_data = user;

    #define _m(s)   \
        do { \
            memcpy(buf, s, sizeof(s) - 1);  \
            *n = sizeof(s) - 1; \
        } while (0)

    switch (test_data->step++) {
        case 0:
            e4t__term_obuf_consume();
            _m("1 2 3 4 .s clear");
            break;
        case 1:
            e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 2 3 4  ok\n");
            _m("1 2 3 4 .s clear quit");
            break;
        case 2:
            e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 2 3 4 ");
            e4__stack_push(test_data->task, (e4__cell)17);
            _m("1 2 3 4 .s clear notarealword");
            break;
        case 3:
            e4t__ASSERT_MATCH(e4t__term_obuf_consume(),
                    "<5> 17 1 2 3 4  EXCEPTION: -13\n");
            e4t__ASSERT_EQ(e4__stack_depth(test_data->task), 0);
            _m("bye");
            break;
        default:
            /* Should be unreachable. If we get here BYE has failed
               to terminate execution and we are now set to run
               forever. */
            e4t__ASSERT_CRITICAL(0);
            break;
    }

    #undef _m

    return e4__E_OK;
}

static void e4t__test_kernel_stack(void)
{
    struct e4__task *task = e4t__transient_task();

    e4__stack_push(task, (void *)2);
    e4__stack_push(task, (void *)4);
    e4__stack_push(task, (void *)8);
    e4__stack_push(task, (void *)16);
    e4__stack_push(task, (void *)32);

    e4t__ASSERT_EQ(e4__stack_depth(task), 5);
    e4t__ASSERT_EQ((e4__usize)e4__stack_peek(task), 32);
    e4t__ASSERT_EQ((e4__usize)e4__stack_pop(task), 32);
    e4t__ASSERT_EQ((e4__usize)e4__stack_pop(task), 16);
    e4t__ASSERT_EQ((e4__usize)e4__stack_pop(task), 8);
    e4t__ASSERT_EQ((e4__usize)e4__stack_pop(task), 4);
    e4t__ASSERT_EQ((e4__usize)e4__stack_pop(task), 2);
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    e4__stack_rpush(task, (void *)3);
    e4__stack_rpush(task, (void *)5);
    e4__stack_rpush(task, (void *)9);
    e4__stack_rpush(task, (void *)17);
    e4__stack_rpush(task, (void *)33);

    e4t__ASSERT_EQ((e4__usize)e4__stack_rpeek(task), 33);
    e4t__ASSERT_EQ((e4__usize)e4__stack_rpop(task), 33);
    e4t__ASSERT_EQ((e4__usize)e4__stack_rpop(task), 17);
    e4t__ASSERT_EQ((e4__usize)e4__stack_rpop(task), 9);
    e4t__ASSERT_EQ((e4__usize)e4__stack_rpop(task), 5);
    e4t__ASSERT_EQ((e4__usize)e4__stack_rpop(task), 3);
}

static void e4t__test_kernel_wordparse(void)
{
    const char *word;
    e4__usize len;

    #define _p(s, d, f) e4__mem_parse(s, d, strlen(s), f, &word)
    len = _p("   foo bar bas", ' ', e4__F_SKIP_LEADING);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo", len));

    len = _p("   foo, bar,, bas", ',', e4__F_SKIP_LEADING);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "   foo", len));

    len = _p("         ", ' ', e4__F_SKIP_LEADING);
    e4t__ASSERT_EQ(len, 0);

    len = _p("  foo\nbar", ' ', e4__F_SKIP_LEADING);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo", len));

    len = _p("  foo.bar", ' ', e4__F_SKIP_LEADING);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo.bar", len));

    len = _p("  foo.bar", ' ', 0);
    e4t__ASSERT_EQ(len, 0);

    #undef _p
}

void e4t__test_kernel(void)
{
    /* FIXME: Add uservar tests. */
    e4t__test_kernel_dict();
    e4t__test_kernel_exceptions();
    e4t__test_kernel_evaluate();
    e4t__test_kernel_io();
    e4t__test_kernel_math();
    e4t__test_kernel_mem_dict();
    e4t__test_kernel_numformat();
    e4t__test_kernel_numparse();
    e4t__test_kernel_quit();
    e4t__test_kernel_stack();
    e4t__test_kernel_wordparse();
}
