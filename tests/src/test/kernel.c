#define _POSIX_C_SOURCE 200809L
#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void e4t__test_kernel_builtin_exec(void)
{
    struct e4__task *task = e4t__transient_task();

    /* e4__builtin_exec should behave correctly in non-exceptional
       cases. */
    e4__stack_push(task, (e4__cell)2);
    e4__stack_push(task, (e4__cell)3);
    e4t__ASSERT_OK(e4__builtin_exec(task, e4__B_PLUS));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    /* Test that exceptions are correctly captured and returned by
       e4__builtin_exec. */
    e4t__ASSERT_EQ(e4__builtin_exec(task, e4__B_DUP), e4__E_STKUNDERFLOW);
}

static void e4t__test_kernel_dict(void)
{
    struct e4__task *task = e4t__transient_task();
    const struct e4__dict_header *sug, *h0, *h1, *h2;

    #define _d(s, c)    e4__dict_entry(task, s, sizeof(s) - 1, 0, c, NULL)
    #define _l(s)       e4__dict_lookup(task, s, sizeof(s) - 1)
    #define _s(l, p)    e4__dict_suggest(task, l, p, sizeof(p) - 1)
    #define _f(s)       e4__dict_forget(task, s, sizeof(s) - 1)
    #define _lxt(x)     e4__dict_lookup_xt(task, x)

    /* Simply insertion and lookup. */
    _d("first-entry", (e4__code_ptr)0xabcde);
    e4t__ASSERT(_l("first-entry"));
    e4t__ASSERT_EQ(_l("first-entry")->xt->code, 0xabcde);

    _d("second-entry", (e4__code_ptr)0x12345);
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
    _d("prefix73-first", (e4__code_ptr)0x11111);
    _d("prefix73-second", (e4__code_ptr)0x22222);
    _d("prefix73-third", (e4__code_ptr)0x33333);

    e4t__ASSERT((sug = _s(NULL, "prefix73")));
    e4t__ASSERT_EQ(sug->xt->code, 0x33333);
    e4t__ASSERT((sug = _s(sug, "prefix73")));
    e4t__ASSERT_EQ(sug->xt->code, 0x22222);
    e4t__ASSERT((sug = _s(sug, "prefix73")));
    e4t__ASSERT_EQ(sug->xt->code, 0x11111);
    e4t__ASSERT(!(sug = _s(sug, "prefix73")));
    e4t__ASSERT((sug = _s(sug, "prefix73")));
    e4t__ASSERT_EQ(sug->xt->code, 0x33333);

    /* Lookup based on execute token works correctly. */
    _d("xt-entry-1", (e4__code_ptr)0xabcde);
    e4t__ASSERT((h0 = _l("xt-entry-1")));
    e4t__ASSERT((h1 = _lxt((e4__cell)h0->xt)));
    e4t__ASSERT_EQ(h0, h1);

    _d("xt-entry-2", (e4__code_ptr)0x12345);
    e4t__ASSERT((h1 = _l("xt-entry-2")));

    /* XXX: In user code, no one should ever use a dictionary header
       returned from one of the lookup APIs in this way, but for the
       purposes of this test it is known to be safe. */
    ((struct e4__dict_header *)h1)->xt = h0->xt;

    e4t__ASSERT((h2 = _lxt((e4__cell)h1->xt)));
    e4t__ASSERT_EQ(h1, h2);
    e4t__ASSERT(h0 != h2);
    e4t__ASSERT_EQ(h0->xt, h2->xt);

    e4t__ASSERT(!_lxt(NULL));

    #undef _lxt
    #undef _f
    #undef _s
    #undef _l
    #undef _d
}

