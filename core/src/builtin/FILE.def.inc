/* File word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static const void * const e4__builtin_BIN[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)e4__F_BIN,
    &e4__BUILTIN_XT[e4__B_OR],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

/* vim: set ft=c: */
