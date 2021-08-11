/* System word list declarations. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

#define _e4__BUILTIN_SYSTEM_DECL()    \
    _e4__BUILTIN_PROC_F(BRANCH, e4__F_COMPONLY)     \
    _e4__BUILTIN_PROC_F(BRANCH0, e4__F_COMPONLY)    \
    _e4__BUILTIN_PROC(CLEAR)    \
    _e4__BUILTIN_PROC_F(DLITERAL, e4__F_COMPONLY)   \
    _e4__BUILTIN_PROC_F(SENTINEL, e4__F_IMMEDIATE)  \
    _e4__BUILTIN_PROC_F(SKIP, e4__F_COMPONLY)

/* vim: set ft=c: */
