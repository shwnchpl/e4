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
    e4__cell sz;        /* input buffer size */
};

struct e4__task
{
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
    e4__cell base; /* Initialize to 10. */
    e4__cell compiling;
    struct e4__io_src io_src;

    /* Buffers */
    e4__cell pob;   /* parse output buffer: 130 characters min */
    e4__cell sq;    /* string scratch buffer 80 chars min */

    /* System variables. */
    unsigned long sz;
    struct e4__dict_header *dict;
    struct e4__io_func io_func;
};

#endif /* E4_TASK_H_ */
