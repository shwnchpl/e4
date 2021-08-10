/* Core word list declarations. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

#define _e4__BUILTIN_CORE_DECL()    \
    _e4__BUILTIN_PROC_FIRST(ABORT)  \
    _e4__BUILTIN_PROC(ALLOT)    \
    _e4__BUILTIN_USERVAR(BASE) \
    _e4__BUILTIN_CONSTANT(BL, ' ')  \
    _e4__BUILTIN_PROC_NF(BRACKET_TICK, "[']",   \
           e4__F_IMMEDIATE | e4__F_COMPONLY)   \
    _e4__BUILTIN_PROC(CELLS)    \
    _e4__BUILTIN_PROC_N(COLON, ":") \
    _e4__BUILTIN_PROC_N(COMMA, ",") \
    _e4__BUILTIN_PROC(CONSTANT) \
    _e4__BUILTIN_PROC(CR)   \
    _e4__BUILTIN_PROC(CREATE)   \
    _e4__BUILTIN_PROC(DEPTH)    \
    _e4__BUILTIN_PROC_NF(DOES, "DOES>", e4__F_IMMEDIATE)    \
    _e4__BUILTIN_PROC_N(DOT, ".")    \
    _e4__BUILTIN_PROC(DROP) \
    _e4__BUILTIN_PROC(DUP)  \
    _e4__BUILTIN_PROC(EMIT) \
    _e4__BUILTIN_PROC(EXECUTE)  \
    _e4__BUILTIN_PROC_F(EXIT, e4__F_COMPONLY)   \
    _e4__BUILTIN_PROC_N(FETCH, "@") \
    _e4__BUILTIN_USERVAR(HERE)  \
    _e4__BUILTIN_PROC_F(LITERAL, e4__F_COMPONLY)    \
    _e4__BUILTIN_PROC_N(MINUS, "-") \
    _e4__BUILTIN_PROC(OVER) \
    _e4__BUILTIN_PROC_N(PLUS, "+")  \
    _e4__BUILTIN_PROC(QUIT) \
    _e4__BUILTIN_PROC(ROT)  \
    _e4__BUILTIN_PROC_NF(SEMICOLON, ";", e4__F_IMMEDIATE | e4__F_COMPONLY)  \
    _e4__BUILTIN_PROC_N(STORE, "!") \
    _e4__BUILTIN_PROC(SWAP) \
    _e4__BUILTIN_PROC_N(TICK, "'")  \
    _e4__BUILTIN_PROC_N(TO_BODY, ">BODY")   \
    _e4__BUILTIN_PROC_N(TO_NUMBER, ">NUMBER")   \
    _e4__BUILTIN_PROC_N(U_DOT, "U.")  \
    _e4__BUILTIN_PROC(VARIABLE) \
    _e4__BUILTIN_PROC(WORD)

/* vim: set ft=c: */
