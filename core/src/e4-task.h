#ifndef e4_TASK_H_
#define e4_TASK_H_

/* e4 task structure and internal API definitions */
/* FIXME: Should this file just be e4-internal.h? */

#include "e4.h"

#include <setjmp.h>

/* FIXME: Move to an e4-io.h header file? */
struct e4__io_src
{
    e4__cell buffer;    /* input buffer */
    e4__usize sid;      /* input source id */
    e4__usize in;       /* >IN */
    e4__usize length;   /* input buffer populated length */
    e4__usize sz;       /* input buffer size */
};

/* FIXME: Move to some exception header file? */
struct e4__exception_ctx
{
    jmp_buf ctx;
};

struct e4__task
{
    /* FIXME: Ensure somewhere that a parse output buffer with 130
       characters minimum is always maintained at the top of HERE.
       130 = 64 * 2 + 2 and the pictured numeric output buffer is
       required to be at least n * 2 + 2 bytes long, where n is the
       number of bits in a cell, and the largest this value is ever
       likely to be is 64.
       This buffer is used by WORD, which requires a buffer of at
       least 33 characters that *may* be the same as the buffer used
       for pictured numeric output. Per this implementation, WORD may
       output strings as long as 255 characters. The current builtin
       WORD implementation does NOT check that there is room at the
       end of HERE when writing to this space.

       Since here is being used, no dedicated buffers are provided
       for this purposes outlined above. */
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
    /* FIXME: Can these somehow be made to be flags? */
    e4__usize base; /* Initialize to 10. */
    e4__usize compiling;
    struct e4__io_src io_src;

    /* System variables. */
    e4__usize sz;
    struct e4__dict_header *dict;
    struct e4__io_func io_func;
    e4__bool exception_valid;
    e4__usize exception_code;
    struct e4__exception_ctx exception;
};

#endif /* e4_TASK_H_ */
