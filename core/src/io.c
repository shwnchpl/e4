#include <string.h>
#include "e4.h"
#include "e4-task.h"

e4__usize e4__io_accept(struct e4__task *task, char *buf, e4__usize n)
{
    if (!task->io_func.accept)
        return e4__E_UNSUPPORTED;

    return task->io_func.accept(task->io_func.user, buf, n);
}

e4__usize e4__io_key(struct e4__task *task, void *buf)
{
    if (!task->io_func.key)
        return e4__E_UNSUPPORTED;

    return task->io_func.key(task->io_func.user, buf);
}

e4__usize e4__io_type(struct e4__task *task, const char *buf, e4__usize n)
{
    if (!task->io_func.type)
        return e4__E_UNSUPPORTED;

    return task->io_func.type(task->io_func.user, buf,
            n != (e4__usize)-1 ? n : strlen(buf));
}