static void e4t__test_kernel_environmentq(void)
{
    struct e4__task *task = e4t__transient_task();
    e4__usize uout;
    struct e4__double dout;

    /* XXX: Many of these tests are either tautological and absurd or
       subject to frequent change if any fields are ever added to the
       task struct, but honestly whatever. */

    e4__task_eq(task, e4__EQ_ADDRUNITBITS, &uout);
    e4t__ASSERT_EQ(uout, e4__USIZE_BIT);

    e4__task_eq(task, e4__EQ_FLOORED, &uout);
    e4t__ASSERT_EQ(uout, 0);

    e4__task_eq(task, e4__EQ_HOLDSZ, &uout);
    e4t__ASSERT_EQ(uout, e4__PNO_MIN_SZ);

    e4__task_eq(task, e4__EQ_MAXCHAR, &uout);
    e4t__ASSERT_EQ(uout, 255);
    uout = 0;

    e4__task_eq(task, e4__EQ_MAXCSTRSZ, &uout);
    e4t__ASSERT_EQ(uout, 255);

    e4__task_eq(task, e4__EQ_MAXDOUBLE, &dout);
    e4t__ASSERT_DEQ(dout, e4__double_u(-1, (e4__usize)-1 >> 1));

    e4__task_eq(task, e4__EQ_MAXINT, &uout);
    e4t__ASSERT_EQ(uout, ((e4__usize)-1) >> 1);

    e4__task_eq(task, e4__EQ_MAXUDOUBLE, &dout);
    e4t__ASSERT_DEQ(dout, e4__double_u(-1, -1));

    e4__task_eq(task, e4__EQ_MAXUINT, &uout);
    e4t__ASSERT_EQ(uout, -1);

    e4__task_eq(task, e4__EQ_PADSZ, &uout);
    e4t__ASSERT_EQ(uout, 208);

    e4__task_eq(task, e4__EQ_RSTACKCELLS, &uout);
    e4t__ASSERT_EQ(uout, 52);

    e4__task_eq(task, e4__EQ_STACKCELLS, &uout);
    e4t__ASSERT_EQ(uout, 51);
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

static void e4t__test_kernel_evaluate_file(void)
{
    struct e4__task *task = e4t__transient_task();
    struct e4__file_exception fex = { e4__E_BUG, };
    char path[] = "/tmp/e4-XXXXXX";
    int fd;
    e4__usize fid = (e4__usize)-1;

    /* FIXME: This test exercises the POSIX file handlers. Ideally, it
       would also install mock handlers to test for behavioral issues
       unrelated to the POSIX handlers. */

    /* Create a temporary file and write some data into it. */
    fd = mkstemp(path);
    e4t__ASSERT(fd >= 0);
    e4t__ASSERT_EQ(write(fd, ": foo 3 7 + ;\n", 14), 14);
    e4t__ASSERT_EQ(write(fd, "foo 15", 6), 6);
    e4t__ASSERT_EQ(close(fd), 0);

    /* Attempt to evaluate a file with an open handler, ensuring that
       doing so closes the file descriptor and has the expected side
       effects on the task. */
    e4t__ASSERT_OK(e4__io_file_open(task, path, -1, e4__F_READ, &fid));
    e4t__ASSERT_EQ(fcntl((int)fid, F_GETFD), 0);
    e4t__ASSERT_OK(e4__evaluate_file(task, fid));
    e4t__ASSERT(fcntl((int)fid, F_GETFD) < 0);

    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);

    /* Attempt to evaluate a file based upon path. */
    e4t__ASSERT_OK(e4__evaluate_path(task, path, -1));
    e4t__ASSERT_OK(e4__task_fex(task, &fex));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);

    /* Ensure that on, on success, exception info is populated
       correctly. */
    e4t__ASSERT_EQ(fex.ex, e4__E_OK);

    /* Append some bad code to the file. */
    fd = open(path, O_WRONLY);
    e4t__ASSERT(fd >= 0);
    e4t__ASSERT(lseek(fd, 0, SEEK_END) >= 0);
    e4t__ASSERT_EQ(write(fd, "\nfoo bar", 8), 8);
    e4t__ASSERT_EQ(close(fd), 0);

    /* Ensure that exception info is populated correctly when executing
       from file descriptor. */
    e4t__ASSERT_OK(e4__io_file_open(task, path, -1, e4__F_READ, &fid));
    e4t__ASSERT_EQ(fcntl((int)fid, F_GETFD), 0);
    e4t__ASSERT_OK(e4__evaluate_file(task, fid));
    e4t__ASSERT_EQ(e4__task_fex(task, &fex), e4__E_UNDEFWORD);
    e4t__ASSERT(fcntl((int)fid, F_GETFD) < 0);

    e4t__ASSERT_EQ(fex.ex, e4__E_UNDEFWORD);
    e4t__ASSERT_EQ(fex.line, 3);
    e4t__ASSERT_EQ(fex.path_sz, 0);
    e4t__ASSERT_EQ(fex.path, NULL);
    e4t__ASSERT_EQ(fex.buf_sz, 7);
    e4t__ASSERT(!memcmp(fex.buf, "foo bar", 7));

    /* Ensure that exception info is populated correctly when executing
       from path. */
    e4t__ASSERT_OK(e4__evaluate_path(task, path, -1));
    e4t__ASSERT_EQ(e4__task_fex(task, &fex), e4__E_UNDEFWORD);
    e4t__ASSERT_EQ(fex.ex, e4__E_UNDEFWORD);
    e4t__ASSERT_EQ(fex.line, 3);
    e4t__ASSERT_EQ(fex.path_sz, 14);
    e4t__ASSERT(!memcmp(fex.path, path, 14));
    e4t__ASSERT_EQ(fex.buf_sz, 7);
    e4t__ASSERT(!memcmp(fex.buf, "foo bar", 7));

    e4__stack_clear(task);

    /* Ensure that exception info reflects e4__E_BYE exception when BYE
       occurs in a file and also that BYE halts file execution. */
    e4t__ASSERT_EQ(truncate(path, 0), 0);
    fd = open(path, O_WRONLY);
    e4t__ASSERT(fd >= 0);
    e4t__ASSERT_EQ(write(fd, "33 bye", 6), 6);
    e4t__ASSERT_EQ(close(fd), 0);

    e4t__ASSERT_OK(e4__evaluate_path(task, path, -1));
    e4t__ASSERT_EQ(e4__task_fex(task, &fex), e4__E_BYE);
    e4t__ASSERT_EQ(fex.ex, e4__E_BYE);
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    /* Ensure that attempting to evaluate a non-existent file throws the
       appropriate error. */
    e4t__ASSERT_EQ(e4__evaluate_path(task, "/not/real/bogusfile123", -1),
            e4__E_OPEN_FILE);
    e4t__ASSERT_EQ(e4__task_ior(task, 0), ENOENT);

    /* Delete the temporary file. */
    e4t__ASSERT_EQ(unlink(path), 0);
}

