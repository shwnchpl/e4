#ifndef E4_H_
#define E4_H_

#if defined(e4__LOAD_CONFIG)
    /* To inject configuration into e4, provide the e4__USIZE_DEFINED
       preprocessor token using your build system and create a file
       with the name "e4-config.h" containing the relevant
       configuration. */
    #include "e4-config.h"
#endif

/**************************************************
 *               e4 public C API
 *************************************************/

/* e4 types */
typedef void** e4__cell;

/* Define the e4__usize type, an integral type that is the same width as
   the e4__cell pointer type, if it has not been provided. */
#if !defined(e4__USIZE_DEFINED)
    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
        /* If C99 features are available, try to just use uintptr_t. */
        #include <stdint.h>
        typedef uintptr_t e4__usize;
        #define e4__USIZE_DEFINED
    #else
        /* Attempt to deduce the appropriate size for e4__usize based on
           whatever information is available. */
        #include <limits.h>

        #if !defined(e4__WORDSIZE)
            #if defined(__WORDSIZE)
                #define e4__WORDSIZE __WORDSIZE
            #elif defined(_WIN64)
                #define e4__WORDSIZE 64
            #elif defined(_WIN32)
                #define e4__WORDSIZE 32
            #endif
        #endif

        #if defined(e4__WORDSIZE)
            #if UCHAR_MAX >> (e4__WORDSIZE - 1) == 1
                typedef unsigned char e4__usize;
                #define e4__USIZE_DEFINED
            #elif USHRT_MAX >> (e4__WORDSIZE - 1) == 1
                typedef unsigned short e4__usize;
                #define e4__USIZE_DEFINED
            #elif UINT_MAX >> (e4__WORDSIZE - 1) == 1
                typedef unsigned int e4__usize;
                #define e4__USIZE_DEFINED
            #elif ULONG_MAX >> (e4__WORDSIZE - 1) == 1
                typedef unsigned long e4__usize;
                #define e4__USIZE_DEFINED
            #elif defined(ULLONG_MAX) && ULLONG_MAX >> (e4__WORDSIZE - 1) == 1
                typedef unsigned long long e4__usize;
                #define e4__USIZE_DEFINED
            #endif
        #endif

        #if !defined(e4__USIZE_DEFINED)
            /* As a last ditch effort, attempt to just use size_t.
               There's a relatively decent chance this will actually
               work fine, but if it won't then the compile time
               assertion in assert.c will prevent compilation. If that
               does happen, there are still options (which are outlined
               in assert.c). */
            typedef size_t e4__usize;
            #define e4__USIZE_DEFINED
        #endif
    #endif
#endif

typedef unsigned char e4__bool;
typedef unsigned char e4__u8;

struct e4__task;

typedef void (*e4__code_ptr)(struct e4__task *, void *);

struct e4__dict_header
{
    struct e4__dict_header *link;
    struct e4__execute_token *xt;

    /* FIXME: Consider moving flags to the xt. */
    e4__u8 flags;
    e4__u8 nbytes;
    const char *name;
};

struct e4__execute_token
{
    e4__code_ptr code;
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
    int (*accept)(void *user, char *buf, e4__usize n);
    int (*type)(void *user, const char *buf, e4__usize n);
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

#define e4__F_SIGNED        (0x01)

/* builtin constants */
#define e4__B_RET           (0)
#define e4__B_ABORT         (1)
#define e4__B_GTNUMBER      (2)
#define e4__B_LIT           (3)
#define e4__B_SKIP          (4)
#define e4__B_WORD          (5)
#define e4__BUILTIN_COUNT   (6)

/* source ID constants */
#define e4__SID_STR         (-1)
#define e4__SID_UID         (0)

/* task constants */
/* TODO: Determine if this is sensible. */
#define e4__TASK_MIN_SZ     (2048)

/* e4 macros */
#define e4__ASSERT_MSG0(c, m)   enum { e4__assert__ ## m = 1 / !!(c) }
#define e4__ASSERT_MSG1(c, m)   e4__ASSERT_MSG0(c, m)
#define e4__ASSERT(c)           e4__ASSERT_MSG1(c, __LINE__)

#define e4__DEREF(p)    (*((e4__cell *)(p)))
#define e4__DEREF2(p)   (**((e4__cell **)(p)))

#define e4__USIZE_IS_NEGATIVE(u)    (u > ((e4__usize)-1 >> 1))
#define e4__USIZE_NEGATE(u)         (((u) ^ (e4__usize)-1) + 1)

/* builtin declarations */
/* FIXME: Should the header table even be exposed here? */
extern const struct e4__dict_header e4__BUILTIN_HEADER[e4__BUILTIN_COUNT];
extern const struct e4__execute_token e4__BUILTIN_XT[e4__BUILTIN_COUNT];
void e4__builtin_RET(struct e4__task *task, void *user);

/* dict.c functions */
/* FIXME: Should either of these be public at all? */
void* e4__dict_entry(void *here, struct e4__dict_header *prev,
        const char *name, e4__u8 nbytes, void *code, void *user, e4__u8 flags);
struct e4__dict_header* e4__dict_lookup(struct e4__dict_header *dict,
        const char *name, e4__u8 nbytes);

/* stack.c functions */
void e4__stack_clear(struct e4__task *task);
e4__usize e4__stack_depth(struct e4__task *task);
void e4__stack_drop(struct e4__task *task);
void e4__stack_dup(struct e4__task *task);
void e4__stack_over(struct e4__task *task);
e4__cell e4__stack_peek(struct e4__task *task);
e4__cell e4__stack_pop(struct e4__task *task);
void e4__stack_push(struct e4__task *task, void *v);
void e4__stack_rot(struct e4__task *task);
e4__cell e4__stack_rpeek(struct e4__task *task);
e4__cell e4__stack_rpop(struct e4__task *task);
void e4__stack_roll(struct e4__task *task);
void e4__stack_rpush(struct e4__task *task, void *v);
void e4__stack_swap(struct e4__task *task);
void e4__stack_tuck(struct e4__task *task);

/* execute.c functions */
void e4__execute(struct e4__task *task, void *user);
void e4__execute_threaded(struct e4__task *task, void *user);

/* io.c functions */
int e4__io_key(struct e4__task *task, void *buf);
int e4__io_accept(struct e4__task *task, char *buf, e4__usize n);
int e4__io_type(struct e4__task *task, const char *buf, e4__usize n);

/* mem.c functions */
int e4__mem_strncasecmp(const char *left, const char *right, e4__usize n);
e4__usize e4__mem_number(const char *buf, e4__usize size, e4__u8 base,
        e4__u8 flags, e4__usize *out);
const char* e4__mem_parse(const char *buf, char delim, e4__usize size,
        e4__usize flags, e4__usize *length);

/* num.c functions */
e4__usize e4__num_digit(e4__usize u, e4__u8 base, char *d);
char* e4__num_format(e4__usize n, e4__u8 base, e4__u8 flags, char *buf,
        e4__usize sz);
e4__usize e4__num_sdiv(e4__usize n, e4__usize d);

/* task.c functions */
struct e4__task* e4__task_create(void *buffer, e4__usize size);
void e4__task_io_init(struct e4__task *task, struct e4__io_func *io);

#endif /* E4_H_ */
