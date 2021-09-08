#ifndef e4_H_
#define e4_H_

/**************************************************
 *                  e4 version
 *************************************************/

#define e4__VERSION_NUMBER      2001
#define e4__VERSION_STRING      "0.2.1"

/**************************************************
 *               e4 configuration
 *************************************************/

#if defined(e4__LOAD_CONFIG)
    /* To inject configuration into e4, provide the e4__USIZE_DEFINED
       preprocessor token using your build system and create a file
       with the name "e4-config.h" containing the relevant
       configuration. */
    #include "e4-config.h"
#endif

#if !defined(e4__BUILD_EVERYTHING) && !defined(e4__BUILD_DEFAULT) && \
        !defined(e4__BUILD_MINIMAL)
    #define e4__BUILD_DEFAULT
#endif

#if defined(e4__BUILD_EVERYTHING)
    #define e4__INCLUDE_CORE_EXT
    #define e4__INCLUDE_EXCEPTION
    #define e4__INCLUDE_TOOLS
    #define e4__INCLUDE_TOOLS_EXT
#elif defined(e4__BUILD_DEFAULT)
    /* XXX: For now, this is the same as everything, but that likely
       will not always be the case. */
    #define e4__INCLUDE_CORE_EXT
    #define e4__INCLUDE_EXCEPTION
    #define e4__INCLUDE_TOOLS
    #define e4__INCLUDE_TOOLS_EXT
#endif

#if defined(e4__EXCLUDE_CORE_EXT) && defined(e4__INCLUDE_CORE_EXT)
    #undef e4__INCLUDE_CORE_EXT
#endif

#if defined(e4__EXCLUDE_EXCEPTION) && defined(e4__INCLUDE_EXCEPTION)
    #undef e4__INCLUDE_EXCEPTION
#endif

#if defined(e4__EXCLUDE_TOOLS) && defined(e4__INCLUDE_TOOLS)
    #undef e4__INCLUDE_TOOLS
#endif

#if defined(e4__EXCLUDE_TOOLS_EXT) && defined(e4__INCLUDE_TOOLS_EXT)
    #undef e4__INCLUDE_TOOLS_EXT
#endif

/**************************************************
 *               e4 public C API
 *************************************************/

#include <stdlib.h>

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

#define e4__USIZE_BIT   (sizeof(e4__usize) * CHAR_BIT)

typedef unsigned char e4__bool;
typedef unsigned char e4__u8;

struct e4__task;

typedef void (*e4__code_ptr)(struct e4__task *, e4__cell user);

struct e4__double {
    e4__usize low;
    e4__usize high;
};

struct e4__dict_header {
    struct e4__dict_header *link;
    struct e4__execute_token *xt;

    /* FIXME: Consider moving flags to the xt. */
    e4__u8 flags;
    e4__u8 nbytes;
    const char *name;
};

struct e4__execute_tuple {
    e4__code_ptr code;
    void *user;
};

struct e4__execute_token {
    e4__code_ptr code;
    void *user;
    e4__cell data[1];
};

/* FIXME: The current API is unstable. As it stands, if you're
   going to implement one of these, you need to implement them
   all. In the future, this may change. */
struct e4__io_func {
    void *user;
    e4__usize (*accept)(void *user, char *buf, e4__usize *n);
    e4__usize (*key)(void *user, char *buf);
    e4__usize (*type)(void *user, const char *buf, e4__usize n);
};

/* error constants - standard */
#define e4__E_OK            (0)
#define e4__E_ABORT         (-1)
#define e4__E_STKUNDERFLOW  (-4)
#define e4__E_RSTKUNDERFLOW (-6)
#define e4__E_INVMEMADDR    (-9)
#define e4__E_DIVBYZERO     (-10)
#define e4__E_RSLTOUTORANGE (-11)
#define e4__E_UNDEFWORD     (-13)
#define e4__E_COMPONLYWORD  (-14)
#define e4__E_INVFORGET     (-15)
#define e4__E_ZLNAME        (-16)
#define e4__E_UNSUPPORTED   (-21)
#define e4__E_CSMISMATCH    (-22)
#define e4__E_RSTKIMBALANCE (-25)
#define e4__E_USERINTERRUPT (-28)
#define e4__E_NESTEDCOMPILE (-29)
#define e4__E_INVNAMEARG    (-32)
#define e4__E_QUIT          (-56)

