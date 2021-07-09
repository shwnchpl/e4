#include "e4-debug.h"
#include <string.h>

struct e4__task* e4__task_create(void *buffer, unsigned long size)
{
    struct e4__task *task;

    if (size < e4__MIN_TASK_SZ)
        return NULL;

    /* TODO: Make this optional somehow? */
    memset(buffer, 0, size);

    task = buffer;
    task->sz = size;

    /* TODO: Set task pad and tid. */
    task->pad = NULL;
    task->tib = NULL;

    /* TODO: Set the stack position. */
    task->s0 = NULL;
    task->sp = NULL;

    /* TODO: Set the return stack to its real position and make it grow down. */
    task->r0 = (e4__void)buffer + (size/sizeof(e4__void)) - 64;
    task->rp = (e4__void)buffer + (size/sizeof(e4__void)) - 64;

    task->dict = NULL; /* TODO: Set dict position. */
    task->here = NULL; /* TODO: Set dict position. */

    return task;
}
