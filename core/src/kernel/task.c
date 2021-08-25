#include "e4.h"
#include "e4-internal.h"

#include <string.h>

/* XXX: It is the responsibility of the caller to ensure that HERE
   stays aligned, either by reserving appropriate sizes to maintain
   alignment or by only allowing HERE to stay unaligned very
   temporarily.

   The appropriate way to call this function is almost always one of
   the following:

        e4__task_allot(task, e4__mem_cells(n))
        e4__task_allot(task, e4__mem_aligned(n)) */
e4__cell e4__task_allot(struct e4__task *task, e4__usize sz)
{
    /* FIXME: Actually fail and throw an exception on dictionary
       overflow, then add some allot_unchecked API to fill this
       usecase. If exceptions are off, perhaps return NULL? */
    register const e4__cell old_here = task->here;
    task->here = (e4__cell)((e4__u8 *)task->here + sz);
    return old_here;
}

e4__bool e4__task_compiling(struct e4__task *task)
{
    return !!task->compile.state;
}

struct e4__task* e4__task_create(void *buffer, e4__usize sz)
{
    register struct e4__task *task;
    register const e4__cell c0 = buffer;

    /* Align size to pointer width. */
    sz = sz / sizeof(e4__cell) * sizeof(e4__cell);

    if (sz < e4__TASK_MIN_SZ)
        return NULL;

    memset(buffer, 0, sz);

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
    task->sz = sz;
    task->dict = (struct e4__dict_header *)
            &e4__BUILTIN_HEADER[e4__BUILTIN_COUNT - 1];

    /* struct e4__task is guaranteed to be aligned to at least
       sizeof(e4__cell) (since it contains fields that are
       e4__cell), so this is safe. */

    task->here = c0 + sizeof(*task) / sizeof(e4__cell);
    task->pad = c0 + (70 * sz) / (100 * sizeof(e4__cell));
    task->s0 = c0 + (85 * sz) / (100 * sizeof(e4__cell));
    task->sp = task->s0;
    task->tib = c0 + (85 * sz) / (100 * sizeof(e4__cell)) + 1;
    task->tib_sz = (((e4__u8 *)c0 + (90 * sz) / 100) -
            (e4__u8 *)task->tib - 1);
    task->base_ptr = (e4__cell)&task->base;
    task->io_src.buffer = task->tib;
    task->io_src.sz = task->tib_sz;
    task->r0 = c0 + sz / sizeof(e4__cell) - 1;
    task->rp = task->r0;
    task->tr0 = task->r0;
    task->base = 10;

    return task;
}

void e4__task_io_init(struct e4__task *task, struct e4__io_func *io_func)
{
    task->io_func = *io_func;
}

void e4__task_io_get(struct e4__task *task, struct e4__io_func *io_func)
{
    *io_func = task->io_func;
}

e4__usize e4__task_unused(struct e4__task *task)
{
    /* XXX: Dictionary overflows should be prevented from happening
       elsewhere, but in the event that the dictionary *has* overflowed
       into PAD, report that there is no space available. */
    return (e4__usize)task->pad > (e4__usize)task->here ?
            (e4__usize)(task->pad - task->here) * sizeof(e4__cell) :
            0;
}

e4__cell e4__task_uservar(struct e4__task *task, e4__usize offset)
{
    /* XXX: This function is unsafe. Do not call it with
       an inappropriate offset. */
    return ((e4__cell *)task)[offset];
}
