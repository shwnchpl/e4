#include "e4.h"
#include "e4-task.h"

#include <string.h>

struct e4__evaluate_thunk {
    e4__code_ptr code;
    void *padding;
    const char *buf;
    e4__usize sz;
    e4__u8 mode;
};

static void e4__evaluate_wrapper(struct e4__task *task, void *user)
{
    struct e4__evaluate_thunk *thunk =
            (struct e4__evaluate_thunk *)((e4__cell)user - 1);

    e4__evaluate(task, thunk->buf, thunk->sz, thunk->mode);

    /* Simply pop from the return stack. ip probably isn't valid
       anyhow, but why risk it? */
    e4__stack_rpop(task);
}

e4__usize e4__evaluate(struct e4__task *task, const char *buf, e4__usize sz,
        e4__u8 mode)
{
    /* FIXME: Actually handle other modes. Figure out how this part
       of the API should be structured. */
    struct e4__io_src old_io_src;

    if (!task->exception_valid) {
        /* If there's nothing currently catching exceptions, catch and
           return them here. */
        struct e4__evaluate_thunk thunk;

        thunk.code = e4__evaluate_wrapper;
        thunk.buf = buf;
        thunk.sz = sz;
        thunk.mode = mode;

        return e4__exception_catch(task, (struct e4__execute_token *)&thunk);
    }

    old_io_src = task->io_src;

    /* FIXME: Hack. Use mode somehow instead? */
    if ((e4__cell)buf != task->tib) {
        task->io_src.buffer = (e4__cell)buf;
        task->io_src.in = 0;
        task->io_src.length = sz != (e4__usize)-1 ? sz : (e4__usize)strlen(buf);
        task->io_src.sz = task->io_src.length;
        task->io_src.sid = e4__SID_STR;
    }

    while (task->io_src.in < task->io_src.length) {
        register struct e4__dict_header *header;
        register const char *word;
        register e4__u8 len;
        register e4__usize pcount;
        e4__usize num;

        /* FIXME: Expose WORD in some way so that it isn't necessary
           to call it like this? */
        e4__builtin_exec(task, e4__B_WORD, (e4__usize)' ');

        /* FIXME: Execute builtin lookup here and catch the exception
           rather than doing this? */
        word = (const char *)e4__stack_pop(task);
        len = (e4__u8)*word++;
        header = e4__mem_dict_lookup(task->dict, word, len);

        if (header) {
            e4__execute(task, header->xt);
            continue;
        }

        /* Attempt to convert to a number and push it onto the stack. */
        pcount = e4__mem_number(word, len, task->base,
            e4__F_CHAR_LITERAL | e4__F_NEG_PREFIX | e4__F_BASE_PREFIX, &num);
        /* FIXME: Should e4__mem_number return a e4__u8? */
        if (pcount == (e4__usize)len) {
            e4__stack_push(task, (e4__cell)num);
            continue;
        }

        /* FIXME: Raise some kind of exception; don't just fail
           silently. */
        e4__exception_throw(task, e4__E_UNDEFWORD);
    }

    /* Restore old IO src. */
    /* FIXME: Is this *actually* always correct? */
    task->io_src = old_io_src;

    return 0;
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

        task->rp = task->r0;

        /* FIXME: Reset IO src to TIB etc. This shouldn't actually be
           necessary. */

        /* FIXME: Create e4__io_refill function and use that. */
        e4__builtin_exec(task, e4__B_REFILL);
        e4__stack_pop(task);

        res = e4__evaluate(task, (const char *)task->io_src.buffer,
                task->io_src.length, 0);
        switch (res) {
            case e4__E_OK:
                /* FIXME: Use CR here instead for newline? */
                e4__io_type(task, " ok\n", 4);
                /* fall through */
            case e4__E_QUIT:
                break;
            case e4__E_BYE:
                running = 0;
                break;
            default: {
                /* FIXME: Improve this handling once better string
                   formatting is available. */
                register e4__usize len;
                register char *buf = (char *)task->here;
                register char *num;

                /* XXX: An uncaught exception clears the stack. */
                e4__stack_clear(task);

                e4__io_type(task, " EXCEPTION: ", 12);
                num = e4__num_format(res, task->base, e4__F_SIGNED, buf, 130);
                len = &buf[130] - num;
                num[len++] = '\n';
                e4__io_type(task, num, len);
             }
        }
    }
}
