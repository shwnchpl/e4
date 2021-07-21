#include "e4-debug.h"
#include "e4.h"
#include "e4-task.h"

#include <string.h>

struct e4__task* e4__task_create(void *buffer, unsigned long size)
{
    register struct e4__task *task;
    register e4__cell cursor = buffer;

    /* Align size to pointer width. */
    size = size / sizeof(e4__cell) * sizeof(e4__cell);

    if (size < e4__TASK_MIN_SZ)
        return NULL;

    /* TODO: Make this optional somehow? */
    memset(buffer, 0, size);

    /* FIXME: Verify that these fields are all a reasonable size (per
       standard Forth 2012) with the minimum task size e4 will
       allow. */
    /* Allocate space as follows:
        70%-sizeof(*task) of dictionary
        5% pad
        10% parameter stack
        5% TIB
        10% return stack

       To start, none of these will be checked, and none of them may
       ever be checked. Since the user specifies the size, they should
       have a good idea of whether or not they'll encounter issues
       without checking. If they think they will, they should do so
       themselves.

       This is all just tentative. Builtin checks may be added later.
    */

    task = buffer;
    task->sz = size;
    task->dict = (struct e4__dict_header *)
            &e4__BUILTIN_HEADER[e4__BUILTIN_COUNT - 1];

    /* struct e4__task is guaranteed to be aligned to at least
       sizeof(e4__cell) (since it contains fields that are
       e4__cell), so this is safe. */

    task->here = cursor + sizeof(*task) / sizeof(e4__cell);
    task->pad = cursor + (70 * size) / (100 * sizeof(e4__cell));
    task->s0 = cursor + (85 * size) / (100 * sizeof(e4__cell));
    task->sp = task->s0;
    task->tib = cursor + (85 * size) / (100 * sizeof(e4__cell)) + 1;
    task->io_src.buffer = task->tib;
    /* FIXME: Store this size somewhere? Or recalculate it if needed? */
    task->io_src.sz = (e4__cell)(((const char *)cursor + (90 * size ) / 100) -
            (const char*)task->tib - 1);
    task->r0 = cursor + size / sizeof(e4__cell) - 1;
    task->rp = task->r0;
    task->base = (e4__cell)10;

#if 0 /* FIXME: Remove/alter this debug code. */
    printf("\nCREATED TASK:\n"
        "\ttask = %p\n"
        "\ttask->sz = %lu\n"
        "\ttask->dict = %p\n"
        "\ttask->here = %p\n"
        "\ttask->pad = %p\n"
        "\ttask->s0 = %p\n"
        "\ttask->sp = %p\n"
        "\ttask->tib = %p\n"
        "\ttask->io_src.sz = %p\n"
        "\ttask->r0 = %p\n"
        "\ttask->rp = %p\n\n",
        task,
        task->sz,
        task->dict,
        task->here,
        task->pad,
        task->s0,
        task->sp,
        task->tib,
        task->io_src.sz,
        task->r0,
        task->rp
    );
#endif

    return task;
}
