#include "e4.h"
#include "e4-internal.h"

e4__usize e4__exception_catch(struct e4__task *task, void *xt)
{
    register const e4__cell saved_sp = task->sp;
    register const e4__cell saved_rp = task->rp;
    register const e4__cell saved_tr0 = task->tr0;
    const struct e4__io_src saved_io_src = task->io_src;
    register const e4__usize saved_e = task->exception.code;
    register e4__usize result;
    register e4__bool fired = 0;

    /* XXX: Check whether the current exception is valid before wasting
       stack space preserving it. This code duplication is ugly, but a
       jmp_buf can be relatively large and any half decent compiler
       should optimize the duplication out of existence. */
    if (!task->exception.valid) {
        task->exception.valid = 1;
        if (!setjmp(task->exception.ctx.buf))
            e4__execute(task, xt);
        else
            fired = 1;
        task->exception.valid = 0;
    } else {
        const struct e4__exception_ctx old_ctx = task->exception.ctx;

        if (!setjmp(task->exception.ctx.buf)) {
            e4__execute(task, xt);
        } else {
            task->exception.ctx = old_ctx;
            fired = 1;

            /* If the exception is e4__E_QUIT or e4__E_BYE and we're not
               the last handler, keep going. */
            if (task->exception.code == e4__E_QUIT ||
                    task->exception.code == e4__E_BYE)
                e4__exception_throw(task, task->exception.code);
        }

        task->exception.ctx = old_ctx;
    }

    if (fired) {
        /* FIXME: Document this odd system behavior somewhere. */
        if (task->exception.code != e4__E_QUIT)
            task->sp = saved_sp;
        task->rp = saved_rp;
        task->tr0 = saved_tr0;
        task->io_src = saved_io_src;
        task->ip = NULL;

        if (task->exception.then)
            task->exception.then(task, &task->exception.user);

        task->exception.then = NULL;
        task->exception.user = NULL;
    }

    result = task->exception.code;
    task->exception.code = saved_e;

    return result;
}

void e4__exception_throw(struct e4__task *task, e4__usize e)
{
    e4__exception_throw_then(task, e, NULL, NULL);
}

/* The callback passed as `then` will be executed immediately after
   the exception is caught and stack pointers etc. are restored, before
   returning into any other code. The intention of this API is to
   provide a mechanism for users to throw exceptions from things like
   signal handlers or interrupt service routines then unblock signals
   or re-enable interrupts *after* having jumped out.

   There is no guarantee that exceptions will be enabled when the
   then-callback is excepted so throwing exceptions from the callback
   itself may not work. That said, *attempting* to throw exceptions from
   the then- callback is a supported operation so long as the caller
   does not regard no exception being thrown as an error condition.

   If a provided callback modifies the return stack in any way,
   including by calling e4__execute_ret, the result is unspecified
   behavior.

   If exceptions are disabled or if e is e4__E_OK, no exception will be
   thrown and the then-callback will never be executed. */
void e4__exception_throw_then(struct e4__task *task, e4__usize e,
        e4__code_ptr then, void *user)
{
    /* If this function returns, e was 0 or we couldn't throw
       an exception. */
    if (!e || !task->exception.valid)
        return;
    task->exception.then = then;
    task->exception.user = user;
    task->exception.code = e;
    longjmp(task->exception.ctx.buf, 1);
}
