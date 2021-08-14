#include "e4.h"
#include "e4-internal.h"

/* FIXME: Rename user to something more reasonable here. */
void e4__execute(struct e4__task *task, void *user)
{
    register const e4__bool ip_valid = task->ip != NULL;
    void **code = user;
    e4__code_ptr entry = *code;

    /* If the instruction pointer is currently NULL, simply return
       to NULL. */
    e4__DEREF(task->rp--) = ip_valid ? task->ip + 1 : NULL;

    entry(task, code + 1);

    /* If ip was NULL, restore it to NULL. */
    if (!ip_valid)
        task->ip = NULL;
}

void e4__execute_deferthunk(struct e4__task *task, void *user)
{
    /* Execute whatever execution token is pointed to by the
       user pointer. */
    e4__execute(task, e4__DEREF(user));
}

void e4__execute_doesthunk(struct e4__task *task, void *user)
{
    /* Push the address of the data pointer onto the stack then
       branch to the address in user. */
    e4__stack_push(task, (e4__cell)user + 1);
    e4__execute_threaded(task, e4__DEREF(user) - 1);
}

void e4__execute_ret(struct e4__task *task)
{
    task->ip = e4__DEREF(++task->rp);
}

void e4__execute_threaded(struct e4__task *task, void *user)
{
    register int depth = 1;

    task->ip = (e4__cell)user + 1;

    while (depth) {
        if (e4__DEREF2(task->ip) == (e4__cell)e4__execute_threaded) {
            depth += 1;
            e4__DEREF(task->rp--) = task->ip + 1;
            task->ip = e4__DEREF(task->ip) + 2;
        } else if (e4__DEREF(task->ip) ==
                (e4__cell)&e4__BUILTIN_XT[e4__B_EXIT]) {
            depth -= 1;
            task->ip = e4__DEREF(++task->rp);
        } else {
            e4__execute(task, e4__DEREF(task->ip));
        }
    }
}

void e4__execute_userval(struct e4__task *task, void *user)
{
    register const e4__cell val = e4__DEREF(user);
    e4__stack_push(task, val);
    e4__execute_ret(task);
}

void e4__execute_uservar(struct e4__task *task, void *user)
{
    register const e4__cell uv_offset = e4__DEREF(user);
    e4__stack_push(task, e4__task_uservar(task, (e4__usize)uv_offset));
    e4__execute_ret(task);
}

void e4__execute_value(struct e4__task *task, void *user)
{
    e4__stack_push(task, e4__DEREF((e4__cell)user + 1));
    e4__execute_ret(task);
}

void e4__execute_variable(struct e4__task *task, void *user)
{
    e4__stack_push(task, (e4__cell)user + 1);
    e4__execute_ret(task);
}
