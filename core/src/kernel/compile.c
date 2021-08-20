#include "e4.h"
#include "e4-internal.h"

#include <string.h>

#define _e4__COMP_ANONYMOUS  ( 4 )

void e4__compile_cancel(struct e4__task *task)
{
    /* Resume compilation if it has been suspended. If it hasn't, this
       is a no-op. */
    e4__compile_resume(task);

    /* Nothing to do if this was just anonymous compilation. */
    if (task->compile.state == _e4__COMP_ANONYMOUS) {
        task->compile.state = 0;
        return;
    }

    /* Attempt to restore HERE to a reasonable state. */
    switch (task->compile.state) {
        case 0:
            return;
        case e4__COMP_COLON:
            /* Forget the most recently defined word. */
            e4__dict_forget(task, task->dict->name, task->dict->nbytes);
            break;
        case e4__COMP_NONAME:
            /* Set here to the provided xt. */
            task->here = (e4__cell)task->compile.xt;
            break;
        case e4__COMP_DOES:
        default:
            /* Simply restore HERE to whatever it was when
               compilation started. */
            task->here = task->compile.here0;
            break;
    };

    task->compile.state = 0;
}

void e4__compile_cell(struct e4__task *task, e4__cell cell)
{
    e4__DEREF(task->here++) = cell;
}

void e4__compile_char(struct e4__task *task, e4__cell cell)
{
    register e4__u8 *c = (e4__u8 *)task->here;
    *c++ = (e4__u8)((e4__usize)cell & 0xff);
    task->here = (e4__cell)c;
}

void e4__compile_cstr(struct e4__task *task, const char *str, e4__u8 len)
{
    /* Copy the string to the correct position *first*, since we may be
       about to write over it. */
    memmove(((e4__u8 *)(task->here + 1)) + 1, str, len);
    e4__DEREF(task->here++) = (e4__cell)&e4__BUILTIN_XT[e4__B_LIT_CSTR];
    *((e4__u8 *)task->here) = len;
    task->here = (e4__cell)e4__mem_aligned((e4__usize)task->here + len + 1);
}

void e4__compile_estr(struct e4__task *task, const char *str, e4__usize len)
{
    const char *chunk;
    e4__usize chunk_len;
    char scratch[2];
    char *buffer = (char *)(task->here + 2);

    /* Copy the string to the correct position *first*, since we may be
       about to write over it. */
    while ((chunk = e4__mem_strnescape(&str, &len, &chunk_len, scratch))) {
        memmove(buffer, chunk, chunk_len);
        buffer += chunk_len;
    }

    len = buffer - (const char *)(task->here + 2);

    e4__DEREF(task->here++) = (e4__cell)&e4__BUILTIN_XT[e4__B_LIT_STR];
    e4__DEREF(task->here++) = (e4__cell)len;
    task->here = (e4__cell)e4__mem_aligned((e4__usize)task->here + len);
}

e4__usize e4__compile_finish(struct e4__task *task)
{
    /* Resume compilation if it has been suspended. If it hasn't, this
       is a no-op. */
    e4__compile_resume(task);

    /* Nothing to do if this was just anonymous compilation. */
    if (task->compile.state == _e4__COMP_ANONYMOUS) {
        task->compile.state = 0;
        return e4__E_OK;
    }

    if (task->compile.s0 != task->sp) {
        e4__compile_cancel(task);
        return e4__E_CSMISMATCH;
    }

    if (task->compile.state == e4__COMP_DOES) {
        task->compile.xt->code = e4__execute_doesthunk;
        task->compile.xt->user = task->compile.here0;
    } else {
        if (task->compile.state == e4__COMP_NONAME)
            e4__stack_push(task, (e4__cell)task->compile.xt);
        else if (task->compile.state == e4__COMP_COLON)
            task->dict->flags &= ~e4__F_COMPILING;
        task->compile.xt->code = e4__execute_threaded;
    }

    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_EXIT]);
    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_SENTINEL]);

    task->compile.state = 0;

    return e4__E_OK;
}

void e4__compile_literal(struct e4__task *task, e4__cell lit)
{
    e4__DEREF(task->here++) = (e4__cell)&e4__BUILTIN_XT[e4__B_LIT_CELL];
    e4__DEREF(task->here++) = lit;
}

void e4__compile_resume(struct e4__task *task)
{
    if (task->compile.state)
        return;
    task->compile.state = task->compile.sus_state ? task->compile.sus_state :
            _e4__COMP_ANONYMOUS;
    task->compile.sus_state = 0;
}

void e4__compile_start(struct e4__task *task, struct e4__execute_token *xt,
        e4__usize mode)
{
    /* FIXME: Check for nested compilation and throw an error. This can
       happen if we're either already compiling (and this is being
       executed via some immediate word). */

    /* XXX: We do not catch and handle errors that result from beginning
       compiling (or ALLOTing in in general, for that matter) when
       compilation is suspended. */

    /* XXX: The whole point of setting the appropriate fields in XT at
       the end of the compilation process is that ideally a poorly
       constructed DOES> shouldn't destroy an existing dictionary
       entry. In theory, since DOES> only applies to the most recent
       execution token, we *could* get by with just a mode parameter,
       but then we wouldn't be able to reset HERE correctly in the
       event of a :NONAME compilation failure. */
    task->compile.state = mode;
    task->compile.s0 = task->sp;
    task->compile.here0 = task->here;
    task->compile.xt = xt;
}

void e4__compile_str(struct e4__task *task, const char *str, e4__usize len)
{
    /* Copy the string to the correct position *first*, since we may be
       about to write over it. */
    memmove(task->here + 2, str, len);
    e4__DEREF(task->here++) = (e4__cell)&e4__BUILTIN_XT[e4__B_LIT_STR];
    e4__DEREF(task->here++) = (e4__cell)len;
    task->here = (e4__cell)e4__mem_aligned((e4__usize)task->here + len);
}

void e4__compile_suspend(struct e4__task *task)
{
    task->compile.sus_state = task->compile.state;
    task->compile.state = 0;
}
