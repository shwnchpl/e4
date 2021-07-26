#include "e4-debug.h"
#include "e4.h"
#include "e4-task.h"

/* FIXME: For performance reasons, they aren't actually used internally
   where we have access to the fields of struct task. Is this the right
   decision?
   FIXME: Add bounds checking? Probably not. It can be added in Forth
   user facing builtin words. C programmers can do their own, so this
   API probably doesn't need it. */

e4__usize e4__stack_depth(struct e4__task *task)
{
    return task->s0 - task->sp;
}

void e4__stack_dup(struct e4__task *task)
{
    e4__DEREF(task->sp) = e4__DEREF(task->sp + 1);
    --task->sp;
}

e4__cell e4__stack_peek(struct e4__task *task)
{
    return e4__DEREF(task->sp + 1);
}

e4__cell e4__stack_pop(struct e4__task *task)
{
    return e4__DEREF(++task->sp);
}

void e4__stack_push(struct e4__task *task, void *v)
{
    e4__DEREF(task->sp--) = v;
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

void e4__stack_swap(struct e4__task *task)
{
    register const e4__cell tmp = e4__DEREF(task->sp + 2);
    e4__DEREF(task->sp + 2) = e4__DEREF(task->sp + 1);
    e4__DEREF(task->sp + 1) = tmp;;
}
