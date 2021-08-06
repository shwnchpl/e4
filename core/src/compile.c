#include "e4.h"
#include "e4-task.h"

void e4__compile_cell(struct e4__task *task, e4__cell cell)
{
    e4__DEREF(task->here++) = cell;
}

void e4__compile_literal(struct e4__task *task, e4__cell lit)
{
    e4__DEREF(task->here++) = (e4__cell)&e4__BUILTIN_XT[e4__B_LITERAL];
    e4__DEREF(task->here++) = lit;
}