static void e4t__test_kernel_exceptions_da(struct e4__task *task,
        e4__cell user);
static void e4t__test_kernel_exceptions_throw10then(struct e4__task *task,
        e4__cell user);
static void e4t__test_kernel_exceptions_push(struct e4__task *task,
        e4__cell user);
static void e4t__test_kernel_exceptions_throw50(struct e4__task *task,
        e4__cell user);

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

    /* Test throwing with a then-callback. */
    e4__dict_entry(task, "throw10then-push5", 17, 0,
            e4t__test_kernel_exceptions_throw10then,
            (void *)(e4__usize)e4t__test_kernel_exceptions_push);
    e4t__ASSERT_EQ(e4__evaluate(task, "throw10then-push5", -1), 10);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    /* Test throwing an exception from a then-callback. */
    e4__dict_entry(task, "throw10then-throw50", 19, 0,
            e4t__test_kernel_exceptions_throw10then,
            (void *)(e4__usize)e4t__test_kernel_exceptions_throw50);
    e4t__ASSERT_EQ(e4__evaluate(task, "throw10then-throw50", -1), 10);
    e4t__ASSERT_EQ(e4__evaluate(task, "' throw10then-throw50 catch", -1), 50);
}

static void e4t__test_kernel_exceptions_da(struct e4__task *task,
        e4__cell user)
{
    const e4__usize ex = (e4__usize)e4__DEREF(user);

    if (!e4__USIZE_IS_NEGATIVE(ex)) {
        /* Set up another exception handler and re-call this
           function. */
        struct e4__execute_token xt = {
            e4t__test_kernel_exceptions_da,
            NULL,
        };
        xt.user = (e4__cell)e4__USIZE_NEGATE(ex);
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

static void e4t__test_kernel_exceptions_throw10then(struct e4__task *task,
        e4__cell user)
{
    e4__exception_throw_then(task, 10,
            (e4__code_ptr)(e4__usize)e4__DEREF(user),
            (void *)5);

    /* This should be unreachable. */
    e4__execute_ret(task);
}

static void e4t__test_kernel_exceptions_push(struct e4__task *task,
        e4__cell user)
{
    e4__stack_push(task, e4__DEREF(user));
}

static void e4t__test_kernel_exceptions_throw50(struct e4__task *task,
        e4__cell user)
{
    e4__exception_throw(task, 50);
}

static void e4t__test_kernel_io(void)
{
    struct e4__task *task = e4t__transient_task();
    struct e4__io_func io_func = {0,};
    const char *word;
    e4__usize len;

    /* Clear all test related IO handlers. */
    e4__task_io_init(task, &io_func);

    /* Test that IO functions return the appropriate exception code
       when no handler is available. */
    e4t__ASSERT_EQ(e4__io_key(task, NULL), e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__io_accept(task, NULL, 0), e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__io_type(task, NULL, 0), e4__E_UNSUPPORTED);

    task = e4t__transient_task();

    /* XXX: e4__io_accept, e4__io_type, and e4__io_key APIs are covered
       in term.c. */

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

static void e4t__test_kernel_io_dlfcn(void)
{
    struct e4__task *task = e4t__transient_task();
    void *func0 = NULL, *handle0 = NULL, *func1, *handle1;

    /* XXX: This test is *VERY* likely to only pass on Linux or in an
       extremely Linux-like environment. */

    /* Test that opening a dynamic library and loading a symbol works
       as expected. */
    e4t__ASSERT_OK(e4__io_dlopen(task, "libc.so.6", &handle0));
    e4t__ASSERT_OK(e4__io_dlsym(task, handle0, "strlen", &func0));
    e4t__ASSERT_EQ(strlen, func0);

    /* Test that attempting to open a non-existent library or load
       a nonexistent symbol fails as expected. */
    e4t__ASSERT_EQ(e4__io_dlopen(task, "probably-not-real.so.what", &handle1),
            e4__E_DLFAILURE);
    e4t__ASSERT_EQ(e4__io_dlsym(task, handle0, "probably-not-real.so.what",
                &func1), e4__E_DLFAILURE);

    /* Test that closing the dynamic library works as expected. */
    e4t__ASSERT_OK(e4__io_dlclose(task, handle0));
}

static void e4t__test_kernel_io_dump(void)
{
    /* XXX: Parts of this test only work correctly on a 64 bit system
       where unsigned long long values have 8 bit alignment. */

    static const char *test_str =
            "test \x88\x99\xaa string with some \x01 \x02 \x03 data";

    char expected[240] = {0,};
    unsigned long buffer[8];
    struct e4__task *task = e4t__transient_task();
    char *unaligned_buf = ((char *)buffer) + 3;
    e4__usize len = strlen(test_str);

    /* XXX: It would violate strict aliasing rules if we ever looked at
       buffer again after modifying it using unaligned_buf. So long as
       we don't do that, everything should be fine. */
    memcpy(unaligned_buf, test_str, len);

    e4t__term_obuf_consume();
    e4t__ASSERT_OK(e4__io_dump(task, (e4__cell)unaligned_buf, len));

    #define _f  "%016lx   "
    sprintf(expected,
            _f "???? ??74 6573 7420  8899 aa20 7374 7269   ...test ... stri\n"
            _f "6e67 2077 6974 6820  736f 6d65 2001 2002   ng with some . .\n"
            _f "2003 2064 6174 61??  ???? ???? ???? ????    . data.........\n",
            (unsigned long)&buffer[0],
            (unsigned long)&buffer[2],
            (unsigned long)&buffer[4]);
    #undef _f

    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), expected);
}

static void e4t__test_kernel_io_file(void)
{
    struct e4__task *task = e4t__transient_task();
    char buf[140] = {0};
    char path[] = "/tmp/e4-XXXXXX";
    int fd;
    e4__usize fid = (e4__usize)-1;
    e4__usize sz;

    /* FIXME: This test exercises the POSIX file handlers. Ideally, it
       would also install mock handlers to test for behavioral issues
       unrelated to the POSIX handlers. */

    /* Create a temporary file and write some data into it. */
    fd = mkstemp(path);
    e4t__ASSERT(fd >= 0);
    e4t__ASSERT_EQ(write(fd, "foo bar bas\n", 12), 12);
    e4t__ASSERT_EQ(write(fd, "quux\n", 5), 5);
    e4t__ASSERT_EQ(8, write(fd, "lastline", 8));
    e4t__ASSERT_EQ(close(fd), 0);

    /* Test that it's possible to open a file with read permissions
       and read from the file. */
    e4t__ASSERT_OK(e4__io_file_open(task, path, -1, e4__F_READ, &fid));
    sz = 4;
    e4t__ASSERT_OK(e4__io_file_read(task, fid, buf, &sz));
    e4t__ASSERT_EQ(sz, 4);
    buf[4] = '\0';
    e4t__ASSERT(!strcmp(buf, "foo "));

    /* Test that readline works as expected. */
    sz = sizeof(buf);
    e4t__ASSERT_OK(e4__io_file_readline(task, fid, buf, &sz));
    e4t__ASSERT_EQ(sz, 8);
    buf[8] = '\0';
    e4t__ASSERT(!strcmp(buf, "bar bas\n"));

    sz = sizeof(buf);
    e4t__ASSERT_OK(e4__io_file_readline(task, fid, buf, &sz));
    e4t__ASSERT_EQ(sz, 5);
    buf[5] = '\0';
    e4t__ASSERT(!strcmp(buf, "quux\n"));

    sz = sizeof(buf);
    e4t__ASSERT_OK(e4__io_file_readline(task, fid, buf, &sz));
    e4t__ASSERT_EQ(sz, 8);
    buf[8] = '\0';
    e4t__ASSERT(!strcmp(buf, "lastline"));

    /* Test that EOF is returned when attempting to read from a file
       with no more data available. */
    sz = sizeof(buf);
    e4t__ASSERT_EQ(e4__io_file_readline(task, fid, buf, &sz), e4__E_EOF);
    e4t__ASSERT_EQ(sz, 0);

    /* Test that it's possible to close a file. */
    e4t__ASSERT_OK(e4__io_file_close(task, fid));

    /* Test that attempting to open a file that doesn't exist returns
       the appropriate error code and sets the task platform specific
       IO error code as expected. */
    e4t__ASSERT_EQ(e4__io_file_open(task, "/not/real/bogusfile123", -1,
            e4__F_READ, &fid), e4__E_OPEN_FILE);
    e4t__ASSERT_EQ(e4__task_ior(task, 0), ENOENT);

    /* Delete temporary file. */
    e4t__ASSERT_EQ(unlink(path), 0);

    /* FIXME: Add tests for opening a file with write permissions and
       writing to the file once associated handlers have been
       implemented. */
    /* FIXME: Add tests for writing to a file with only read
       permissions and reading from a file with only write
       permissions. */
    /* FIXME: Add tests for all remaining io_file kernel APIs once the
       associated handlers have been implemented. */
}

static e4__usize e4t__test_kernel_quit_accept(void *user, char *buf,
        e4__usize *n);
static e4__usize e4t__test_kernel_quit_key(void *user, char *b);

struct e4t__test_kernel_quit_data {
    struct e4__task *task;
    e4__usize step;
};

static void e4t__test_kernel_io_pno(void)
{
    struct e4__task *task = e4t__transient_task();
    struct e4__double d = {0};
    char *pno = NULL;
    e4__usize len = 0;

    /* Test that calling PNO kernel APIs when PNO hasn't been
       initialized results in safe failure. */
    e4t__ASSERT_EQ(e4__io_pno_digit(task, &d), e4__E_INVMEMADDR);
    e4t__ASSERT_EQ(e4__io_pno_digits(task, &d, 0), e4__E_INVMEMADDR);
    e4t__ASSERT_EQ(e4__io_pno_finish(task, &pno, &len), e4__E_INVMEMADDR);
    e4t__ASSERT_EQ(pno, NULL);
    e4t__ASSERT_EQ(len, 0);
    e4t__ASSERT_EQ(e4__io_pno_hold(task, 'f'), e4__E_INVMEMADDR);
    e4t__ASSERT_EQ(e4__io_pno_holds(task, "foo bar bas", 10),
            e4__E_INVMEMADDR);

    /* Test that empty PNO works as expected. */
    e4__io_pno_start(task);
    len = 1;
    e4t__ASSERT_OK(e4__io_pno_finish(task, &pno, &len));
    e4t__ASSERT_EQ(len, 0);
    pno = NULL;

    /* Test that calling PNO kernel APIs when PNO has ever been
       initialized but now is not results in safe failure. */
    e4t__ASSERT_EQ(e4__io_pno_digit(task, &d), e4__E_INVMEMADDR);
    e4t__ASSERT_EQ(e4__io_pno_digits(task, &d, 0), e4__E_INVMEMADDR);
    e4t__ASSERT_EQ(e4__io_pno_finish(task, &pno, &len), e4__E_INVMEMADDR);
    e4t__ASSERT_EQ(pno, NULL);
    e4t__ASSERT_EQ(len, 0);
    e4t__ASSERT_EQ(e4__io_pno_hold(task, 'f'), e4__E_INVMEMADDR);
    e4t__ASSERT_EQ(e4__io_pno_holds(task, "foo bar bas", 10),
            e4__E_INVMEMADDR);

    /* Test calling each API once. */
    e4__io_pno_start(task);
    d = e4__double_u(53, 0);
    e4t__ASSERT_OK(e4__io_pno_digit(task, &d));
    e4t__ASSERT_DEQ(d, e4__double_u(5, 0));
    e4t__ASSERT_OK(e4__io_pno_finish(task, &pno, &len));
    e4t__ASSERT_EQ(len, 1);
    e4t__ASSERT(!e4__mem_strncasecmp(pno, "3", len));

    e4__io_pno_start(task);
    e4t__ASSERT_OK(e4__io_pno_hold(task, 'f'));
    e4t__ASSERT_OK(e4__io_pno_finish(task, &pno, &len));
    e4t__ASSERT_EQ(len, 1);
    e4t__ASSERT(!e4__mem_strncasecmp(pno, "f", len));

    e4__io_pno_start(task);
    d = e4__double_u(53419, 0);
    e4t__ASSERT_OK(e4__io_pno_digits(task, &d, 0));
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));
    e4t__ASSERT_OK(e4__io_pno_finish(task, &pno, &len));
    e4t__ASSERT_EQ(len, 5);
    e4t__ASSERT(!e4__mem_strncasecmp(pno, "53419", len));

    e4__io_pno_start(task);
    d = e4__double_u(-53419, -1);
    e4t__ASSERT_OK(e4__io_pno_digits(task, &d, e4__F_SIGNED));
    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));
    e4t__ASSERT_OK(e4__io_pno_finish(task, &pno, &len));
    e4t__ASSERT_EQ(len, 6);
    e4t__ASSERT(!e4__mem_strncasecmp(pno, "-53419", len));

    e4__io_pno_start(task);
    e4t__ASSERT_OK(e4__io_pno_holds(task, "foo bar bas", 10));
    e4t__ASSERT_OK(e4__io_pno_finish(task, &pno, &len));
    e4t__ASSERT_EQ(len, 10);
    e4t__ASSERT(!e4__mem_strncasecmp(pno, "foo bar bas", len));

    /* Test combining calls. */
    d = e4__double_u(1995, 0);

    e4__io_pno_start(task);
    e4t__ASSERT_OK(e4__io_pno_digit(task, &d));
    e4t__ASSERT_OK(e4__io_pno_digit(task, &d));
    e4t__ASSERT_OK(e4__io_pno_hold(task, '.'));
    e4t__ASSERT_OK(e4__io_pno_digits(task, &d, 0));
    e4t__ASSERT_OK(e4__io_pno_holds(task, "Price: $", 8));
    e4t__ASSERT_OK(e4__io_pno_finish(task, &pno, &len));

    e4t__ASSERT_DEQ(d, e4__double_u(0, 0));
    e4t__ASSERT_EQ(len, 13);
    e4t__ASSERT(!e4__mem_strncasecmp(pno, "Price: $19.95", len));

    /* Test that restarting clears current output but is otherwise
       harmless. */
    d = e4__double_u(1234, 0);

    e4__io_pno_start(task);
    e4t__ASSERT_OK(e4__io_pno_digit(task, &d));
    e4t__ASSERT_OK(e4__io_pno_digit(task, &d));

    e4__io_pno_start(task);
    e4t__ASSERT_OK(e4__io_pno_digit(task, &d));
    e4t__ASSERT_OK(e4__io_pno_digit(task, &d));

    e4t__ASSERT_OK(e4__io_pno_finish(task, &pno, &len));
    e4t__ASSERT_EQ(len, 2);
    e4t__ASSERT(!e4__mem_strncasecmp(pno, "12", len));

    /* Test that overflowing works as expected. */
    d = e4__double_u(1234, 0);
    e4__io_pno_start(task);
    e4__task_allot_unchecked(task, e4__task_unused(task) + e4__USIZE_BIT * 2);

    e4t__ASSERT_OK(e4__io_pno_digit(task, &d));
    e4t__ASSERT_OK(e4__io_pno_digit(task, &d));
    e4t__ASSERT_EQ(e4__io_pno_digit(task, &d), e4__E_PNOOVERFLOW);
    e4t__ASSERT_EQ(e4__io_pno_digits(task, &d, 0), e4__E_PNOOVERFLOW);
    e4t__ASSERT_EQ(e4__io_pno_hold(task, 'f'), e4__E_PNOOVERFLOW);
    e4t__ASSERT_EQ(e4__io_pno_holds(task, "foo bar bas", 10),
            e4__E_PNOOVERFLOW);

    e4t__ASSERT_OK(e4__io_pno_finish(task, &pno, &len));
    e4t__ASSERT_EQ(len, 2);
    e4t__ASSERT(!e4__mem_strncasecmp(pno, "34", len));

    e4__io_pno_start(task);
    e4__task_allot_unchecked(task, 3);
    e4t__ASSERT_EQ(e4__io_pno_finish(task, &pno, &len), e4__E_PNOOVERFLOW);

    /* Test that PNO fails gracefully even when the dictionary is full
       or has overflowed. */
    e4__io_pno_start(task);

    e4t__ASSERT_EQ(e4__io_pno_digit(task, &d), e4__E_PNOOVERFLOW);
    e4t__ASSERT_EQ(e4__io_pno_digits(task, &d, 0), e4__E_PNOOVERFLOW);
    e4t__ASSERT_EQ(e4__io_pno_hold(task, 'f'), e4__E_PNOOVERFLOW);
    e4t__ASSERT_EQ(e4__io_pno_holds(task, "foo bar bas", 10),
            e4__E_PNOOVERFLOW);
    e4t__ASSERT_EQ(e4__io_pno_finish(task, &pno, &len), e4__E_PNOOVERFLOW);

    e4__task_allot(task, e4__mem_aligned(1));
    e4__io_pno_start(task);

    e4t__ASSERT_EQ(e4__io_pno_digit(task, &d), e4__E_PNOOVERFLOW);
    e4t__ASSERT_EQ(e4__io_pno_digits(task, &d, 0), e4__E_PNOOVERFLOW);
    e4t__ASSERT_EQ(e4__io_pno_hold(task, 'f'), e4__E_PNOOVERFLOW);
    e4t__ASSERT_EQ(e4__io_pno_holds(task, "foo bar bas", 10),
            e4__E_PNOOVERFLOW);
    e4t__ASSERT_EQ(e4__io_pno_finish(task, &pno, &len), e4__E_PNOOVERFLOW);
}

