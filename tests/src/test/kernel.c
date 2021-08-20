#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

#include <string.h>

static void e4t__test_kernel_dict(void)
{
    struct e4__task *task = e4t__transient_task();
    const struct e4__dict_header *sug;

    #define _d(s, c)    e4__dict_entry(task, s, sizeof(s) - 1, 0, c, NULL)
    #define _l(s)       e4__dict_lookup(task, s, sizeof(s) - 1)
    #define _s(l, p)    e4__dict_suggest(task, l, p, sizeof(p) - 1)
    #define _f(s)       e4__dict_forget(task, s, sizeof(s) - 1)

    /* Simply insertion and lookup. */
    _d("first-entry", (void *)0xabcde);
    e4t__ASSERT(_l("first-entry"));
    e4t__ASSERT_EQ(_l("first-entry")->xt->code, 0xabcde);

    _d("second-entry", (void *)0x12345);
    e4t__ASSERT(_l("second-entry"));
    e4t__ASSERT_EQ(_l("second-entry")->xt->code, 0x12345);

    e4t__ASSERT(_l("first-entry"));
    e4t__ASSERT_EQ(_l("first-entry")->xt->code, 0xabcde);
    e4t__ASSERT_EQ(_l("FIRST-entry")->xt->code, 0xabcde);

    /* Can't forget a word that isn't present or a builtin. */
    e4t__ASSERT_EQ(_l("not-present"), NULL);
    e4t__ASSERT_EQ(_f("not-present"), e4__E_INVFORGET);
    e4t__ASSERT(_l("dup"));
    e4t__ASSERT_EQ(_f("dup"), e4__E_INVFORGET);

    /* FORGET forgets all words defined after a word. */
    e4t__ASSERT_OK(_f("first-entry"));
    e4t__ASSERT_EQ(_l("first-entry"), NULL);
    e4t__ASSERT_EQ(_l("second-entry"), NULL);

    /* Prefix suggestion functions correctly. */
    _d("prefix73-first", (void *)0x11111);
    _d("prefix73-second", (void *)0x22222);
    _d("prefix73-third", (void *)0x33333);

    e4t__ASSERT((sug = _s(NULL, "prefix73")));
    e4t__ASSERT_EQ(sug->xt->code, 0x33333);
    e4t__ASSERT((sug = _s(sug, "prefix73")));
    e4t__ASSERT_EQ(sug->xt->code, 0x22222);
    e4t__ASSERT((sug = _s(sug, "prefix73")));
    e4t__ASSERT_EQ(sug->xt->code, 0x11111);
    e4t__ASSERT(!(sug = _s(sug, "prefix73")));
    e4t__ASSERT((sug = _s(sug, "prefix73")));
    e4t__ASSERT_EQ(sug->xt->code, 0x33333);

    #undef _f
    #undef _s
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
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "4 dropfail", -1));
    e4t__term_obuf_consume();
    e4t__ASSERT_OK(e4__evaluate(task, ".s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 2 3 4 ");

    /* Test that QUIT exception actually percolates all the way up and
       *doesn't* restore sp. */
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "4 dropquit", -1), e4__E_QUIT);
    e4t__ASSERT_OK(e4__evaluate(task, ".s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<1> 1 ");

    /* Test that BYE exception actually percolates all the way up and
       *does* restore sp. */
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "4 dropbye", -1), e4__E_BYE);
    e4t__ASSERT_OK(e4__evaluate(task, ".s clear", -1));
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
        e4__execute_ret(task);
        return;
    }

    e4__stack_drop(task);
    e4__stack_drop(task);
    e4__stack_drop(task);

    e4__exception_throw(task, ex);

    /* This should be unreachable. */
    e4__execute_ret(task);
}

static void e4t__test_kernel_evaluate(void)
{
    struct e4__task *task = e4t__transient_task();

    /* FIXME: Add more explicit evaluate tests. */

    /* Empty strings or strings with only delimiters should be
       no-ops. */
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);
    e4t__ASSERT_EQ(e4__evaluate(task, "", -1), e4__E_OK);
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    e4t__ASSERT_EQ(e4__stack_depth(task), 0);
    e4t__ASSERT_EQ(e4__evaluate(task, " ", -1), e4__E_OK);
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    /* Undefined words should generate an exception and clear
       the stack. */
    e4__stack_clear(task);
    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 4 5 rll", -1),
            e4__E_UNDEFWORD);
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "");
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    /* Attempting to interpret a compile-only word throws
       an exception. */
    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 exit", -1), e4__E_COMPONLYWORD);
}

