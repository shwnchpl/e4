#include "e4.h"
#include "e4-internal.h"

/* FIXME: For performance reasons, they aren't actually used internally
   where we have access to the fields of struct task. Is this the right
   decision?
   FIXME: Add bounds checking? Probably not. It can be added in Forth
   user facing builtin words. C programmers can do their own, so this
   API probably doesn't need it. */

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