static void e4t__test_kernel_io_see(void)
{
    struct e4__task *task = e4t__transient_task();

    /* FIXME: This test would benefit from a case sensitive
       e4t__ASSERT_MATCH variant, should one ever be added. */

    /* XXX: This test is sparse compared to e4t__test_util_mem_see,
       which covers the e4__mem_see's detailed functionality.
       e4__io_see is a thin wrapper around this function, so this test
       exists primarily to ensure that e4__io_see invokes e4__mem_see
       and provides its output correctly. */

    /* Check that calling e4__io_see with an invalid dictionary entry
       fails. */
    e4t__ASSERT_EQ(e4__io_see(task, "notathing", 9), e4__E_UNDEFWORD);

    /* Check that calling e4__io_see with a dictionary entry that is
       too long fails. */
    #define _long_name  \
        "very-long-entry-that-will-definitely-" \
        "cause-an-overflow-because-it-is-itself-over-80-characters"

    e4t__ASSERT_OK(e4__evaluate(task, ": " _long_name " 2 2 + ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, _long_name, -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__io_see(task, _long_name, sizeof(_long_name) - 1),
            e4__E_SEEOVERFLOW);
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "");

    #undef _long_name

    /* Check that calling e4__io_see with a valid dictionary entry
       yields the expected output. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 2 2 + ;", -1));
    e4t__ASSERT_OK(e4__io_see(task, "foo", 3));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(),
            ": foo ( threaded - user: 0x0 )\n"
            "    LIT_CELL\n"
            "    0x2\n"
            "    LIT_CELL\n"
            "    0x2\n"
            "    +\n"
            "    EXIT\n"
            "    SENTINEL\n");
}

static void e4t__test_kernel_mem(void)
{
    struct e4__task *task = e4t__transient_task();
    e4__cell here;
    e4__usize unused;

    /* Test that allocating returns old here and that allocating
       nothing simply returns here and changes nothing. */
    here = e4__task_uservar(task, e4__UV_HERE);
    e4t__ASSERT_EQ(e4__task_allot(task, e4__mem_cells(1)), here);
    here = e4__task_uservar(task, e4__UV_HERE);
    e4t__ASSERT_EQ(e4__task_allot(task, e4__mem_cells(0)), here);
    e4t__ASSERT_EQ(e4__task_uservar(task, e4__UV_HERE), here);

    /* Test that unused changes as expected when allocating. */
    unused = e4__task_unused(task);
    e4__task_allot(task, e4__mem_cells(1));
    e4t__ASSERT_EQ(e4__task_unused(task), unused - e4__mem_cells(1));

    /* Test that allocating all unused memory leaves none. */
    e4__task_allot(task, e4__task_unused(task));
    e4t__ASSERT_EQ(e4__task_unused(task), 0);

    /* Test that unallocating with negation is possible. */
    here = e4__task_allot(task, e4__USIZE_NEGATE(unused));
    e4t__ASSERT_EQ(e4__task_unused(task), unused);

    /* Test that allocating everything runs into the PNO buffer
       then pad. */
    e4t__ASSERT_EQ((e4__usize)here + e4__PNO_MIN_SZ,
            e4__task_uservar(task, e4__UV_PAD));

    /* Test that unused reports zero whenever HERE is greater than
       PAD. */
    e4t__ASSERT((e4__usize)e4__task_uservar(task, e4__UV_HERE) <
            (e4__usize)e4__task_uservar(task, e4__UV_PAD));
    e4__task_allot_unchecked(task, e4__task_unused(task) +
            e4__mem_aligned(e4__PNO_MIN_SZ) + e4__mem_cells(2));
    e4t__ASSERT((e4__usize)e4__task_uservar(task, e4__UV_HERE) >
            (e4__usize)e4__task_uservar(task, e4__UV_PAD));
    e4t__ASSERT_EQ(e4__task_unused(task), 0);

    e4__task_allot_unchecked(task,
            e4__USIZE_NEGATE(e4__mem_aligned(e4__PNO_MIN_SZ)));

    e4__task_allot_unchecked(task, e4__USIZE_NEGATE(e4__mem_cells(1)));
    e4t__ASSERT_EQ(e4__task_unused(task), 0);
    e4__task_allot_unchecked(task, e4__USIZE_NEGATE(e4__mem_cells(1)));
    e4t__ASSERT_EQ(e4__task_unused(task), 0);

    e4__task_allot(task, e4__USIZE_NEGATE(e4__mem_cells(1)));
    e4t__ASSERT_EQ(e4__task_unused(task), e4__mem_cells(1));

    /* Test that checked allocation out of bounds simply doesn't work. */
    here = e4__task_allot(task, e4__mem_cells(1));
    e4t__ASSERT(here);
    e4t__ASSERT_EQ(e4__task_allot(task, 1), NULL);
    e4t__ASSERT_EQ(here + 1, e4__task_uservar(task, e4__UV_HERE));
}

