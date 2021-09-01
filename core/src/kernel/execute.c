#include "e4.h"
#include "e4-internal.h"

/* XXX: Although e4__execute does expect a pointer to something that
   resembles an execute token or execute tuple, really whatever is
   passed must only appear to be an execute tuple when accessed in
   that way. To indicate the extent to which this is a supported
   operation, the type of xt is void * rather than struct execute_token
   *, struct execute_tuple *, or anything other restrictive type that
   would require a cast. */
void e4__execute(struct e4__task *task, void *xt)
{
    register const e4__bool ip_valid = task->ip != NULL;
    struct e4__execute_tuple *xtup = xt;

    /* If the instruction pointer is currently NULL, simply return
       to NULL. */
    e4__DEREF(task->rp--) = ip_valid ? task->ip + 1 : NULL;

    xtup->code(task, &xtup->user);

    /* If ip was NULL, restore it to NULL. */
    if (!ip_valid)
        task->ip = NULL;
}

void e4__execute_deferthunk(struct e4__task *task, e4__cell user)
{
    /* Execute whatever execution token is pointed to by the
       user pointer. */
    e4__execute(task, e4__DEREF(user));
    e4__execute_ret(task);
}

void e4__execute_doesthunk(struct e4__task *task, e4__cell user)
{
    /* Push the address of the data pointer onto the stack then
       branch to the address in user. */
    e4__stack_push(task, (e4__cell)user + 1);
    e4__execute_threaded(task, e4__DEREF(user) - 1);
}

void e4__execute_marker(struct e4__task *task, e4__cell user)
{
    const struct e4__dict_header *header =
            (struct e4__dict_header *)e4__DEREF(user);

    /* Restore dictionary state to just before this marker was
       created. */
    task->dict = header->link;
    task->here = (e4__cell)header;

    e4__execute_ret(task);
}

void e4__execute_ret(struct e4__task *task)
{
    if (e4__stack_rdepth(task) < 1) {
        static const void *return_thunk[] = {
            &e4__BUILTIN_XT[e4__B_EXIT]
        };

        e4__exception_throw(task, e4__E_RSTKUNDERFLOW);

        /* If we're still running here, we're in a very bad situation.
           The return stack is empty but we've been asked to return, so
           it is unclear how/where execution should continue.

           In a last ditch effort to recover and at least make our way
           out of e4 code, push several a return thunk onto the return
           stack and hope for the best. If one thunk is not sufficient,
           we will end up here again. */

        /* XXX: There is *no way* for user code to detect that this has
           happened. If you want safe code, always ensure that
           exceptions are enabled. Exceptions are always enabled when
           executing via the e4__evaluate family of functions. To
           enable exceptions prior to calling a function in the
           e4__execute family directly outside any evaluation context
           (ie. where a call to evaluate is not somehow triggering the
           call to the e4__execute function), employ an exception thunk
           technique as is used in kernel/evaluate.c. */
        e4__stack_rpush(task, (e4__cell)return_thunk);
    }

    task->ip = e4__stack_rpop(task);
}

void e4__execute_threaded(struct e4__task *task, e4__cell user)
{
    register int depth = 1;

    e4__stack_rpush(task, task->tr0);
    task->tr0 = task->rp;

    task->ip = (e4__cell)user + 1;

    /* FIXME: Consider making it optional to disable return stack
       coherence tracking to save RAM. */

    while (depth) {
        if (e4__DEREF2(task->ip) ==
                (e4__cell)(e4__usize)e4__execute_threaded) {
            depth += 1;

            /* Stash expected thread base return pointer for this
               thread on the return stack. Can assume it's valid and
               restore it if it matches what we expect when we
               encounter it again. */
            e4__stack_rpush(task, task->ip + 1);
            e4__stack_rpush(task, task->tr0);
            task->tr0 = task->rp;

            /* Begin executing thread. */
            task->ip = e4__DEREF(task->ip) + 2;
        } else if (e4__DEREF(task->ip) ==
                (e4__cell)&e4__BUILTIN_XT[e4__B_EXIT]) {
            depth -= 1;

            /* Check and restore thread return stack base pointer. */
            if (depth) {
                if (task->rp != task->tr0) {
                    e4__exception_throw(task, e4__E_RSTKIMBALANCE);
                    task->rp = task->tr0;
                }
                task->tr0 = e4__stack_rpop(task);
                e4__execute_ret(task);
            }
        } else {
            e4__execute(task, e4__DEREF(task->ip));
        }
    }

    if (task->rp != task->tr0) {
        e4__exception_throw(task, e4__E_RSTKIMBALANCE);
        task->rp = task->tr0;
    }

    task->tr0 = e4__stack_rpop(task);
    e4__execute_ret(task);
}

void e4__execute_threadedthunk(struct e4__task *task, e4__cell user)
{
    /* Branch to the address in user. */
    e4__execute_threaded(task, e4__DEREF(user) - 1);
}

void e4__execute_userval(struct e4__task *task, e4__cell user)
{
    register const e4__cell val = e4__DEREF(user);
    e4__stack_push(task, val);
    e4__execute_ret(task);
}

void e4__execute_uservar(struct e4__task *task, e4__cell user)
{
    register const e4__cell uv_offset = e4__DEREF(user);
    e4__stack_push(task, e4__task_uservar(task, (e4__usize)uv_offset));
    e4__execute_ret(task);
}

void e4__execute_value(struct e4__task *task, e4__cell user)
{
    e4__stack_push(task, e4__DEREF(user + 1));
    e4__execute_ret(task);
}

void e4__execute_variable(struct e4__task *task, e4__cell user)
{
    e4__stack_push(task, user + 1);
    e4__execute_ret(task);
}
