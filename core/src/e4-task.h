#ifndef E4_TASK_H_
#define E4_TASK_H_

/* e4 task structure and internal API definitions */

#include "e4.h"

struct e4__task
{
    /* User table. */
    e4__cell here;
    e4__cell pad;
    e4__cell s0;
    e4__cell sp;
    e4__cell tib;
    e4__cell tib_oft;
    e4__cell tib_sz;
    e4__cell r0;
    e4__cell rp;
    e4__cell ip;

    /* System variables. */
    unsigned long sz;
    struct e4__dict_header *dict;
};

#endif /* E4_TASK_H_ */
