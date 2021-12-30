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

e4__usize e4__io_pno_digit(struct e4__task *task, struct e4__double *ud)
{
    register e4__usize res;
    char *b = (char *)task->pno.offset;

    if (!task->pno.offset)
        return e4__E_INVMEMADDR;

    if ((e4__usize)task->pno.offset < (e4__usize)task->here)
        return e4__E_PNOOVERFLOW;

    res = e4__mem_pno_digit(&b, (e4__u8)task->base, ud);
    task->pno.offset = (e4__cell)b;

    return res;
}

e4__usize e4__io_pno_digits(struct e4__task *task, struct e4__double *ud,
        e4__u8 flags)
{
    register e4__usize res;
    char *b = (char *)task->pno.offset;
    register e4__usize avail_len;

    if (!task->pno.offset)
        return e4__E_INVMEMADDR;

    if ((e4__usize)task->pno.offset < (e4__usize)task->here)
        return e4__E_PNOOVERFLOW;
    avail_len = (e4__usize)task->pno.offset - (e4__usize)task->here - 1;

    res = e4__mem_pno_digits(&b, avail_len, (e4__u8)task->base, flags, ud);
    task->pno.offset = (e4__cell)b;

    return res;
}

e4__usize e4__io_pno_finish(struct e4__task *task, char **out_buf,
        e4__usize *out_sz)
{
    register e4__usize res = e4__E_OK;

    if (!task->pno.offset)
        return e4__E_INVMEMADDR;

    /* If some allocation has happened since we last wrote to the PNO
       buffer and we now no longer own memory that had been included in
       the buffer and written to, fail.

       It is possible that memory in the buffer has been trashed in some
       other way (such as with allocation followed by deallocation), but
       in such cases at least the buffer returned will be safely mutable
       which is a hard requirement. Part of the PNO buffer is currently
       part of the dictionary, it is not the case that it can be mutated
       freely and misrepresenting that by returning a pointer into that
       memory as mutable here could lead to far more catestrophic
       failures than a string containing gibberish. */
    if ((e4__usize)task->pno.offset < (e4__usize)task->here - 1)
        res = e4__E_PNOOVERFLOW;
    else {
        *out_buf = (char *)task->pno.offset + 1;
        *out_sz = (char *)task->pno.end - (char *)task->pno.offset;
    }

    task->pno.end = NULL;
    task->pno.offset = NULL;

    return res;
}

e4__usize e4__io_pno_hold(struct e4__task *task, char c)
{
    char *b = (char *)task->pno.offset;

    if (!task->pno.offset)
        return e4__E_INVMEMADDR;

    if ((e4__usize)task->pno.offset < (e4__usize)task->here)
        return e4__E_PNOOVERFLOW;

    e4__mem_pno_hold(&b, c);
    task->pno.offset = (e4__cell)b;

    return e4__E_OK;
}

e4__usize e4__io_pno_holds(struct e4__task *task, const char *s, e4__usize len)
{
    char *b = (char *)task->pno.offset;

    if (!task->pno.offset)
        return e4__E_INVMEMADDR;

    if ((e4__usize)task->pno.offset - len < (e4__usize)task->here - 1)
        return e4__E_PNOOVERFLOW;

    e4__mem_pno_holds(&b, s, len);
    task->pno.offset = (e4__cell)b;

    return e4__E_OK;
}

void e4__io_pno_start(struct e4__task *task)
{
    task->pno.end = (e4__cell)((char *)task->pad - 1);
    task->pno.offset = task->pno.end;
}