static void e4t__test_kernel_quit(void)
{
    struct e4__io_func old_iof;
    struct e4__task *task = e4t__transient_task();
    struct e4t__test_kernel_quit_data test_data = {0};
    struct e4__io_func iof = {
        NULL,
        e4t__test_kernel_quit_accept,
        e4t__test_kernel_quit_key,
    };
    test_data.task = task;
    iof.user = &test_data;

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
    const char *msg = (const char *)0x1234;

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
                    "<5> 17 1 2 3 4  EXCEPTION: undefined word (-13)\n");
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
            e4t__ASSERT_MATCH(e4t__term_obuf_consume(),
                    " EXCEPTION: undefined word (-13)\n");
            e4t__ASSERT(!e4__task_compiling(test_data->task));
            e4t__ASSERT_EQ(e4__stack_depth(test_data->task), 0);
            /* Test that SOURCE-ID contains UID. */
            _m("source-id");
            break;
        case 7:
            e4t__ASSERT_EQ(e4__stack_depth(test_data->task), 1);
            e4t__ASSERT_EQ(e4__stack_pop(test_data->task), e4__SID_UID);
            /* Test the KEY builtin. */
            _m("key");
            break;
        case 8:
            e4t__ASSERT_EQ(e4__stack_depth(test_data->task), 1);
            e4t__ASSERT_EQ(e4__stack_pop(test_data->task), 'f');
            /* Test that raising an arbitrary exception from this
               context behaves correctly. */
            e4t__term_obuf_consume();
            e4__exception_throw(test_data->task, e4__E_FAILURE);
            /* This should not be reached. */
            e4t__ASSERT(0);
            break;
        case 9:
            /* The output buffer should reflect the fact than an
               exception has occurred. */
            e4t__ASSERT_MATCH(e4t__term_obuf_consume(),
                    " EXCEPTION: generic failure (-256)\n");
            /* Test that a firing ABORT" exception is formatted
               correctly. */
            e4t__ASSERT_OK(e4__evaluate(test_data->task,
                    ": alwaysabort -1 abort\" You asked for it.\" ;", -1));
            _m("alwaysabort");
            break;
        case 10:
            /* The output buffer should reflect the fact that an ABORT"
               exception has occurred. */
            e4t__ASSERT_MATCH(e4t__term_obuf_consume(),
                    " You asked for it.\n"
                    " EXCEPTION: abort with message (-2)\n");
            e4t__ASSERT_EQ(e4__task_last_abortq(test_data->task, &msg), 0);
            e4t__ASSERT_EQ(msg, NULL);
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

