#include "e4-debug.h"
#include "e4.h"
#include "e4-task.h"

void e4__execute(struct e4__task *task, void *next)
{
    void **code = next;
    void (*entry)(struct e4__task *, void *) = *code;

    e4__DEREF(task->rp--) = task->ip + 1;
    entry(task, code + 1);
}

void e4__execute_threaded(struct e4__task *task, void *next)
{
    register int depth = 1;

    task->ip = next;

    printf("In e4__execute_threaded...: %p\n", e4__execute_threaded);

    while (depth) {
        printf("Branching (%d) to %p, return addr: %p\n",
            depth, e4__DEREF2(task->ip), e4__DEREF(task->ip) + 1);
        if (e4__DEREF2(task->ip) == (e4__cell)e4__execute_threaded) {
            depth += 1;
            e4__DEREF(task->rp--) = task->ip + 1;
            task->ip = e4__DEREF(task->ip) + 1;
        }
        else if (e4__DEREF(task->ip) == (e4__cell)e4__builtin_return) {
            depth -= 1;
            task->ip = e4__DEREF(++task->rp);
            printf("Inline returning to %p\n", task->ip);
        } else {
            printf("Executing...%p\n", task->ip);
            e4__execute(task, e4__DEREF(task->ip));
        }
    }
}