static void e4t__test_kernel_io(void)
{
    struct e4__task *task = e4t__transient_task();
    struct e4__io_func io_func = {0,};
    const char *word;
    e4__usize len;

    /* FIXME: Add more IO tests. */

    /* Clear all test related IO handlers. */
    e4__task_io_init(task, &io_func);

    /* Test that IO functions return the appropriate exception code
       when no handler is available. */
    e4t__ASSERT_EQ(e4__io_key(task, NULL), e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__io_accept(task, NULL, 0), e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__io_type(task, NULL, 0), e4__E_UNSUPPORTED);

    task = e4t__transient_task();

    /* Test the e4__io_word API. */
    e4t__term_ibuf_feed("parse      some  words  ", -1);
    e4__io_refill(task, NULL);

    e4t__ASSERT((word = e4__io_word(task, ' ')));
    e4t__ASSERT_EQ(*word++, 5);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "parse", 5));

    e4t__ASSERT((word = e4__io_word(task, ' ')));
    e4t__ASSERT_EQ(*word++, 4);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "some", 4));

    e4t__ASSERT((word = e4__io_word(task, ' ')));
    e4t__ASSERT_EQ(*word++, 5);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "words", 4));

    e4t__ASSERT((word = e4__io_word(task, ' ')));
    e4t__ASSERT_EQ(*word++, 0);

    /* Test the e4__io_parse API. */
    e4t__term_ibuf_feed("  first s\" \"\"foo bar\\\" bas\" quux", -1);
    e4__io_refill(task, NULL);

    len = e4__io_parse(task, ' ', e4__F_SKIP_LEADING, &word);
    e4t__ASSERT_EQ(len, 5);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "first", 5));

    len = e4__io_parse(task, ' ', e4__F_SKIP_LEADING, &word);
    e4t__ASSERT_EQ(len, 2);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "s\"", 2));

    len = e4__io_parse(task, '"', 0, &word);
    e4t__ASSERT_EQ(len, 0);

    len = e4__io_parse(task, '"', e4__F_SKIP_LEADING | e4__F_IGNORE_ESC,
            &word);
    e4t__ASSERT_EQ(len, 13);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo bar\\\" bas", 13));

    len = e4__io_parse(task, ' ', e4__F_SKIP_LEADING, &word);
    e4t__ASSERT_EQ(len, 4);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "quux", 4));
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

static e4__usize e4t__test_kernel_quit_accept(void *user, char *buf,
        e4__usize *n)
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
            _m(": foo");
            break;
        case 4:
            e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "");
            _m(";");
            break;
        case 5:
            e4t__ASSERT_MATCH(e4t__term_obuf_consume(), " ok\n");
            /* Test that an exception while compiling is handled
               correctly. */
            _m(": foo to absolutely-nothing-at-all");
            break;
        case 6:
            e4t__ASSERT_MATCH(e4t__term_obuf_consume(), " EXCEPTION: -13\n");
            e4t__ASSERT(!e4__task_compiling(test_data->task));
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
    e4t__ASSERT_EQ(e4__stack_peek(task), 32);
    e4t__ASSERT_EQ(e4__stack_pop(task), 32);
    e4t__ASSERT_EQ(e4__stack_pop(task), 16);
    e4t__ASSERT_EQ(e4__stack_pop(task), 8);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    e4__stack_rpush(task, (void *)3);
    e4__stack_rpush(task, (void *)5);
    e4__stack_rpush(task, (void *)9);
    e4__stack_rpush(task, (void *)17);
    e4__stack_rpush(task, (void *)33);

    e4t__ASSERT_EQ(e4__stack_rdepth(task), 5);
    e4t__ASSERT_EQ(e4__stack_rpeek(task), 33);
    e4t__ASSERT_EQ(e4__stack_rpop(task), 33);
    e4t__ASSERT_EQ(e4__stack_rpop(task), 17);
    e4t__ASSERT_EQ(e4__stack_rpop(task), 9);
    e4t__ASSERT_EQ(e4__stack_rpop(task), 5);
    e4t__ASSERT_EQ(e4__stack_rpop(task), 3);
    e4t__ASSERT_EQ(e4__stack_rdepth(task), 0);
}

void e4t__test_kernel(void)
{
    /* FIXME: Add uservar tests. */
    e4t__test_kernel_dict();
    e4t__test_kernel_exceptions();
    e4t__test_kernel_evaluate();
    e4t__test_kernel_io();
    e4t__test_kernel_quit();
    e4t__test_kernel_stack();
}
