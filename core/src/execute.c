#include "e4.h"
#include "e4-task.h"

void e4__execute(struct e4__task *task, void *user)
{
    void **code = user;
    e4__code_ptr entry = *code;

    e4__DEREF(task->rp--) = task->ip + 1;
    entry(task, code + 1);
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
        } else if (e4__DEREF(task->ip) == (e4__cell)e4__builtin_RET) {
            depth -= 1;
            task->ip = e4__DEREF(++task->rp);
        } else {
            e4__execute(task, e4__DEREF(task->ip));
        }
    }
}
