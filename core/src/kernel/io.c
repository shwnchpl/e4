#include "e4.h"
#include "e4-internal.h"

#include <string.h>

e4__usize e4__io_accept(struct e4__task *task, char *buf, e4__usize *n)
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

e4__usize e4__io_refill(struct e4__task *task, e4__usize *bf)
{
    register e4__usize io_res;

    if (bf)
        *bf = e4__BF_FALSE;

    /* Unless SOURCE-ID is user input device, just return false. */
    if (task->io_src.sid) {
        return e4__E_OK;
    }

    /* Attempt to read the entire buffer. */
    task->io_src.length = task->io_src.sz;
    io_res = e4__io_accept(task, (char *)task->io_src.buffer,
            &task->io_src.length);

    if (io_res)
        /* We've failed. Say the buffer is empty. */
        task->io_src.length = 0;
    else if (bf)
        *bf = e4__BF_TRUE;

    return io_res;
}

e4__usize e4__io_type(struct e4__task *task, const char *buf, e4__usize n)
{
    if (!task->io_func.type)
        return e4__E_UNSUPPORTED;

    return task->io_func.type(task->io_func.user, buf,
            n != (e4__usize)-1 ? n : strlen(buf));
}

char* e4__io_word(struct e4__task *task, char delim)
{
    register e4__usize length;
    register e4__u8 clamped_length;
    const char *word = NULL;

    /* Parse word. */
    word = (const char *)task->io_src.buffer + (e4__usize)task->io_src.in;

    if ((e4__usize)task->io_src.length > (e4__usize)task->io_src.in)
        length = (e4__usize)task->io_src.length - (e4__usize)task->io_src.in;
    else
        length = 0;

    length = e4__mem_parse(word, delim, length, e4__F_SKIP_LEADING, &word);

    /* XXX: Ambiguous condition. We take only the first 255 bytes of
       a parsed string that is longer than this, but we advance >IN
       to the actual end of the word (beyond the encountered delim),
       as per the specified delim. */

    /* Write word to HERE. */
    /* FIXME: Check that there is space in HERE before writing
       to it. */
    clamped_length = length < 256 ? (e4__u8)length : 255;
    *((e4__u8 *)task->here) = clamped_length;
    memcpy((e4__u8 *)task->here + 1, word, clamped_length);

    /* Update offset and push result. */
    task->io_src.in = word + length + 1 - (const char *)task->io_src.buffer;

    return (char *)task->here;
}