/* error constants - system */
#define e4__E_FAILURE       (-256)
#define e4__E_BYE           (-257)
#define e4__E_BUG           (-258)
#define e4__E_INVBUILTINMUT (-259)

/* flag constants - dictionary entry */
#define e4__F_IMMEDIATE     (0x01)
#define e4__F_COMPONLY      (0x02)
#define e4__F_CONSTANT      (0x04)
#define e4__F_BUILTIN       (0x08)
#define e4__F_COMPILING     (0x10)

/* flag constants - numeric parsing */
#define e4__F_NEG_PREFIX    (0x01)
#define e4__F_BASE_PREFIX   (0x02)
#define e4__F_CHAR_LITERAL  (0x04)

/* flag constants - numeric formatting and arithmetic */
#define e4__F_SIGNED        (0x01)
#define e4__F_FLOORDIV      (0x02)

/* flag constants - word parsing */
#define e4__F_SKIP_LEADING  (0x01)
#define e4__F_IGNORE_ESC    (0x02)

/* boolean flags */
#define e4__BF_TRUE         ((e4__usize)-1)
#define e4__BF_FALSE        ((e4__usize)0)

/* uservar offsets */
/* FIXME: Add more user variable offsets. */
#define e4__UV_HERE         (0)
#define e4__UV_PAD          (1)
#define e4__UV_BASE         (8)
#define e4__UV_SOURCE_ID    (10)

/* builtin constants */
enum e4__builtin_id {
    /* CORE words */
    e4__B_ABORT = 0,
    e4__B_ABS,
    e4__B_ALIGN,
    e4__B_ALIGNED,
    e4__B_ALLOT,
    e4__B_AND,
    e4__B_BASE,
    e4__B_BEGIN,
    e4__B_BL,
    e4__B_BRACKET_CHAR,
    e4__B_BRACKET_TICK,
    e4__B_C_COMMA,
    e4__B_C_FETCH,
    e4__B_C_QUOTE,
    e4__B_C_STORE,
    e4__B_CELLS,
    e4__B_CELL_PLUS,
    e4__B_CHAR,
    e4__B_CHAR_PLUS,
    e4__B_CHARS,
    e4__B_COLON,
    e4__B_COMMA,
    e4__B_CONSTANT,
    e4__B_COUNT,
    e4__B_CR,
    e4__B_CREATE,
    e4__B_DECIMAL,
    e4__B_DEPTH,
    e4__B_DO,
    e4__B_DOES,
    e4__B_DOT,
    e4__B_DOT_QUOTE,
    e4__B_DROP,
    e4__B_DUP,
    e4__B_ELSE,
    e4__B_EMIT,
    e4__B_EQUALS,
    e4__B_EVALUATE,
    e4__B_EXECUTE,
    e4__B_EXIT,
    e4__B_FETCH,
    e4__B_FILL,
    e4__B_GREATER_THAN,
    e4__B_HERE,
    e4__B_I,
    e4__B_IF,
    e4__B_IMMEDIATE,
    e4__B_INVERT,
    e4__B_J,
    e4__B_KEY,
    e4__B_LEAVE,
    e4__B_LEFT_BRACKET,
    e4__B_LESS_THAN,
    e4__B_LITERAL,
    e4__B_LOOP,
    e4__B_LSHIFT,
    e4__B_MAX,
    e4__B_MIN,
    e4__B_MINUS,
    e4__B_MOD,
    e4__B_MOVE,
    e4__B_NEGATE,
    e4__B_ONE_MINUS,
    e4__B_ONE_PLUS,
    e4__B_OR,
    e4__B_OVER,
    e4__B_PAREN,
    e4__B_PLUS,
    e4__B_PLUS_LOOP,
    e4__B_PLUS_STORE,
    e4__B_POSTPONE,
    e4__B_QUESTION_DUP,
    e4__B_QUIT,
    e4__B_R_FETCH,
    e4__B_R_FROM,
    e4__B_RECURSE,
    e4__B_REPEAT,
    e4__B_RIGHT_BRACKET,
    e4__B_ROT,
    e4__B_RSHIFT,
    e4__B_S_TO_D,
    e4__B_S_QUOTE,
    e4__B_SEMICOLON,
    e4__B_SLASH,
    e4__B_SLASH_MOD,
    e4__B_SPACE,
    e4__B_SPACES,
    e4__B_STAR,
    e4__B_STORE,
    e4__B_SWAP,
    e4__B_THEN,
    e4__B_TICK,
    e4__B_TO_BODY,
    e4__B_TO_NUMBER,
    e4__B_TO_R,
    e4__B_TWO_DROP,
    e4__B_TWO_DUP,
    e4__B_TWO_FETCH,
    e4__B_TWO_OVER,
    e4__B_TWO_SLASH,
    e4__B_TWO_STAR,
    e4__B_TWO_STORE,
    e4__B_TWO_SWAP,
    e4__B_TYPE,
    e4__B_U_DOT,
    e4__B_U_LESS_THAN,
    e4__B_UNLOOP,
    e4__B_UNTIL,
    e4__B_VARIABLE,
    e4__B_WHILE,
    e4__B_WORD,
    e4__B_XOR,
    e4__B_ZERO_EQUALS,
    e4__B_ZERO_LESS,

