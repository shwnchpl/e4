#include <string.h>
#include "e4.h"
#include "e4-task.h"

void e4__task_io_init(struct e4__task *task, struct e4__io_func *io_func)
{
    task->io_func = *io_func;
}

int e4__io_key(struct e4__task *task, void *buf)
{
    if (!task->io_func.key)
        return e4__E_UNSUPPORTED;

    return task->io_func.key(task->io_func.user, buf);
}

int e4__io_accept(struct e4__task *task, char *buf, e4__usize n)
{
    if (!task->io_func.accept)
        return e4__E_UNSUPPORTED;

    return task->io_func.accept(task->io_func.user, buf, n);
}

int e4__io_type(struct e4__task *task, const char *buf, e4__usize n)
{
    if (!task->io_func.type)
        return e4__E_UNSUPPORTED;

    return task->io_func.type(task->io_func.user, buf, n);
}
