#include "e4.h"
#include "e4-internal.h"

#include <string.h>

struct e4__evaluate_thunk {
    e4__code_ptr code;
    void *padding;
};

static void e4__evaluate_compile(struct e4__task *task, const char *word,
        e4__u8 len)
{
    register const struct e4__dict_header *header;
    register e4__usize pcount;
    e4__usize num;

    header = e4__dict_lookup(task, word, len);

    if (header) {
        if (header->flags & e4__F_IMMEDIATE)
            e4__execute(task, header->xt);
        else
            e4__compile_cell(task, (e4__cell)header->xt);
        return;
    }

    /* Attempt to convert to a number and compile. */
    pcount = e4__mem_number(word, len, task->base,
        e4__F_CHAR_LITERAL | e4__F_NEG_PREFIX | e4__F_BASE_PREFIX, &num);
    if (pcount == (e4__usize)len) {
        e4__compile_literal(task, (e4__cell)num);
        return;
    }

    e4__exception_throw(task, e4__E_UNDEFWORD);
}

static void e4__evaluate_interpret(struct e4__task *task, const char *word,
        e4__u8 len)
{
    register const struct e4__dict_header *header;
    register e4__usize pcount;
    e4__usize num;

    header = e4__dict_lookup(task, word, len);

    if (header) {
        if (header->flags & e4__F_COMPONLY)
            e4__exception_throw(task, e4__E_COMPONLYWORD);
        e4__execute(task, header->xt);
        return;
    }

    /* Attempt to convert to a number and push it onto the stack. */
    pcount = e4__mem_number(word, len, task->base,
        e4__F_CHAR_LITERAL | e4__F_NEG_PREFIX | e4__F_BASE_PREFIX, &num);
    /* FIXME: Should e4__mem_number return a e4__u8? */
    if (pcount == (e4__usize)len) {
        if (e4__stack_avail(task))
            e4__stack_push(task, (e4__cell)num);
        else
            e4__exception_throw(task, e4__E_STKOVERFLOW);
        return;
    }

    e4__exception_throw(task, e4__E_UNDEFWORD);
}

static void e4__evaluate_wrapper(struct e4__task *task, e4__cell user);

static e4__usize e4__evaluate_internal(struct e4__task *task,
        e4__bool force_thunk)
{

    if (!task->exception.valid || force_thunk) {
        /* If there's nothing currently catching exceptions or a thunk
           is being explicitly requested, catch exceptions and return
           them here. */
        static const struct e4__evaluate_thunk thunk = {
            e4__evaluate_wrapper,
            NULL
        };

        return e4__exception_catch(task, (struct e4__execute_token *)&thunk);
    }

    while (task->io_src.in < task->io_src.length) {
        register const char *word;
        register e4__u8 len;

        /* FIXME: Execute builtin lookup here and catch the exception
           rather than doing this? */
        word = e4__io_word(task, ' ');
        len = (e4__u8)*word++;

        if (!len)
            continue;

        if (e4__task_compiling(task))
            e4__evaluate_compile(task, word, len);
        else
            e4__evaluate_interpret(task, word, len);
    }

    return e4__E_OK;
}

static void e4__evaluate_refill_wrapper(struct e4__task *task, e4__cell user);

static e4__usize e4__evaluate_refill(struct e4__task *task,
        e4__bool force_thunk)
{
    if (!task->exception.valid || force_thunk) {
        static const struct e4__evaluate_thunk thunk = {
            e4__evaluate_refill_wrapper,
            NULL
        };

        return e4__exception_catch(task, (struct e4__execute_token *)&thunk);
    }

    /* Based on the context we're calling this from, this should
       *really* never happen, but if it somehow does, just execute
       refill. */
    e4__evaluate_refill_wrapper(task, NULL);

    return e4__E_OK;
}

