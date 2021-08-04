#include "e4.h"
#include "e4-task.h"

/* FIXME: Rename user to something sane here and in e4__execute. */
e4__usize e4__exception_catch(struct e4__task * task, void *user)
{
    register const e4__cell saved_sp = task->sp;
    register const e4__cell saved_rp = task->rp;
    const struct e4__io_src saved_io_src = task->io_src;
    register const e4__usize saved_e = task->exception_code;
    register e4__usize result;

    /* Check whether the current exception is valid before wasting stack
       space preserving it. */
    if (!task->exception_valid) {
        task->exception_valid = 1;
        if (!setjmp(task->exception.ctx))
            e4__execute(task, user);
        else {
            task->sp = saved_sp;
            task->rp = saved_rp;
            task->io_src = saved_io_src;
        }
        task->exception_valid = 0;
    } else {
        const struct e4__exception_ctx old_ex = task->exception;

        if (!setjmp(task->exception.ctx)) {
            e4__execute(task, user);
        } else {
            /* If the exception is e4__E_QUIT or e4__E_BYE and we're not
               the last handler, keep going. */
            task->exception = old_ex;

            /* FIXME: Should BYE be implemented with some flag
               elsewhere so that QUIT is the only exception with this
               behavior? */
            if (task->exception_code == e4__E_QUIT ||
                    task->exception_code == e4__E_BYE)
                e4__exception_throw(task, task->exception_code);

            task->sp = saved_sp;
            task->rp = saved_rp;
            task->io_src = saved_io_src;
        }

        task->exception = old_ex;
    }

    result = task->exception_code;
    task->exception_code = saved_e;

    return result;
}

void e4__exception_throw(struct e4__task *task, e4__usize e)
{
    /* If this function returns, e was 0 or we couldn't throw
       an exception. */
    if (!e || !task->exception_valid)
        return;
    task->exception_code = e;
    longjmp(task->exception.ctx, 1);
}
