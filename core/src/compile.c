#include "e4.h"
#include "e4-task.h"

void e4__compile_cancel(struct e4__task *task)
{
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

e4__usize e4__compile_finish(struct e4__task *task)
{
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
    e4__DEREF(task->here++) = (e4__cell)&e4__BUILTIN_XT[e4__B_LITERAL];
    e4__DEREF(task->here++) = lit;
}

void e4__compile_start(struct e4__task *task, struct e4__execute_token *xt,
        e4__usize mode)
{
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