static void e4__evaluate_refill_wrapper(struct e4__task *task, e4__cell user)
{
    register e4__usize io_res;

    if ((io_res = e4__io_refill(task, NULL)))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

static void e4__evaluate_wrapper(struct e4__task *task, e4__cell user)
{
    e4__evaluate_internal(task, 0);
    e4__execute_ret(task);
}

/* XXX: If evaluate returns an exception code, it is the caller's
   responsibility to clear the data stack and cancel any compilation
   that is currently in progress. */
e4__usize e4__evaluate(struct e4__task *task, const char *buf, e4__usize sz)
{
    struct e4__io_src old_io_src;
    e4__usize res;

    old_io_src = task->io_src;

    task->io_src.buffer = (e4__cell)buf;
    task->io_src.in = 0;
    task->io_src.length = sz != (e4__usize)-1 ? sz : (e4__usize)strlen(buf);
    task->io_src.sz = task->io_src.length;
    task->io_src.sid = e4__SID_STR;

    res = e4__evaluate_internal(task, 0);

    /* Restore old IO src. */
    /* FIXME: Is this *actually* always correct? */
    task->io_src = old_io_src;

    return res;
}

void e4__evaluate_quit(struct e4__task *task)
{
    e4__bool running;

    /* The behavior of this function depends on whether or not we're
       already in some evaluate context. If we are, raise a quit
       exception. If not, perform the QUIT behavior as prescribed
       by the standard. */
    e4__exception_throw(task, e4__E_QUIT);

    /* Okay, we're still here. That means exceptions aren't currently
       enabled and it's time to enter a QUIT loop. */
    running = 1;
    while (running) {
        register e4__usize res;

        /* FIXME: Consider adding an e4__stack_rclear API. */
        task->rp = task->r0;

        /* Reset IO src to TIB. */
        /* XXX: This shouldn't actually be necessary under the current
           implementation, but do it anyway just to be safe. */
        task->io_src.buffer = task->tib;
        task->io_src.sid = e4__SID_UID;
        task->io_src.in = 0;
        task->io_src.length = 0;
        task->io_src.sz = task->tib_sz;

        /* FIXME: Thunk into exceptions being enabled for this call to
           refill. The easiest way to do this may be by extending
           e4__builtin_excute to enable exceptions and return an error
           code. This will allow REPL implementations to raise
           exceptions while e4 is blocked waiting for input within
           the quit loop, which may be desirable in many scenarios. */
        res = e4__evaluate_refill(task, 1);
        res = res ? res : e4__evaluate_internal(task, 1);
        switch (res) {
            case e4__E_OK:
                /* FIXME: Use CR here instead for newline? */
                if (!e4__task_compiling(task))
                    e4__io_type(task, " ok\n", 4);
                /* fall through */
            case e4__E_QUIT:
                break;
            case e4__E_BYE:
                running = 0;
                break;
            case e4__E_ABORTQ:
                if (task->abortq_msg) {
                    e4__io_type(task, " ", 1);
                    e4__io_type(task, task->abortq_msg, task->abortq_msg_len);
                    e4__io_type(task, "\n", 1);

                    task->abortq_msg = NULL;
                    task->abortq_msg_len = 0;
                }
                /* fall through */
            default: {
                struct e4__double d;
                const char *str;
                e4__usize len;

                /* XXX: An uncaught exception clears the stack and
                   cancels any in-progress compilation. */
                e4__stack_clear(task);
                e4__compile_cancel(task);

                e4__io_type(task, " EXCEPTION: ", 12);
                str = e4__usize_format_exception(res, &len);
                e4__io_type(task, str, len);

                /* XXX: Some of these calls can theoretically fail,
                   likely implying that the dictionary is in an overflow
                   state. If this does happen, there isn't anything we
                   can do to handle it anyhow and the worst thing that
                   can happen here if we ignore it is that we'll fail
                   to print exception information correctly. As such,
                   the following lines ignore potential PNO errors. */
                e4__io_pno_start(task);

                e4__io_pno_holds(task, ")\n", 2);
                d = e4__usize_todouble(res);
                e4__io_pno_digits(task, &d, e4__F_SIGNED);
                e4__io_pno_holds(task, " (", 2);

                len = 0;
                e4__io_pno_finish(task, (char **)&str, &len);
                e4__io_type(task, str, len);
             }
        }
    }
}

#if defined(e4__INCLUDE_FILE) || defined(e4__INCLUDE_FILE_EXT)

    /* XXX: Never throws, even when called in an exception context.
       Any exception that occurs while parsing the file is stored in
       *ex. Other exceptions are stored in return value.

       Platform specific file related exception codes can be
       accessed with e4__task_ior. */
    e4__usize e4__evaluate_file(struct e4__task *task, e4__usize fd,
            struct e4__file_exception *fex)
    {
        register struct e4__io_src old_io_src;
        register e4__usize res;
        register e4__bool running;
        register e4__usize line = 0;

        if (task->fib_depth >= e4__FIB_MAXDEPTH)
            return e4__E_INCFOVERFLOW;

        old_io_src = task->io_src;

        task->io_src.buffer = (e4__cell)task->fib[task->fib_depth++];
        task->io_src.sid = fd;
        task->io_src.sz = e4__FIB_SZ;

        running = 1;
        while (running) {
            task->io_src.in = 0;
            task->io_src.length = 0;

            ++line;

            res = e4__evaluate_refill(task, 1);
            res = res ? res : e4__evaluate_internal(task, 1);
            switch (res) {
                case e4__E_EOF:
                    running = 0;
                    /* fall through */
                case e4__E_QUIT:
                    res = e4__E_OK;
                    /* fall through */
                case e4__E_OK:
                    break;
                case e4__E_BYE:
                default:
                    running = 0;
                    break;
            }
        }

        --task->fib_depth;

        if (fex) {
            fex->ex = res;
            fex->line = line;
            fex->path = NULL;
            fex->path_sz = 0;
            fex->buf = ((const char *)task->io_src.buffer) + task->io_src.in;
            fex->buf_sz = task->io_src.length;
        }

        task->io_src = old_io_src;

        return e4__io_file_close(task, fd);
    }

    e4__usize e4__evaluate_path(struct e4__task *task, const char *path,
            e4__usize sz, struct e4__file_exception *fex)
    {
        register e4__usize res;
        e4__usize fd;

        if (task->fib_depth >= e4__FIB_MAXDEPTH)
            return e4__E_INCFOVERFLOW;

        if ((res = e4__io_file_open(task, path, sz, e4__F_READ, &fd)))
            return res;

        res = e4__evaluate_file(task, fd, fex);

        if (fex) {
            fex->path = path;
            fex->path_sz = sz != (e4__usize)-1 ? sz : strlen(path);
        }

        return res;
    }

#endif /* defined(e4__INCLUDE_FILE) || defined(e4__INCLUDE_FILE_EXT) */
