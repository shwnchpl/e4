/* File word list declarations. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

#define _e4__BUILTIN_FILE_DECL()    \
    _e4__BUILTIN_THUNK(BIN) \
    _e4__BUILTIN_PROC_N(CLOSE_FILE, "CLOSE-FILE")   \
    _e4__BUILTIN_THUNK(INCLUDED)    \
    _e4__BUILTIN_PROC_N(INCLUDE_FILE, "INCLUDE-FILE")   \
    _e4__BUILTIN_PROC_N(OPEN_FILE, "OPEN-FILE") \
    _e4__BUILTIN_CONSTANT_N(R_SLASH_O, "R/O", e4__F_READ)   \
    _e4__BUILTIN_CONSTANT_N(R_SLASH_W, "R/W", e4__F_READWRITE)  \
    _e4__BUILTIN_CONSTANT_N(W_SLASH_O, "W/O", e4__F_WRITE)

/* vim: set ft=c: */
