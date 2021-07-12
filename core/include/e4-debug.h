#ifndef E4_DEBUG_H_
#define E4_DEBUG_H_

/* FIXME: Everything in this file is a hack. None of it is where it
   belongs. This only exists for very preliminary debug/proof of
   concept purposes and eventually will be removed completely. */

#include <stdio.h>

/* TODO: Consider renaming this to e4__cell. */
typedef void** e4__void;

#define e4__DEREF(p)    (*((e4__void*)(p)))
#define e4__DEREF2(p)   (**((e4__void**)(p)))

/* TODO: Determine if this is sensible. */
#define e4__MIN_TASK_SZ (2048)

struct e4__task
{
    /* User table. */
    e4__void here;
    e4__void pad;
    e4__void s0;
    e4__void sp;
    e4__void tib;
    e4__void tib_oft;
    e4__void tib_sz;
    e4__void r0;
    e4__void rp;
    e4__void ip;

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
    e4__void data[1];
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