    /* SYSTEM words */
    e4__B_BRANCH,
    e4__B_BRANCH0,
    e4__B_CLEAR,
    e4__B_LIT_CELL,
    e4__B_LIT_CSTR,
    e4__B_LIT_DOES,
    e4__B_LIT_STR,
    e4__B_RESTRICT,
    e4__B_SENTINEL,

    #if defined(e4__INCLUDE_CORE_EXT)

        /* CORE EXT words */
        e4__B_AGAIN,
        e4__B_BACKSLASH,
        e4__B_BUFFER_COLON,
        e4__B_COLON_NONAME,
        e4__B_DEFER,
        e4__B_DEFER_FETCH,
        e4__B_DEFER_STORE,
        e4__B_DOT_PAREN,
        e4__B_ERASE,
        e4__B_FALSE,
        e4__B_HEX,
        e4__B_MARKER,
        e4__B_NIP,
        e4__B_NOT_EQUALS,
        e4__B_PAD,
        e4__B_PICK,
        e4__B_QUESTION_DO,
        e4__B_REFILL,
        e4__B_ROLL,
        e4__B_S_BACKSLASH_QUOTE,
        e4__B_SOURCE_ID,
        e4__B_TO,
        e4__B_TRUE,
        e4__B_TUCK,
        e4__B_TWO_R_FETCH,
        e4__B_TWO_R_FROM,
        e4__B_TWO_TO_R,
        e4__B_U_GREATER_THAN,
        e4__B_UNUSED,
        e4__B_VALUE,
        e4__B_ZERO_GREATER,
        e4__B_ZERO_NOT_EQUALS,

    #endif /* defined(e4__INCLUDE_CORE_EXT) */

    #if defined(e4__INCLUDE_EXCEPTION)

        /* EXCEPTION words */
        e4__B_CATCH,
        e4__B_THROW,

    #endif /* defined(e4__INCLUDE_EXCEPTION) */

    #if defined(e4__INCLUDE_TOOLS)

        /* TOOLS words */
        e4__B_DOT_S,
        e4__B_DUMP,
        e4__B_QUESTION,
        e4__B_WORDS,

    #endif /* defined(e4__INCLUDE_TOOLS) */

    #if defined(e4__INCLUDE_TOOLS_EXT)

        /* TOOLS EXT words */
        e4__B_BYE,
        e4__B_FORGET,

    #endif

    e4__BUILTIN_COUNT
};

/* source ID constants */
#define e4__SID_STR         (-1)
#define e4__SID_UID         (0)

/* compilation mode/state constants */
#define e4__COMP_COLON      ( 1 )
#define e4__COMP_DOES       ( 2 )
#define e4__COMP_NONAME     ( 3 )

/* task constants */
/* TODO: Determine if this is sensible. */
#define e4__TASK_MIN_SZ     (1024)

/* e4 macros */
#define e4__ASSERT_MSG0(c, m)   enum { e4__assert__ ## m = 1 / !!(c) }
#define e4__ASSERT_MSG1(c, m)   e4__ASSERT_MSG0(c, m)
#define e4__ASSERT(c)           e4__ASSERT_MSG1(c, __LINE__)

