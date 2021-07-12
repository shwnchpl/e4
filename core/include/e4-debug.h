#ifndef E4_DEBUG_H_
#define E4_DEBUG_H_

/* FIXME: Everything in this file is a hack. None of it is where it
   belongs. This only exists for very preliminary debug/proof of
   concept purposes and eventually will be removed completely. */

#include <stdio.h>

/* TODO: Consider renaming this to e4__cell. */
typedef void** e4__cell;

#define e4__DEREF(p)    (*((e4__cell*)(p)))
#define e4__DEREF2(p)   (**((e4__cell**)(p)))

/* TODO: Determine if this is sensible. */
#define e4__MIN_TASK_SZ (2048)

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

struct e4__dict_header
{
    struct e4__dict_header *link;
    struct e4__dict_footer *footer;

    /* FIXME: Consider moving flags to the footer. */
    unsigned short flags;
    unsigned short nbytes;
    char name[1];
};

struct e4__dict_footer
{
    void *user;
    void *code;
    e4__cell data[1];
};

struct e4__builtin
{
    char *name;
    unsigned short nbytes;
    void (*code)(struct e4__task *, void *);
};

extern struct e4__builtin e4__BUILTIN_TABLE[];

void e4__execute(struct e4__task *state, void *next);
void e4__execute_threaded(struct e4__task *state, void *next);
void e4__builtin_abort(struct e4__task *state, void *next);
void e4__builtin_return(struct e4__task *state, void *next);
void e4__builtin_skip(struct e4__task *state, void *next);

void* e4__dict_entry(void *here, struct e4__dict_header *prev, char *name,
        unsigned short nbytes, void *code, void *user, unsigned short flags);
struct e4__dict_header* e4__dict_lookup(struct e4__dict_header *dict,
        char *name, unsigned short nbytes);

struct e4__task* e4__task_create(void *buffer, unsigned long size);
void e4__task_load_builtins(struct e4__task *task);

#endif /* E4_DEBUG_H_ */
