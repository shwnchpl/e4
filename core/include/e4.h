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
    struct e4__execute_token *xt;

    /* FIXME: Consider moving flags to the xt. */
    unsigned char flags;
    unsigned char nbytes;
    char *name;
};

struct e4__execute_token
{
    void *code;
    void *user;
    e4__cell data[1];
};

/* FIXME: The current API is unstable. As it stands, if you're
   going to implement one of these, you need to implement them
   all. In the future, this may change. */
struct e4__io_func
{
    void *user;
    int (*key)(void *user, char *buf);
    int (*accept)(void *user, char *buf, unsigned long n);
    int (*type)(void *user, const char *buf, unsigned long n);
};

/* error constants */
#define e4__E_OK            (0)
#define e4__E_UNSUPPORTED   (-21)
#define e4__E_FAILURE       (-256)

/* flag constants */
#define e4__F_SKIP_LEADING  (0x01)
#define e4__F_NEG_PREFIX    (0x01)
#define e4__F_BASE_PREFIX   (0x02)
#define e4__F_CHAR_LITERAL  (0x04)
#define e4__F_BUILTIN       (0x08)

/* builtin constants */
#define e4__B_RET           (0)
#define e4__B_ABORT         (1)
#define e4__B_LIT           (2)
#define e4__B_SKIP          (3)
#define e4__B_WORD          (4)
#define e4__BUILTIN_COUNT   (5)

/* source ID constants */
#define e4__SID_STR         (-1)
#define e4__SID_UID         (0)

/* task constants */
/* TODO: Determine if this is sensible. */
#define e4__TASK_MIN_SZ     (2048)

/* e4 macros */
#define e4__DEREF(p)    (*((e4__cell*)(p)))
#define e4__DEREF2(p)   (**((e4__cell**)(p)))

/* builtin declarations */
/* FIXME: Should the header table even be exposed here? */
extern const struct e4__dict_header e4__BUILTIN_HEADER[e4__BUILTIN_COUNT];
extern const struct e4__execute_token e4__BUILTIN_XT[e4__BUILTIN_COUNT];
void e4__builtin_RET(struct e4__task *task, void *user);

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
void e4__execute(struct e4__task *task, void *user);
void e4__execute_threaded(struct e4__task *task, void *user);

/* io.c functions */
int e4__io_key(struct e4__task *task, void *buf);
int e4__io_accept(struct e4__task *task, char *buf, unsigned long n);
int e4__io_type(struct e4__task *task, const char *buf, unsigned long n);

/* mem.c functions */
int e4__mem_strncasecmp(const char *left, const char *right, unsigned long n);
unsigned long e4__mem_number(const char *buf, unsigned long size,
        unsigned char base, unsigned long flags, unsigned long long *out);
const char* e4__mem_parse(const char *buf, char delim, unsigned long size,
        unsigned long flags, unsigned long *length);

/* task.c functions */
struct e4__task* e4__task_create(void *buffer, unsigned long size);
void e4__task_io_init(struct e4__task *task, struct e4__io_func *io);

#endif /* E4_H_ */
