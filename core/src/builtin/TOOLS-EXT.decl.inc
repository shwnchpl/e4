/* Tools Extensions word list declarations. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

#define _e4__BUILTIN_TOOLS_EXT_DECL()    \
    _e4__BUILTIN_PROC_NF(BRACKET_ELSE, "[ELSE]", e4__F_IMMEDIATE)   \
    _e4__BUILTIN_PROC_NF(BRACKET_IF, "[IF]", e4__F_IMMEDIATE)   \
    _e4__BUILTIN_ALIAS_NF(BRACKET_THEN, SENTINEL, "[THEN]", e4__F_IMMEDIATE)  \
    _e4__BUILTIN_PROC(BYE)  \
    _e4__BUILTIN_PROC(FORGET)

/* vim: set ft=c: */
