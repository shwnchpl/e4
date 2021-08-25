/* Core Extensions word list declarations. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

#define _e4__BUILTIN_CORE_EXT_DECL()    \
    _e4__BUILTIN_PROC_F(AGAIN, e4__F_COMPONLY | e4__F_IMMEDIATE)    \
    _e4__BUILTIN_THUNK_NF(BACKSLASH, "\\", e4__F_IMMEDIATE) \
    _e4__BUILTIN_PROC_N(COLON_NONAME, ":NONAME")    \
    _e4__BUILTIN_PROC(DEFER)    \
    _e4__BUILTIN_PROC_N(DEFER_FETCH, "DEFER@")  \
    _e4__BUILTIN_PROC_N(DEFER_STORE, "DEFER!")  \
    _e4__BUILTIN_PROC_NF(DOT_PAREN, ".(", e4__F_IMMEDIATE)  \
    _e4__BUILTIN_THUNK(ERASE)   \
    _e4__BUILTIN_CONSTANT(FALSE, e4__BF_FALSE)  \
    _e4__BUILTIN_THUNK(HEX) \
    _e4__BUILTIN_PROC(MARKER)   \
    _e4__BUILTIN_THUNK(NIP) \
    _e4__BUILTIN_THUNK_N(NOT_EQUALS, "<>")  \
    _e4__BUILTIN_USERVAR(PAD)   \
    _e4__BUILTIN_PROC(PICK) \
    _e4__BUILTIN_PROC_NF(QUESTION_DO, "?DO", e4__F_COMPONLY | e4__F_IMMEDIATE)\
    _e4__BUILTIN_PROC(REFILL)   \
    _e4__BUILTIN_PROC(ROLL) \
    _e4__BUILTIN_PROC_NF(S_BACKSLASH_QUOTE, "S\\\"", \
            e4__F_COMPONLY | e4__F_IMMEDIATE)   \
    _e4__BUILTIN_PROC_F(TO, e4__F_IMMEDIATE)    \
    _e4__BUILTIN_CONSTANT(TRUE, e4__BF_TRUE)    \
    _e4__BUILTIN_PROC(TUCK) \
    _e4__BUILTIN_PROC_N(TWO_R_FETCH, "2R@") \
    _e4__BUILTIN_PROC_N(TWO_R_FROM, "2R>")  \
    _e4__BUILTIN_PROC_N(TWO_TO_R, "2>R")    \
    _e4__BUILTIN_THUNK_N(U_GREATER_THAN, "U>")  \
    _e4__BUILTIN_PROC(VALUE)    \
    _e4__BUILTIN_THUNK_N(ZERO_GREATER, "0>")  \
    _e4__BUILTIN_THUNK_N(ZERO_NOT_EQUALS, "0<>")

/* vim: set ft=c: */
