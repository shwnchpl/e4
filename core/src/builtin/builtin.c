#include "e4.h"
#include "../kernel/e4-internal.h"

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

#define _e4__BUILTIN_EXPECT_RDEPTH(t, c)    \
    do {    \
        register const e4__usize _c = (e4__usize)(c);   \
        if (e4__stack_rdepth(task) < _c) { \
            e4__exception_throw(t, e4__E_RSTKUNDERFLOW);    \
            e4__execute_ret(task);  \
            return; \
        }   \
    } while (0)

#define _e4__BUILTIN_EXPECT_REF(t, c)   \
    do {    \
        if (e4__stack_depth(t) < c) {   \
            e4__exception_throw(t, e4__E_CSMISMATCH);   \
            e4__execute_ret(t); \
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
#include "CORE.decl.inc"
#include "CORE-EXT.decl.inc"
#include "SYSTEM.decl.inc"
#include "TOOLS.decl.inc"
#include "TOOLS-EXT.decl.inc"

#define _e4__BUILTIN_PROC(w)    \
    _e4__BUILTIN_PROC_NF(w, #w, 0)

#define _e4__BUILTIN_PROC_N(w, n)   \
    _e4__BUILTIN_PROC_NF(w, n, 0)

#define _e4__BUILTIN_PROC_F(w, f)   \
    _e4__BUILTIN_PROC_NF(w, #w, f)

#define _e4__BUILTIN_THUNK(w)   \
    _e4__BUILTIN_THUNK_NF(w, #w, 0)

#define _e4__BUILTIN_THUNK_N(w, n)  \
    _e4__BUILTIN_THUNK_NF(w, n, 0)

#define _e4__BUILTIN_THUNK_F(w, f)  \
    _e4__BUILTIN_THUNK_NF(w, #w, f)

#define _e4__BUILTIN_PUN(w, t)  \
    _e4__BUILTIN_PUN_NF(w, t, t, #w, 0)

#define _e4__BUILTIN_PUN_N(w, t, n) \
    _e4__BUILTIN_PUN_NF(w, t, n, 0)

#define _e4__BUILTIN_PUN_F(w, t, f) \
    _e4__BUILTIN_PUN_NF(w, t, #w, f)

/* Declare builtin functions. */
#define _e4__BUILTIN_PROC_FIRST(w)  _e4__BUILTIN_PROC(w)
#define _e4__BUILTIN_PROC_NF(w, n, f)   \
    static void e4__builtin_##w(struct e4__task *task, void *user);
#define _e4__BUILTIN_THUNK_NF(w, n, f)  \
    static const void *e4__builtin_##w[];
#define _e4__BUILTIN_PUN_NF(w, t, n, f)

#define _e4__BUILTIN_CONSTANT(w, c)
#define _e4__BUILTIN_USERVAR(w)

_e4__BUILTIN_CORE_DECL();
_e4__BUILTIN_CORE_EXT_DECL();
_e4__BUILTIN_SYSTEM_DECL();
_e4__BUILTIN_TOOLS_DECL();
_e4__BUILTIN_TOOLS_EXT_DECL();

#undef _e4__BUILTIN_USERVAR
#undef _e4__BUILTIN_CONSTANT
#undef _e4__BUILTIN_PUN_NF
#undef _e4__BUILTIN_THUNK_NF
#undef _e4__BUILTIN_PROC_NF
#undef _e4__BUILTIN_PROC_FIRST

/* Define builtin header table. */
#define _e4__BUILTIN_PROC_HEADER(w, link, n, f) \
    {   \
        link,   \
        (struct e4__execute_token *)&e4__BUILTIN_XT[e4__B_##w], \
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
#define _e4__BUILTIN_THUNK_NF(w, n, f)  \
    _e4__BUILTIN_PROC_NF(w, n, f)
#define _e4__BUILTIN_PUN_NF(w, t, n, f) \
    _e4__BUILTIN_PROC_NF(w, n, f)
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
#undef _e4__BUILTIN_PUN_NF
#undef _e4__BUILTIN_THUNK_NF
#undef _e4__BUILTIN_PROC_NF
#undef _e4__BUILTIN_PROC_FIRST
#undef _e4__BUILTIN_PROC_HEADER

/* Define builtin execution token table. */
#define _e4__BUILTIN_PROC_FIRST(w)  _e4__BUILTIN_PROC(w)
#define _e4__BUILTIN_PROC_NF(w, n, f)   \
    {e4__builtin_##w, NULL},
#define _e4__BUILTIN_THUNK_NF(w, n, f)  \
    {e4__execute_deferthunk, e4__builtin_##w},
#define _e4__BUILTIN_PUN_NF(w, t, n, f) \
    {e4__execute_deferthunk, (void *)&e4__BUILTIN_XT[e4__B_##t]},
#define _e4__BUILTIN_CONSTANT(w, c) \
    {e4__execute_userval, (void *)((e4__usize)(c))},
#define _e4__BUILTIN_USERVAR(w) \
    {e4__execute_uservar, (void *)((e4__usize)(e4__UV_##w))},

const struct e4__execute_tuple e4__BUILTIN_XT[e4__BUILTIN_COUNT] =
{
    _e4__BUILTIN_CORE_DECL()
    _e4__BUILTIN_CORE_EXT_DECL()
    _e4__BUILTIN_SYSTEM_DECL()
    _e4__BUILTIN_TOOLS_DECL()
    _e4__BUILTIN_TOOLS_EXT_DECL()
};

#undef _e4__BUILTIN_USERVAR
#undef _e4__BUILTIN_CONSTANT
#undef _e4__BUILTIN_PUN_NF
#undef _e4__BUILTIN_THUNK_NF
#undef _e4__BUILTIN_PROC_NF
#undef _e4__BUILTIN_PROC_FIRST
#undef _e4__BUILTIN_PUN_F
#undef _e4__BUILTIN_PUN_N
#undef _e4__BUILTIN_PUN
#undef _e4__BUILTIN_THUNK_F
#undef _e4__BUILTIN_THUNK_N
#undef _e4__BUILTIN_THUNK
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
    register unsigned id;

    if (count < 2)
        return;

    va_start(ap, count);

    task = va_arg(ap, struct e4__task *);
    id = va_arg(ap, unsigned);
    count -= 2;

    while (count--)
        e4__stack_push(task, va_arg(ap, e4__cell));

    va_end(ap);

    e4__stack_rpush(task, NULL);
    e4__BUILTIN_XT[id].code(task, NULL);
}

/* builtin implementations */

#include "CORE.def.inc"
#include "CORE-EXT.def.inc"
#include "SYSTEM.def.inc"
#include "TOOLS.def.inc"
#include "TOOLS-EXT.def.inc"