e4__usize e4__io_refill(struct e4__task *task, e4__usize *bf)
{
    register e4__usize io_res;
    register e4__bool attempt;

    if (bf)
        *bf = e4__BF_FALSE;

    /* Unless SOURCE-ID is user input device or a file (when relevant),
       just return false. */
    #if defined(e4__INCLUDE_FILE) || defined(e4__INCLUDE_FILE_EXT)
        attempt = (task->io_src.sid < (e4__usize)e4__SID_STR);
    #else
        attempt = !task->io_src.sid;
    #endif

    if (!attempt)
        return e4__E_OK;

    task->io_src.in = 0;

    /* Attempt to read the entire buffer. */
    task->io_src.length = task->io_src.sz;

    #if defined(e4__INCLUDE_FILE) || defined(e4__INCLUDE_FILE_EXT)
        if (task->io_src.sid != e4__SID_UID)
            io_res = e4__io_file_readline(task, task->io_src.sid,
                    (char *)task->io_src.buffer, &task->io_src.length);
        else
    #endif
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

#if defined(e4__INCLUDE_FACILITY)

    e4__usize e4__io_keyq(struct e4__task *task, e4__usize *bflag)
    {
        if (!task->io_func.keyq)
            return e4__E_UNSUPPORTED;

        return task->io_func.keyq(task->io_func.user, bflag);
    }

#endif /* defined(e4__INCLUDE_FACILITY) */

#if defined(e4__INCLUDE_FACILITY_EXT)

    e4__usize e4__io_ms(struct e4__task *task, e4__usize ms)
    {
        if (!task->io_func.ms)
            return e4__E_UNSUPPORTED;

        return task->io_func.ms(task->io_func.user, ms);
    }

    e4__usize e4__io_unixtime(struct e4__task *task, e4__usize *t)
    {
        /* XXX: The unixtime handler is expected to set t to the
           positive number of seconds elapsed from the Unix epoch at the
           real time when the handler is invoked. */

        if (!task->io_func.unixtime)
            return e4__E_UNSUPPORTED;

        return task->io_func.unixtime(task->io_func.user, t);
    }

#endif /* defined(e4__INCLUDE_FACILITY_EXT) */

#if defined(e4__INCLUDE_FILE) || defined(e4__INCLUDE_FILE_EXT)

    e4__usize e4__io_file_close(struct e4__task *task, e4__usize fd)
    {
        register e4__usize res;
        e4__usize ior = 0;

        if (!task->io_func.file_close)
            return e4__E_UNSUPPORTED;

        res = task->io_func.file_close(task->io_func.user, fd, &ior);
        e4__task_ior(task, ior);

        return res;
    }

    e4__usize e4__io_file_open(struct e4__task *task, const char *path,
            e4__usize sz, e4__usize perm, e4__usize *fd)
    {
        register e4__usize res;
        e4__usize ior = 0;

        if (!task->io_func.file_open)
            return e4__E_UNSUPPORTED;

        res = task->io_func.file_open(task->io_func.user, path,
                sz != (e4__usize)-1 ? sz : strlen(path), perm, fd, &ior);
        e4__task_ior(task, ior);

        return res;
    }

    e4__usize e4__io_file_read(struct e4__task *task, e4__usize fd, char *buf,
            e4__usize *n)
    {
        register e4__usize res;
        e4__usize ior = 0;

        if (!task->io_func.file_read)
            return e4__E_UNSUPPORTED;

        res = task->io_func.file_read(task->io_func.user, fd, buf, n, &ior);
        e4__task_ior(task, ior);

        return res;
    }

    e4__usize e4__io_file_readline(struct e4__task *task, e4__usize fd,
            char *buf, e4__usize *n)
    {
        register e4__usize res, r_total = 0;
        e4__usize r_now = 1;
        char ch = '\0';

        /* Read until the end of the line, discarding anything
           in excess of the number of characters requested. */
        /* FIXME: Use platform independent newline rather than
           just \n? */
        while (ch != '\n') {
            res = e4__io_file_read(task, fd, &ch, &r_now);

            if (r_total < *n && r_now)
                buf[r_total++] = ch;

            if (res == e4__E_EOF) {
                if (r_total > 0)
                    res = e4__E_OK;
                break;
            }
            else if (res)
                return res;
            else if (!r_now) {
                /* XXX: This should never happen. If r_now has not been
                   set to indicate a read has occurred, res should be
                   some IO error. This indicates a bug in the user
                   provided file_read handler. */
                return e4__E_BUG;
            }
        }

        *n = r_total;

        return res;
    }

#endif /* defined(e4__INCLUDE_FILE) || defined(e4__INCLUDE_FILE_EXT) */

#if defined(e4__INCLUDE_FILE)

    e4__usize e4__io_file_create(struct e4__task *task, const char *path,
            e4__usize sz, e4__usize perm, e4__usize *fd)
    {
        register e4__usize res;
        e4__usize ior = 0;

        if (!task->io_func.file_create)
            return e4__E_UNSUPPORTED;

        res = task->io_func.file_create(task->io_func.user, path,
                sz != (e4__usize)-1 ? sz : strlen(path), perm, fd, &ior);
        e4__task_ior(task, ior);

        return res;
    }

    e4__usize e4__io_file_delete(struct e4__task *task, const char *path,
            e4__usize sz)
    {
        register e4__usize res;
        e4__usize ior = 0;

        if (!task->io_func.file_delete)
            return e4__E_UNSUPPORTED;

        res = task->io_func.file_delete(task->io_func.user, path,
                sz != (e4__usize)-1 ? sz : strlen(path), &ior);
        e4__task_ior(task, ior);

        return res;
    }

    e4__usize e4__io_file_position(struct e4__task *task, e4__usize fd,
            e4__usize *pos)
    {
        register e4__usize res;
        e4__usize ior = 0;

        if (!task->io_func.file_position)
            return e4__E_UNSUPPORTED;

        res = task->io_func.file_position(task->io_func.user, fd, pos, &ior);
        e4__task_ior(task, ior);

        return res;
    }

    e4__usize e4__io_file_reposition(struct e4__task *task, e4__usize fd,
            e4__usize pos)
    {
        register e4__usize res;
        e4__usize ior = 0;

        if (!task->io_func.file_reposition)
            return e4__E_UNSUPPORTED;

        res = task->io_func.file_reposition(task->io_func.user, fd, pos, &ior);
        e4__task_ior(task, ior);

        return res;
    }

    e4__usize e4__io_file_resize(struct e4__task *task, e4__usize fd,
            e4__usize sz)
    {
        register e4__usize res;
        e4__usize ior = 0;

        if (!task->io_func.file_resize)
            return e4__E_UNSUPPORTED;

        res = task->io_func.file_resize(task->io_func.user, fd, sz, &ior);
        e4__task_ior(task, ior);

        return res;
    }

    e4__usize e4__io_file_size(struct e4__task *task, e4__usize fd,
            e4__usize *sz)
    {
        register e4__usize res;
        e4__usize ior = 0;

        if (!task->io_func.file_size)
            return e4__E_UNSUPPORTED;

        res = task->io_func.file_size(task->io_func.user, fd, sz, &ior);
        e4__task_ior(task, ior);

        return res;
    }

#endif /* defined(e4__INCLUDE_FILE) */

#if defined(e4__INCLUDE_FILE_EXT)

    e4__usize e4__io_file_flush(struct e4__task *task, e4__usize fd)
    {
        register e4__usize res;
        e4__usize ior = 0;

        if (!task->io_func.file_flush)
            return e4__E_UNSUPPORTED;

        res = task->io_func.file_flush(task->io_func.user, fd, &ior);
        e4__task_ior(task, ior);

        return res;
    }

    e4__usize e4__io_file_rename(struct e4__task *task, const char *old_path,
            e4__usize old_sz, const char *new_path, e4__usize new_sz)
    {
        register e4__usize res;
        e4__usize ior = 0;

        if (!task->io_func.file_rename)
            return e4__E_UNSUPPORTED;

        res = task->io_func.file_rename(task->io_func.user, old_path,
                old_sz != (e4__usize)-1 ? old_sz : strlen(old_path),
                new_path, new_sz != (e4__usize)-1 ? new_sz : strlen(new_path),
                &ior);
        e4__task_ior(task, ior);

        return res;
    }

#endif /* defined(e4__INCLUDE_FILE_EXT) */
