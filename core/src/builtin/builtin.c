#include "e4.h"
#include "../kernel/e4-internal.h"

#include <string.h>

#if defined(e4__INCLUDE_FFI)
    #include <ffi.h>
#endif

/* utility macros */

#define _e4__BUILTIN_EXPECT_AVAIL(t, c) \
    do {    \
        register const e4__usize _c = (e4__usize)(c);   \
        if (e4__stack_avail(t) < _c) { \
            e4__exception_throw(t, e4__E_STKOVERFLOW);  \
            e4__execute_ret(task);  \
            return; \
        }   \
    } while (0)

#define _e4__BUILTIN_EXPECT_DEPTH(t, c) \
    do {    \
        register const e4__usize _c = (e4__usize)(c);   \
        if (e4__stack_depth(t) < _c) { \
            e4__exception_throw(t, e4__E_STKUNDERFLOW);  \
            e4__execute_ret(task);  \
            return; \
        }   \
    } while (0)

#define _e4__BUILTIN_EXPECT_RAVAIL(t, c) \
    do {    \
        register const e4__usize _c = (e4__usize)(c);   \
        if (e4__stack_ravail(t) < _c) { \
            e4__exception_throw(t, e4__E_RSTKOVERFLOW); \
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
            e4__compile_cancel(t);  \
            e4__exception_throw(t, e4__E_CSMISMATCH);   \
            e4__execute_ret(t); \
            return; \
        }   \
    } while (0)

#define _e4__BUILTIN_LOOKAHEAD(t, w, l) \
    do {    \
        w = e4__io_word(task, ' '); \
        if (!(l = (e4__u8)*w++)) {  \
            e4__exception_throw(t, e4__E_ZLNAME);   \
            e4__execute_ret(t);  \
            return; \
        }   \
    } while (0)

static const void *_e4__BUILTIN_RETURN_THUNK[] =
{
    &e4__BUILTIN_XT[e4__B_EXIT]
};

/* builtin implementations */

#include "CORE.def.inc"
#include "SYSTEM.def.inc"

#if defined(e4__INCLUDE_CORE_EXT)
    #include "CORE-EXT.def.inc"
#endif
#if defined(e4__INCLUDE_DLFCN)
    #include "DLFCN.def.inc"
#endif
#if defined(e4__INCLUDE_EXCEPTION)
    #include "EXCEPTION.def.inc"
#endif
#if defined(e4__INCLUDE_FACILITY)
    #include "FACILITY.def.inc"
#endif
#if defined(e4__INCLUDE_FACILITY_EXT)
    #include "FACILITY-EXT.def.inc"
#endif
#if defined(e4__INCLUDE_FFI)
    #include "FFI.def.inc"
#endif
#if defined(e4__INCLUDE_FILE)
    #include "FILE.def.inc"
#endif
#if defined(e4__INCLUDE_TOOLS)
    #include "TOOLS.def.inc"
#endif
#if defined(e4__INCLUDE_TOOLS_EXT)
    #include "TOOLS-EXT.def.inc"
#endif

/* builtin dictionary definitions */

#include "CORE.decl.inc"
#include "SYSTEM.decl.inc"

#if defined(e4__INCLUDE_CORE_EXT)
    #include "CORE-EXT.decl.inc"
#endif
#if defined(e4__INCLUDE_DLFCN)
    #include "DLFCN.decl.inc"
#endif
#if defined(e4__INCLUDE_EXCEPTION)
    #include "EXCEPTION.decl.inc"
#endif
#if defined(e4__INCLUDE_FACILITY)
    #include "FACILITY.decl.inc"
#endif
#if defined(e4__INCLUDE_FACILITY_EXT)
    #include "FACILITY-EXT.decl.inc"
#endif
#if defined(e4__INCLUDE_FFI)
    #include "FFI.decl.inc"
#endif
#if defined(e4__INCLUDE_FILE)
    #include "FILE.decl.inc"
#endif
#if defined(e4__INCLUDE_TOOLS)
    #include "TOOLS.decl.inc"
#endif
#if defined(e4__INCLUDE_TOOLS_EXT)
    #include "TOOLS-EXT.decl.inc"
