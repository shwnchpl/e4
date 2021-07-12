#include "e4-debug.h"
#include "e4.h"
#include "e4-task.h"

/* FIXME: For performance reasons, they aren't actually used internally
   where we have access to the fields of struct task. Is this the right
   decision?
   FIXME: Add bounds checking? Probably not. It can be added in Forth
   user facing builtin words. C programmers can do their own, so this
   API probably doesn't need it. */

void e4__stack_push(struct e4__task *task, void *v)
{
    e4__DEREF(task->sp--) = v;
}

e4__cell e4__stack_pop(struct e4__task *task)
{
    return e4__DEREF(++task->sp);
}

e4__cell e4__stack_peek(struct e4__task *task)
{
    return e4__DEREF(task->sp + 1);
}

unsigned long e4__stack_depth(struct e4__task *task)
{
    return task->s0 - task->sp;
}

void e4__stack_rpush(struct e4__task *task, void *v)
{
    e4__DEREF(task->rp--) = v;
}

e4__cell e4__stack_rpop(struct e4__task *task)
{
    return e4__DEREF(++task->rp);
}

e4__cell e4__stack_rpeek(struct e4__task *task)
{
    return e4__DEREF(task->rp + 1);
}
