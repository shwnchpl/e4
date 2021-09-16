/* Facility Extensions word list declarations. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

#define _e4__BUILTIN_FACILITY_EXT_DECL()    \
    _e4__BUILTIN_PROC(MS)   \
    _e4__BUILTIN_PROC_N(TIME_AND_DATE, "TIME&DATE")

/* vim: set ft=c: */
