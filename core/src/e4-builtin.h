#ifndef E4_BUILTIN_H_
#define E4_BUILTIN_H_

/* FIXME: Should this code even live in builtin.c? Could it just be
   moved to task.c? */

struct e4__builtin
{
    char *name;
    unsigned short nbytes;
    void (*code)(struct e4__task *, void *);
};

extern struct e4__builtin e4__BUILTIN_TABLE[];

#endif /* E4_BUILTIN_H_ */
