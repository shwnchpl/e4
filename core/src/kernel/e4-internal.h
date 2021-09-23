#ifndef e4_INTERNAL_H_
#define e4_INTERNAL_H_

/* e4 internal declarations and definitions */

#include "e4.h"

#include <setjmp.h>

struct e4__compile_state
{
    e4__usize state;
    e4__usize sus_state;
    e4__cell s0;
    e4__cell here0;
    struct e4__execute_token *xt;
};

struct e4__exception_ctx
{
    jmp_buf buf;
};

struct e4__exception_state
{
    struct e4__exception_ctx ctx;
    e4__bool valid;
    e4__usize code;
    e4__code_ptr then;
    void *user;
};

struct e4__io_src
{
    e4__cell buffer;    /* input buffer */
    e4__usize sid;      /* input source id */
    e4__usize in;       /* >IN */
    e4__usize length;   /* input buffer populated length */
    e4__usize sz;       /* input buffer size */
};

struct e4__pno_state
{
    e4__cell end;
    e4__cell offset;
};

struct e4__task
{
    /* FIXME: Add a dedicated counted string scratch buffer (to be
       used by S", S\", and C"), which must be able to store strings
       at least 80 characters long *AND* must allow strings returned
       to be valid for at least one consecutive call. For instance,
       `S" bar" S" foo" type type` should output "foobar" not
       "foofoo." */

    /* FIXME: Reorder these, if necessary. */
    /* User table. */
    e4__cell here;
    e4__cell pad; /* 84 characters minimum */
    e4__cell s0;
    e4__cell sp;
    e4__cell tib; /* FIXME: tib size is calculable. Do this. */
    e4__cell r0;
    e4__cell rp;
    e4__cell ip;
    e4__cell base_ptr;
    struct e4__io_src io_src;
    struct e4__compile_state compile;
    struct e4__pno_state pno;

    /* System variables. */
    e4__cell tr0;
    e4__usize sz;
    e4__usize tib_sz;
    e4__usize base; /* Initialize to 10. */
    struct e4__dict_header *dict;
    struct e4__io_func io_func;
    struct e4__exception_state exception;
    const char *abortq_msg;
    e4__usize abortq_msg_len;
};

#endif /* e4_INTERNAL_H_ */
