/* Facility word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static const void *e4__builtin_PAGE[] = {
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)"\033[2J\033[0;0H",
    &e4__BUILTIN_XT[e4__B_LIT_CELL],
    (void *)10,
    &e4__BUILTIN_XT[e4__B_TYPE],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};
