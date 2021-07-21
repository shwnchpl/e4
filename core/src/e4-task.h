#ifndef E4_TASK_H_
#define E4_TASK_H_

/* e4 task structure and internal API definitions */

#include "e4.h"

/* FIXME: Move to an e4-io.h header file? */
struct e4__io_src
{
    e4__cell buffer;    /* input buffer */
    e4__cell sid;       /* input source id */
    e4__cell in;        /* >IN */
    e4__cell length;    /* input buffer populated length */
    e4__cell sz;        /* input buffer size */
};

struct e4__task
{
    /* FIXME: Reorder these, if necessary. */
    /* FIXME: Ensure somewhere that a parse output buffer with 130
       characters minimum is always maintained at the top of HERE.
       This buffer is used by WORD, and per this implementation may
       output strings as long as 255 characters. The current builtin
       WORD implementation does NOT check that there is room at the
       end of HERE when writing to this space.
       This is the same buffer used for the counted string scratch
       buffer (used by S" etc.), which must be at least 80
       characters. Since HERE is being used, the dedicated buffers
       in this struct can be removed. */

    /* User table. */
    e4__cell here;
    e4__cell pad; /* 84 characters minimum */
    e4__cell s0;
    e4__cell sp;
    e4__cell tib; /* FIXME: tib size is calculable. Do this. */
    e4__cell r0;
    e4__cell rp;
    e4__cell ip;
    e4__cell base; /* Initialize to 10. */
    e4__cell compiling;
    struct e4__io_src io_src;

    /* System variables. */
    unsigned long sz;
    struct e4__dict_header *dict;
    struct e4__io_func io_func;
};

#endif /* E4_TASK_H_ */