#define e4__DEREF(p)    (*((e4__cell *)(p)))
#define e4__DEREF2(p)   (**((e4__cell **)(p)))

#define e4__USIZE_IS_NEGATIVE(u)    (u > ((e4__usize)-1 >> 1))
#define e4__USIZE_NEGATE(u)         (((u) ^ (e4__usize)-1) + 1)

#define e4__USIZE_HH(u)     (((u) & e4__USIZE_HHMASK) >> (e4__USIZE_BIT >> 1))
#define e4__USIZE_HHMASK    (((e4__usize)-1) << (e4__USIZE_BIT >> 1))
#define e4__USIZE_LH(u)     ((u) & e4__USIZE_LHMASK)
#define e4__USIZE_LHMASK    (((e4__usize)-1) >> (e4__USIZE_BIT >> 1))

#define e4__USIZE_U(m, l)   \
    (((m) << (e4__USIZE_BIT >> 1) & e4__USIZE_HHMASK) | (l & e4__USIZE_LHMASK))
#define e4__USIZE_SPLIT(u, h, l)    \
    h = e4__USIZE_HH(u), l = e4__USIZE_LH(u)

/* builtin declarations */
/* FIXME: Should the header table even be exposed here? */
extern const struct e4__dict_header e4__BUILTIN_HEADER[e4__BUILTIN_COUNT];
extern const struct e4__execute_tuple e4__BUILTIN_XT[e4__BUILTIN_COUNT];
e4__usize e4__builtin_exec(struct e4__task *task, enum e4__builtin_id id);

/* compile.c functions */
void e4__compile_cancel(struct e4__task *task);
void e4__compile_cell(struct e4__task *task, e4__cell cell);
void e4__compile_char(struct e4__task *task, e4__cell cell);
void e4__compile_cstr(struct e4__task *task, const char *str, e4__u8 len);
void e4__compile_estr(struct e4__task *task, const char *str, e4__usize len);
e4__usize e4__compile_finish(struct e4__task *task);
void e4__compile_literal(struct e4__task *task, e4__cell num);
e4__usize e4__compile_resume(struct e4__task *task);
e4__usize e4__compile_start(struct e4__task *task,
        struct e4__execute_token *xt, e4__usize mode);
void e4__compile_str(struct e4__task *task, const char *str, e4__usize len);
void e4__compile_suspend(struct e4__task *task);

/* dict.c functions */
void e4__dict_entry(struct e4__task *task, const char *name, e4__u8 nbytes,
        e4__u8 flags, e4__code_ptr code, void *user);
e4__usize e4__dict_forget(struct e4__task *task, const char *name,
        e4__u8 nbytes);
const struct e4__dict_header* e4__dict_lookup(struct e4__task *task,
        const char *name, e4__u8 nbytes);
const struct e4__dict_header* e4__dict_suggest(struct e4__task *task,
        const struct e4__dict_header *from, const char *prefix,
        e4__u8 pbytes);

/* double.c functions */
e4__usize e4__double_div(struct e4__double n, e4__usize d, e4__usize flags,
        struct e4__double *q, e4__usize *r);
e4__usize e4__double_ndiv(struct e4__double n, e4__usize d,
        e4__usize flags, e4__usize *q, e4__usize *r);
struct e4__double e4__double_negate(struct e4__double d);
struct e4__double e4__double_u(e4__usize low, e4__usize high);

/* evaluate.c functions */
e4__usize e4__evaluate(struct e4__task *task, const char *buf, e4__usize sz);
void e4__evaluate_quit(struct e4__task *task);

/* exception.c functions */
e4__usize e4__exception_catch(struct e4__task *task, void *xt);
void e4__exception_throw(struct e4__task *task, e4__usize e);
void e4__exception_throw_then(struct e4__task *task, e4__usize e,
        e4__code_ptr then, void *user);

/* execute.c functions */
void e4__execute(struct e4__task *task, void *xt);
void e4__execute_deferthunk(struct e4__task *task, e4__cell user);
void e4__execute_doesthunk(struct e4__task *task, e4__cell user);
void e4__execute_marker(struct e4__task *task, e4__cell user);
void e4__execute_ret(struct e4__task *task);
void e4__execute_threaded(struct e4__task *task, e4__cell user);
void e4__execute_threadedthunk(struct e4__task *task, e4__cell user);
void e4__execute_userval(struct e4__task *task, e4__cell user);
void e4__execute_uservar(struct e4__task *task, e4__cell user);
void e4__execute_value(struct e4__task *task, e4__cell user);
void e4__execute_variable(struct e4__task *task, e4__cell user);

