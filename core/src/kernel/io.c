#include "e4.h"
#include "e4-internal.h"

#include <string.h>

e4__usize e4__io_accept(struct e4__task *task, char *buf, e4__usize *n)
{
    if (!task->io_func.accept)
        return e4__E_UNSUPPORTED;

    return task->io_func.accept(task->io_func.user, buf, n);
}

e4__usize e4__io_dump(struct e4__task *task, e4__cell p, e4__usize len)
{
    const char *buf = (const char *)p;
    e4__usize io_res;

    while (len) {
        const e4__usize line_len = e4__mem_dump(&buf, &len,
                (char *)task->here);
        if ((io_res = e4__io_type(task, (const char *)task->here, line_len)))
            return io_res;
    }

    return e4__E_OK;
}

e4__usize e4__io_key(struct e4__task *task, char *buf)
{
    if (!task->io_func.key)
        return e4__E_UNSUPPORTED;

    return task->io_func.key(task->io_func.user, buf);
}

e4__usize e4__io_parse(struct e4__task *task, char delim, e4__usize flags,
        const char **out)
{
    register e4__usize length;
    const char *word = NULL;

    /* Parse word. */
    word = (const char *)task->io_src.buffer + task->io_src.in;

    if (task->io_src.length > task->io_src.in)
        length = task->io_src.length - task->io_src.in;
    else
        length = 0;

    length = e4__mem_parse(word, delim, length, flags, &word);
    task->io_src.in = word + length + 1 - (const char *)task->io_src.buffer;

    *out = word;

    return length;
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

    task->io_src.in = 0;

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
    length = e4__io_parse(task, delim, e4__F_SKIP_LEADING, &word);

    /* XXX: Ambiguous condition. We take only the first 255 bytes of
       a parsed string that is longer than this, but we advance >IN
       to the actual end of the word (beyond the encountered delim),
       as per the specified delim (via the call to e4__io_parse). */

    /* Write word to HERE. */
    /* FIXME: Check that there is space in HERE before writing
       to it. */
    clamped_length = length < 256 ? (e4__u8)length : 255;
    *((e4__u8 *)task->here) = clamped_length;
    memcpy((e4__u8 *)task->here + 1, word, clamped_length);


    return (char *)task->here;
}
