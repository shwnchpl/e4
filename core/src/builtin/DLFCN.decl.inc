/* DLCFN word list declarations. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

#define _e4__BUILTIN_DLFCN_DECL()   \
    _e4__BUILTIN_PROC(DLCLOSE)  \
    _e4__BUILTIN_PROC(DLOPEN)   \
    _e4__BUILTIN_PROC(DLSYM)

/* vim: set ft=c: */