/* io.c functions */
e4__usize e4__io_accept(struct e4__task *task, char *buf, e4__usize *n);
e4__usize e4__io_dump(struct e4__task *task, e4__cell p, e4__usize len);
e4__usize e4__io_key(struct e4__task *task, char *buf);
e4__usize e4__io_parse(struct e4__task *task, char delim, e4__usize flags,
        const char **out);
e4__usize e4__io_refill(struct e4__task *task, e4__usize *bf);
e4__usize e4__io_type(struct e4__task *task, const char *buf, e4__usize n);
char* e4__io_word(struct e4__task *task, char delim);

/* mem.c functions */
e4__usize e4__mem_aligned(e4__usize n);
e4__usize e4__mem_cells(e4__usize n);
e4__usize e4__mem_dict_entry(void *here, struct e4__dict_header *prev,
        const char *name, e4__u8 nbytes, e4__u8 flags, e4__code_ptr code,
        void *user);
const struct e4__dict_header* e4__mem_dict_lookup(
        const struct e4__dict_header *dict, const char *name, e4__u8 nbytes);
const struct e4__dict_header* e4__mem_dict_suggest(
        const struct e4__dict_header *dict, const char *prefix, e4__u8 pbytes);
e4__usize e4__mem_dump(const char **p, e4__usize *len, char *buffer);
const char* e4__mem_strnescape(const char **str, e4__usize *len,
        e4__usize *chunk_len, char *scratch);
int e4__mem_strncasecmp(const char *left, const char *right, e4__usize n);
e4__usize e4__mem_number(const char *buf, e4__usize sz, e4__u8 base,
        e4__u8 flags, e4__usize *out);
e4__usize e4__mem_parse(const char *buf, char delim, e4__usize sz,
        e4__usize flags, const char **length);

/* stack.c functions */
void e4__stack_clear(struct e4__task *task);
e4__usize e4__stack_depth(struct e4__task *task);
void e4__stack_drop(struct e4__task *task);
void e4__stack_dup(struct e4__task *task);
void e4__stack_over(struct e4__task *task);
e4__cell e4__stack_peek(struct e4__task *task);
e4__cell e4__stack_pick(struct e4__task *task, e4__usize u);
e4__cell e4__stack_pop(struct e4__task *task);
void e4__stack_push(struct e4__task *task, void *v);
void e4__stack_rot(struct e4__task *task);
e4__usize e4__stack_rdepth(struct e4__task *task);
e4__cell e4__stack_rpeek(struct e4__task *task);
e4__cell e4__stack_rpop(struct e4__task *task);
void e4__stack_rpush(struct e4__task *task, void *v);
void e4__stack_roll(struct e4__task *task);
void e4__stack_swap(struct e4__task *task);
void e4__stack_tuck(struct e4__task *task);

/* task.c functions */
e4__cell e4__task_allot(struct e4__task *task, e4__usize sz);
e4__bool e4__task_compiling(struct e4__task *task);
struct e4__task* e4__task_create(void *buffer, e4__usize sz);
void e4__task_io_init(struct e4__task *task, struct e4__io_func *io);
void e4__task_io_get(struct e4__task *task, struct e4__io_func *io);
e4__usize e4__task_unused(struct e4__task *task);
e4__cell e4__task_uservar(struct e4__task *task, e4__usize offset);

/* usize.c functions */
e4__usize e4__usize_clz(e4__usize u);
e4__usize e4__usize_digit(e4__usize u, e4__u8 base, char *d);
char* e4__usize_format(e4__usize n, e4__u8 base, e4__u8 flags, char *buf,
        e4__usize sz);
const char* e4__usize_format_exception(e4__usize e, e4__usize *len);
struct e4__double e4__usize_mul(e4__usize l, e4__usize r, e4__u8 flags);
e4__usize e4__usize_sdiv(e4__usize n, e4__usize d, e4__usize *r);
struct e4__double e4__usize_todouble(e4__usize u);

#endif /* e4_H_ */
