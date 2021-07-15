#include <string.h>
#include "e4.h"
#include "e4-task.h"

void e4__task_io_init(struct e4__task *task, struct e4__io *io)
{
    task->io = *io;
}

int e4__io_key(struct e4__task *task, void *buf)
{
    if (!task->io.key)
        return e4__E_UNSUPPORTED;

    return task->io.key(task->io.user, buf);
}

int e4__io_accept(struct e4__task *task, char *buf, unsigned long n)
{
    if (!task->io.accept)
        return e4__E_UNSUPPORTED;

    return task->io.accept(task->io.user, buf, n);
}

int e4__io_type(struct e4__task *task, const char *buf, unsigned long n)
{
    if (!task->io.type)
        return e4__E_UNSUPPORTED;

    return task->io.type(task->io.user, buf, n);
}
