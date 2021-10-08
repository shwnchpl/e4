#include "e4.h"
#include "e4-internal.h"

/* XXX: For performance and compatibility reasons, none of these
   functions perform bounds checking internally, nor do they throw
   any exceptions. When writing user functions, it is critical to
   confirm that there is data/space on the stack/return-stack before
   pushing/popping. The e4__stack_avail, e4__stack_depth,
   e4__stack_ravail, and e4__stack_rdepth APIs are provided for this
   purpose.

   It is, of course, not necessary to perform a check before *every*
   stack manipulation if the manipulation is known to be safe based
   on the results of some prior check. For example, if some user
   function needs to pop two values off the stack and subsequently
   push two values onto it, that user function need only check that
   stack depth is greater than or equal to two before popping anything,
   since there must be space for at least two values on the stack
   after two values have been popped from the stack.

   This assumes, of course, that the stack is in a coherent state when
   the user function in question has been invoked. This assumption is
   reasonable and if the stack is ever *not* in a reasonable state as
   a result of code internal to e4, that indicates a bug within e4.

   Even if these functions were to throw exceptions themselves, e4 code
   is intended to execute correctly even when exceptions are disabled,
   so explicit checking (either of stack depth or some hypothetical
   error return value) would still be necessary anyhow. */

e4__usize e4__stack_avail(struct e4__task *task)
{
    return (e4__usize)(task->sp - task->sl + 1);
}

void e4__stack_clear(struct e4__task *task)
{
    task->sp = task->s0;
}

e4__usize e4__stack_depth(struct e4__task *task)
{
    return task->s0 - task->sp;
}

void e4__stack_drop(struct e4__task *task)
{
    ++task->sp;
}

void e4__stack_dup(struct e4__task *task)
{
    e4__DEREF(task->sp) = e4__DEREF(task->sp + 1);
    --task->sp;
}

void e4__stack_over(struct e4__task *task)
{
    e4__DEREF(task->sp) = e4__DEREF(task->sp + 2);
    --task->sp;
}

e4__cell e4__stack_peek(struct e4__task *task)
{
    return e4__DEREF(task->sp + 1);
}

e4__cell e4__stack_pick(struct e4__task *task, e4__usize u)
{
    return e4__DEREF(task->sp + u + 1);
}

e4__cell e4__stack_pop(struct e4__task *task)
{
    return e4__DEREF(++task->sp);
}

void e4__stack_push(struct e4__task *task, void *v)
{
    e4__DEREF(task->sp--) = v;
}

void e4__stack_rot(struct e4__task *task)
{
    register const e4__cell tmp = e4__DEREF(task->sp + 3);
    e4__DEREF(task->sp + 3) = e4__DEREF(task->sp + 2);
    e4__DEREF(task->sp + 2) = e4__DEREF(task->sp + 1);
    e4__DEREF(task->sp + 1) = tmp;
}

e4__usize e4__stack_ravail(struct e4__task *task)
{
    return (e4__usize)(task->rp - task->rl + 1);
}

e4__usize e4__stack_rdepth(struct e4__task *task)
{
    return task->tr0 - task->rp;
}

e4__cell e4__stack_rpeek(struct e4__task *task)
{
    return e4__DEREF(task->rp + 1);
}

e4__cell e4__stack_rpop(struct e4__task *task)
{
    return e4__DEREF(++task->rp);
}

void e4__stack_rpush(struct e4__task *task, void *v)
{
    e4__DEREF(task->rp--) = v;
}

/* XXX: Core extension. */
void e4__stack_roll(struct e4__task *task)
{
    register e4__usize u = (e4__usize)e4__stack_pop(task);
    register const e4__cell tmp = e4__DEREF(task->sp + 1 + u);

    while (u > 0) {
        e4__DEREF(task->sp + 1 + u) = e4__DEREF(task->sp + u);
        --u;
    }

    e4__DEREF(task->sp + 1) = tmp;
}

void e4__stack_swap(struct e4__task *task)
{
    register const e4__cell tmp = e4__DEREF(task->sp + 2);
    e4__DEREF(task->sp + 2) = e4__DEREF(task->sp + 1);
    e4__DEREF(task->sp + 1) = tmp;;
}

/* XXX: Core extension. */
void e4__stack_tuck(struct e4__task *task)
{
    e4__stack_swap(task);
    e4__stack_over(task);
}
