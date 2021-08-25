/* Exception word list declarations. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

#define _e4__BUILTIN_EXCEPTION_DECL()    \
    _e4__BUILTIN_PROC(CATCH)    \
    _e4__BUILTIN_PROC(THROW)

/* vim: set ft=c: */