#endif

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
    _e4__BUILTIN_PUN_NF(w, t, #w, 0)

#define _e4__BUILTIN_PUN_N(w, t, n) \
    _e4__BUILTIN_PUN_NF(w, t, n, 0)

#define _e4__BUILTIN_PUN_F(w, t, f) \
    _e4__BUILTIN_PUN_NF(w, t, #w, f)

#define _e4__BUILTIN_ALIAS(w, t)    \
    _e4__BUILTIN_ALIAS_NF(w, t, #w, 0)

#define _e4__BUILTIN_ALIAS_N(w, t, n)   \
    _e4__BUILTIN_ALIAS_NF(w, t, n, 0)

#define _e4__BUILTIN_ALIAS_F(w, t, f)   \
    _e4__BUILTIN_ALIAS_NF(w, t, #w, f)

#define _e4__BUILTIN_USERVAR(w) \
    _e4__BUILTIN_USERVAR_N(w, #w)

#define _e4__BUILTIN_CONSTANT(w, c) \
    _e4__BUILTIN_CONSTANT_N(w, #w, c)

#if defined(e4__INCLUDE_FFI)
    #define _e4__BUILTIN_FFICONST(w, n, ft, ct) \
        _e4__BUILTIN_CONSTANT_N(w, n, \
                (sizeof(e4__cell) >= sizeof(ct) ? ft : NULL))
#endif

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
#define _e4__BUILTIN_ALIAS_NF(w, t, n, f)   \
    _e4__BUILTIN_PROC_NF(w, n, f)
#define _e4__BUILTIN_CONSTANT_N(w, n, c) \
    _e4__BUILTIN_PROC_NF(w, n, e4__F_CONSTANT)
#define _e4__BUILTIN_USERVAR_N(w, n)    \
    _e4__BUILTIN_PROC_N(w, n)

const struct e4__dict_header e4__BUILTIN_HEADER[e4__BUILTIN_COUNT] =
{
    _e4__BUILTIN_CORE_DECL()
    _e4__BUILTIN_SYSTEM_DECL()

    #if defined(e4__INCLUDE_CORE_EXT)
        _e4__BUILTIN_CORE_EXT_DECL()
    #endif
    #if defined(e4__INCLUDE_DLFCN)
        _e4__BUILTIN_DLFCN_DECL()
    #endif
    #if defined(e4__INCLUDE_EXCEPTION)
        _e4__BUILTIN_EXCEPTION_DECL()
    #endif
    #if defined(e4__INCLUDE_FACILITY)
        _e4__BUILTIN_FACILITY_DECL()
    #endif
    #if defined(e4__INCLUDE_FACILITY_EXT)
        _e4__BUILTIN_FACILITY_EXT_DECL()
    #endif
    #if defined(e4__INCLUDE_FFI)
        _e4__BUILTIN_FFI_DECL()
    #endif
    #if defined(e4__INCLUDE_FILE)
        _e4__BUILTIN_FILE_DECL()
    #endif
    #if defined(e4__INCLUDE_TOOLS)
        _e4__BUILTIN_TOOLS_DECL()
    #endif
    #if defined(e4__INCLUDE_TOOLS_EXT)
        _e4__BUILTIN_TOOLS_EXT_DECL()
    #endif
};

#undef _e4__BUILTIN_USERVAR_N
#undef _e4__BUILTIN_CONSTANT_N
#undef _e4__BUILTIN_ALIAS_NF
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
    {e4__execute_threadedthunk, (void *)e4__builtin_##w},
#define _e4__BUILTIN_PUN_NF(w, t, n, f) \
    {e4__execute_deferthunk, (void *)&e4__BUILTIN_XT[e4__B_##t]},
#define _e4__BUILTIN_ALIAS_NF(w, t, n, f)   \
    {e4__builtin_##t, NULL},
#define _e4__BUILTIN_CONSTANT_N(w, n, c) \
    {e4__execute_userval, (void *)((e4__usize)(c))},
#define _e4__BUILTIN_USERVAR_N(w, n)    \
    {e4__execute_uservar, (void *)((e4__usize)(e4__UV_##w))},

const struct e4__execute_tuple e4__BUILTIN_XT[e4__BUILTIN_COUNT] =
{
    _e4__BUILTIN_CORE_DECL()
    _e4__BUILTIN_SYSTEM_DECL()

    #if defined(e4__INCLUDE_CORE_EXT)
        _e4__BUILTIN_CORE_EXT_DECL()
    #endif
    #if defined(e4__INCLUDE_DLFCN)
        _e4__BUILTIN_DLFCN_DECL()
    #endif
    #if defined(e4__INCLUDE_EXCEPTION)
        _e4__BUILTIN_EXCEPTION_DECL()
    #endif
    #if defined(e4__INCLUDE_FACILITY)
        _e4__BUILTIN_FACILITY_DECL()
    #endif
    #if defined(e4__INCLUDE_FACILITY_EXT)
        _e4__BUILTIN_FACILITY_EXT_DECL()
    #endif
    #if defined(e4__INCLUDE_FFI)
        _e4__BUILTIN_FFI_DECL()
    #endif
    #if defined(e4__INCLUDE_FILE)
        _e4__BUILTIN_FILE_DECL()
    #endif
    #if defined(e4__INCLUDE_TOOLS)
        _e4__BUILTIN_TOOLS_DECL()
    #endif
    #if defined(e4__INCLUDE_TOOLS_EXT)
        _e4__BUILTIN_TOOLS_EXT_DECL()
    #endif
};

#undef _e4__BUILTIN_USERVAR_N
#undef _e4__BUILTIN_CONSTANT_N
#undef _e4__BUILTIN_ALIAS_NF
#undef _e4__BUILTIN_PUN_NF
#undef _e4__BUILTIN_THUNK_NF
#undef _e4__BUILTIN_PROC_NF
#undef _e4__BUILTIN_PROC_FIRST
#if defined(e4__INCLUDE_FFI)
    #undef _e4__BUILTIN_FFICONST
#endif
#undef _e4__BUILTIN_CONSTANT
#undef _e4__BUILTIN_USERVAR
#undef _e4__BUILTIN_ALIAS_F
#undef _e4__BUILTIN_ALIAS_N
#undef _e4__BUILTIN_ALIAS
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
#undef _e4__BUILTIN_CORE_EXT_DECL
#undef _e4__BUILTIN_SYSTEM_DECL
#undef _e4__BUILTIN_CORE_DECL

/* builtin utilities */

e4__usize e4__builtin_exec(struct e4__task *task, enum e4__builtin_id id)
{
    /* If exceptions aren't enabled, run the builtin with them enabled
       and return any errors that are caught in this way. */
    if (!task->exception.valid)
        return e4__exception_catch(task, (void *)&e4__BUILTIN_XT[id]);

    /* If exceptions are enabled, simply execute the builtin. If we
       make it back here we know we can return okay. If we don't, the
       exception has already been thrown for us. */
    e4__execute(task, (void *)&e4__BUILTIN_XT[id]);

    return e4__E_OK;
}
