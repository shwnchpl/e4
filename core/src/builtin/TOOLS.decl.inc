/* Tools word list declarations. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

#define _e4__BUILTIN_TOOLS_DECL()    \
    _e4__BUILTIN_PROC_N(DOT_S, ".S")   \
    _e4__BUILTIN_PROC(DUMP) \
    _e4__BUILTIN_THUNK_N(QUESTION, "?") \
    _e4__BUILTIN_PROC(SEE)  \
    _e4__BUILTIN_PROC(WORDS)

/* vim: set ft=c: */
