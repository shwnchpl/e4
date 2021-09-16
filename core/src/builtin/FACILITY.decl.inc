/* Facility word list declarations. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

#define _e4__BUILTIN_FACILITY_DECL()    \
    _e4__BUILTIN_THUNK_N(AT_XY, "AT-XY")    \
    _e4__BUILTIN_PROC_N(KEY_QUESTION, "KEY?")   \
    _e4__BUILTIN_THUNK(PAGE)

/* vim: set ft=c: */