static e4__usize e4t__test_kernel_quit_key(void *user, char *b)
{
    *b = 'f';
    return e4__E_OK;
}

static void e4t__test_kernel_rstack_overflow(void)
{
    struct e4__task *task = e4t__transient_task();

    /* XXX: Only tests kernel return stack overflow through recursion
       and by pushing onto the return stack, with exceptions enabled.
       The corresponding tests for the case when exceptions are disabled
       are in execute.c. */
    /* FIXME: There are other ways the return stack could overflow but
       those are not currently covered. */

    /* Test return stack overflows with exceptions enabled. */
    e4t__ASSERT_OK(e4__evaluate(task, ": overflow recurse ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "overflow", -1), e4__E_RSTKOVERFLOW);
    e4t__ASSERT_OK(e4__evaluate(task, ": overflow begin 5 >r again ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "overflow", -1), e4__E_RSTKOVERFLOW);
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

    e4t__ASSERT_EQ(e4__stack_avail(task), 51);
    e4t__ASSERT_EQ(e4__stack_ravail(task), 52);
}

static void e4t__test_kernel_stack_overflow(void)
{
    struct e4__task *task = e4t__transient_task();

    /* XXX: Only tests kernel stack overflow by pushing onto the
       return stack and repeatedly executing dup, with exceptions
       enabled. The corresponding tests for the case when exceptions are
       disabled are in execute.c. */
    /* FIXME: There are other ways the stack could overflow but those
       are not currently covered. */

    /* Test return stack overflows with exceptions enabled. */
    e4t__ASSERT_OK(e4__evaluate(task, ": overflow begin 5 again ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "overflow", -1), e4__E_STKOVERFLOW);
    e4__stack_clear(task);
    e4t__ASSERT_OK(e4__evaluate(task, ": overflow 5 begin dup again ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "overflow", -1), e4__E_STKOVERFLOW);
}

void e4t__test_kernel(void)
{
    /* FIXME: Add uservar tests. */
    e4t__test_kernel_builtin_exec();
    e4t__test_kernel_dict();
    e4t__test_kernel_environmentq();
    e4t__test_kernel_evaluate();
    e4t__test_kernel_evaluate_file();
    e4t__test_kernel_exceptions();
    e4t__test_kernel_io();
    e4t__test_kernel_io_dlfcn();
    e4t__test_kernel_io_dump();
    e4t__test_kernel_io_file();
    e4t__test_kernel_io_pno();
    e4t__test_kernel_io_see();
    e4t__test_kernel_mem();
    e4t__test_kernel_quit();
    e4t__test_kernel_rstack_overflow();
    e4t__test_kernel_stack();
    e4t__test_kernel_stack_overflow();
}
