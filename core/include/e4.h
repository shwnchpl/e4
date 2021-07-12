#ifndef E4_H_
#define E4_H_

/**************************************************
 *               e4 public C API
 *************************************************/

/* e4 types */
typedef void** e4__cell;

struct e4__task;

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

/* e4 constants */
/* TODO: Determine if this is sensible. */
#define e4__TASK_MIN_SZ (2048)

/* e4 macros */
#define e4__DEREF(p)    (*((e4__cell*)(p)))
#define e4__DEREF2(p)   (**((e4__cell**)(p)))

/* builtin.c functions */
void e4__builtin_abort(struct e4__task *task, void *next);
void e4__builtin_return(struct e4__task *task, void *next);
void e4__builtin_skip(struct e4__task *task, void *next);

/* dict.c functions */
/* FIXME: Should either of these be public at all? */
void* e4__dict_entry(void *here, struct e4__dict_header *prev, char *name,
        unsigned short nbytes, void *code, void *user, unsigned short flags);
struct e4__dict_header* e4__dict_lookup(struct e4__dict_header *dict,
        char *name, unsigned short nbytes);

/* stack.c functions */
void e4__stack_push(struct e4__task *task, void *v);
e4__cell e4__stack_pop(struct e4__task *task);
e4__cell e4__stack_peek(struct e4__task *task);
unsigned long e4__stack_depth(struct e4__task *task);
void e4__stack_rpush(struct e4__task *task, void *v);
e4__cell e4__stack_rpop(struct e4__task *task);
e4__cell e4__stack_rpeek(struct e4__task *task);

/* execute.c functions */
void e4__execute(struct e4__task *task, void *next);
void e4__execute_threaded(struct e4__task *task, void *next);

/* task.c functions */
struct e4__task* e4__task_create(void *buffer, unsigned long size);
void e4__task_load_builtins(struct e4__task *task);

#endif /* E4_H_ */
