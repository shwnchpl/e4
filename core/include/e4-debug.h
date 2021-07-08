#ifndef E4_DEBUG_H_
#define E4_DEBUG_H_

/* FIXME: Everything in this file is a hack. None of it is where it
   belongs. This only exists for very preliminary debug/proof of
   concept purposes and eventually will be removed completely. */

#include <stdio.h>

#define RETURN_ ((void*)e4__return)

struct e4__vm_state
{
    void (***ip)(struct e4__vm_state *, void *);
    void (****r)(struct e4__vm_state *, void *);

    /* FIXME: Hack. Remove me. */
    void (****r_base)(struct e4__vm_state *, void *);
};

void e4__execute(struct e4__vm_state *state, void *next);
void e4__execute_threaded(struct e4__vm_state *state, void *next);
void e4__abort(struct e4__vm_state *state, void *next);
void e4__skip(struct e4__vm_state *state, void *next);
void e4__return(struct e4__vm_state *state, void *next);

#endif /* E4_DEBUG_H_ */
