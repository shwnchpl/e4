#include "e4.h"
#include "e4-task.h"

#include <stdarg.h>
#include <string.h>

/* utility macros */

#define _e4__BUILTIN_EXPECT_DEPTH(t, c) \
    do {    \
        register const e4__usize _c = (e4__usize)(c);   \
        if (e4__stack_depth(t) < _c) { \
            e4__exception_throw(t, e4__E_STKUNDERFLOW);  \
            e4__execute_ret(task);  \
            return; \
        }   \
    } while (0)

#define _e4__BUILTIN_LOOKAHEAD(t, w, l) \
    do {    \
        e4__builtin_exec(t, e4__B_WORD, (e4__usize)' ');    \
        w = (const char *)e4__stack_pop(t); \
        if (!(l = (e4__u8)*w++)) {  \
            e4__exception_throw(t, e4__E_ZLNAME);   \
            e4__execute_ret(t);  \
            return; \
        }   \
    } while (0)

/* builtin dictionary definitions */

/* FIXME: Make some of these conditional here and elsewhere in this
   file. */
#include "builtin/CORE.decl.inc"
#include "builtin/CORE-EXT.decl.inc"
#include "builtin/SYSTEM.decl.inc"
#include "builtin/TOOLS.decl.inc"
#include "builtin/TOOLS-EXT.decl.inc"

#define _e4__BUILTIN_PROC(w)    \
    _e4__BUILTIN_PROC_NF(w, #w, 0)

#define _e4__BUILTIN_PROC_N(w, n)   \
    _e4__BUILTIN_PROC_NF(w, n, 0)

#define _e4__BUILTIN_PROC_F(w, f)   \
    _e4__BUILTIN_PROC_NF(w, #w, f)

/* Declare builtin functions. */
#define _e4__BUILTIN_PROC_FIRST(w)  _e4__BUILTIN_PROC(w)
#define _e4__BUILTIN_PROC_NF(w, n, f)   \
    static void e4__builtin_##w(struct e4__task *task, void *user);

#define _e4__BUILTIN_CONSTANT(w, c)
#define _e4__BUILTIN_USERVAR(w)

_e4__BUILTIN_CORE_DECL();
_e4__BUILTIN_CORE_EXT_DECL();
_e4__BUILTIN_SYSTEM_DECL();
_e4__BUILTIN_TOOLS_DECL();
_e4__BUILTIN_TOOLS_EXT_DECL();

#undef _e4__BUILTIN_USERVAR
#undef _e4__BUILTIN_CONSTANT
#undef _e4__BUILTIN_PROC_NF
#undef _e4__BUILTIN_PROC_FIRST

/* Define builtin header table. */
#define _e4__BUILTIN_PROC_HEADER(w, link, n, f) \
    {   \
        link,   \
        (struct e4__execute_token *)&e4__BUILTIN_XT[e4__B_##w],     \
        e4__F_BUILTIN | (f),    \
        sizeof(n) - 1, \
        n, \
    },
#define _e4__BUILTIN_PROC_FIRST(w)  \
    _e4__BUILTIN_PROC_HEADER(w, NULL, #w, 0)
#define _e4__BUILTIN_PROC_NF(w, n, f)   \
    _e4__BUILTIN_PROC_HEADER(w, \
            (struct e4__dict_header *)&e4__BUILTIN_HEADER[e4__B_##w - 1],   \
            n, f)
#define _e4__BUILTIN_CONSTANT(w, c) \
    _e4__BUILTIN_PROC_F(w, e4__F_CONSTANT)
#define _e4__BUILTIN_USERVAR(w) \
    _e4__BUILTIN_PROC(w)

const struct e4__dict_header e4__BUILTIN_HEADER[e4__BUILTIN_COUNT] =
{
    _e4__BUILTIN_CORE_DECL()
    _e4__BUILTIN_CORE_EXT_DECL()
    _e4__BUILTIN_SYSTEM_DECL()
    _e4__BUILTIN_TOOLS_DECL()
    _e4__BUILTIN_TOOLS_EXT_DECL()
};

#undef _e4__BUILTIN_USERVAR
#undef _e4__BUILTIN_CONSTANT
#undef _e4__BUILTIN_PROC_NF
#undef _e4__BUILTIN_PROC_FIRST
#undef _e4__BUILTIN_PROC_HEADER

/* Define builtin execution token table. */
#define _e4__BUILTIN_PROC_FIRST(w)  _e4__BUILTIN_PROC(w)
#define _e4__BUILTIN_PROC_NF(w, n, f)   \
    {e4__builtin_##w, NULL},
#define _e4__BUILTIN_CONSTANT(w, c) \
    {e4__execute_value, NULL, { (e4__cell)((e4__usize)(c)), }},
#define _e4__BUILTIN_USERVAR(w) \
    {e4__execute_uservar, NULL, {(e4__cell)((e4__usize)(e4__UV_##w))}},

const struct e4__execute_token e4__BUILTIN_XT[e4__BUILTIN_COUNT] =
{
    _e4__BUILTIN_CORE_DECL()
    _e4__BUILTIN_CORE_EXT_DECL()
    _e4__BUILTIN_SYSTEM_DECL()
    _e4__BUILTIN_TOOLS_DECL()
    _e4__BUILTIN_TOOLS_EXT_DECL()
};

#undef _e4__BUILTIN_USERVAR
#undef _e4__BUILTIN_CONSTANT
#undef _e4__BUILTIN_PROC_FIRST
#undef _e4__BUILTIN_PROC_F
#undef _e4__BUILTIN_PROC_N
#undef _e4__BUILTIN_PROC
#undef _e4__BUILTIN_DECL

#undef _e4__BUILTIN_TOOLS_EXT_DECL
#undef _e4__BUILTIN_TOOLS_DECL
#undef _e4__BUILTIN_SYSTEM_DECL
#undef _e4__BUILTIN_CORE_EXT_DECL
#undef _e4__BUILTIN_CORE_DECL

static const void *_e4__BUILTIN_RETURN_THUNK[] =
{
    &e4__BUILTIN_XT[e4__B_EXIT]
};

/* builtin utilities */

/* task and id are mandatory, otherwise this function simply does
   nothing. Executes the builtin corresponding to id on the
   specified task after pushing any remaining arguments onto the
   stack as cells. These arguments should be explicitly cast
   to either e4__cell or e4__usize if they are not already one
   of those types. */
void e4__builtin_exec_(e4__usize count, /* struct e4__task *task, */
        /* enum e4__builtin_id id, */ ...)
{
    va_list ap;
    register struct e4__task *task;
    register enum e4__builtin_id id;

    if (count < 2)
        return;

    va_start(ap, count);

    task = va_arg(ap, struct e4__task *);
    id = va_arg(ap, enum e4__builtin_id);
    count -= 2;

    while (count--)
        e4__stack_push(task, va_arg(ap, e4__cell));

    va_end(ap);

    e4__stack_rpush(task, NULL);
    e4__BUILTIN_XT[id].code(task, NULL);
}

/* builtin implementations */

#include "builtin/CORE.def.inc"
#include "builtin/CORE-EXT.def.inc"
#include "builtin/SYSTEM.def.inc"
#include "builtin/TOOLS.def.inc"
#include "builtin/TOOLS-EXT.def.inc"
