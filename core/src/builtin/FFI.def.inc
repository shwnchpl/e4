/* FFI word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

/* XXX: Several pieces of FFI related functionality are implemented
   directly in this file rather than as e4__mem_ utilities or elsewhere
   in the kernel. This is done because these utilities would have little
   to no use in cases where it is already possible/convenient to write
   C code. Additionally, it is always possible to invoke the builtins
   themselves from C code using e4__builtin_exec, if there is some case
   in which doing so is necessary. */

/* XXX: In several places in this file, we operate under the assumption
   that sizeof(T) >= alignof(T) and that, therefore, since we know
   sizeof(e4__cell) >= sizeof(T) where T is any valid FFI type, we know
   that it is safe to read/write any FFI type at a memory location with
   an alignment equal to alignof(e4__cell). Technically, the standard
   itself doesn't strictly require this. It is, however, required to be
   true for any type for which it is possible to have an array. We can
   take for granted that it is possible to have an array of any of the
   primitive types that we are using for FFI purposes, so it is likely
   a safe assumption that we can treat these types as having an
   alignment of no greater than alignof(e4__cell) for memory
   manipulation purposes. */

/* List of supported FFI types for binding and type fixing. The _tfcase
   macro is defined and undefined as necessary for the each given
   context. For unsigned types, the _tfcase_signed macro may simply be
   defined to _tfcase, but for signed types it is necessary to convert
   from the internal e4__usize 2's complement signed representation to
   whatever representation the system uses. From a practical standpoint,
   this representation will almost always also be 2's complement, but
   such a representation is not required by the C standard and as such
   the safest course of action is to explicitly convert into the proper
   format. */
#define _tfcases_nonvoid()  \
    _tfcase_signed(&ffi_type_double, double)    \
    else _tfcase_signed(&ffi_type_float, float) \
    else _tfcase(&ffi_type_pointer, void *) \
    else _tfcase_signed(&ffi_type_schar, signed char)   \
    else _tfcase_signed(&ffi_type_sint, signed int) \
    else _tfcase_signed(&ffi_type_sint8, e4__s8)    \
    else _tfcase_signed(&ffi_type_sint16, e4__s16)  \
    else _tfcase_signed(&ffi_type_sint32, e4__s32)  \
    else _tfcase_signed(&ffi_type_sint64, e4__s64)  \
    else _tfcase_signed(&ffi_type_slong, signed long)   \
    else _tfcase_signed(&ffi_type_sshort, signed short) \
    else _tfcase(&ffi_type_uchar, unsigned char)    \
    else _tfcase(&ffi_type_uint, unsigned int)  \
    else _tfcase(&ffi_type_uint8, e4__u8)   \
    else _tfcase(&ffi_type_uint16, e4__u16) \
    else _tfcase(&ffi_type_uint32, e4__u32) \
    else _tfcase(&ffi_type_uint64, e4__u64) \
    else _tfcase(&ffi_type_ulong, unsigned long)    \
    else _tfcase(&ffi_type_ushort, unsigned short)

static const void * const e4__builtin_FFI_ALIGN[] = {
    &e4__BUILTIN_XT[e4__B_HERE],
    &e4__BUILTIN_XT[e4__B_DUP],
    &e4__BUILTIN_XT[e4__B_ROT],
    &e4__BUILTIN_XT[e4__B_FFI_ALIGNED],
    &e4__BUILTIN_XT[e4__B_SWAP],
    &e4__BUILTIN_XT[e4__B_MINUS],
    &e4__BUILTIN_XT[e4__B_ALLOT],
    &e4__BUILTIN_XT[e4__B_EXIT],
    &e4__BUILTIN_XT[e4__B_SENTINEL]
};

static void e4__builtin_FFI_ALIGNED(struct e4__task *task, e4__cell user)
{
    register e4__cell at;
    register e4__usize val;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    at = e4__stack_pop(task);
    val = (e4__usize)e4__stack_pop(task);

    #define _tfcase(t, v)   \
        if (at == (e4__cell)t) {    \
            if (val % sizeof(v))    \
                val += (sizeof(v) - (val % sizeof(v))); \
        }
    #define _tfcase_signed _tfcase

    _tfcases_nonvoid()
    else {
        e4__exception_throw(task, e4__E_BADFFITYPE);
        e4__execute_ret(task);
        return;
    }

    #undef _tfcase_signed
    #undef _tfcase

    e4__stack_push(task, (e4__cell)val);

    e4__execute_ret(task);
}

static void e4__builtin_FFI_CALL(struct e4__task *task, e4__cell user)
{
    struct e4__ffi_data {
        ffi_cif cif;
        void *func;
        ffi_type *rtype;
        e4__usize argc;
        ffi_type *typev[1];
    };

    register e4__usize i;
    register e4__usize *argv, type_fixed_rv = 0;
    register struct e4__ffi_data *data;
    register void *aligned_here =
            (void *)e4__mem_aligned((e4__usize)task->here);

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    data = (struct e4__ffi_data *)e4__stack_pop(task);

    _e4__BUILTIN_EXPECT_DEPTH(task, data->argc);
    _e4__BUILTIN_EXPECT_AVAIL(task, data->argc);

    /* Validate return type. */
    #define _tfcase(t, v)   \
        if (data->rtype == t) (void)NULL;
    #define _tfcase_signed _tfcase

    _tfcases_nonvoid()
    else if (data->rtype != &ffi_type_void) {
        e4__exception_throw(task, e4__E_BADFFITYPE);
        e4__execute_ret(task);
        return;
    }

    #undef _tfcase_signed
    #undef _tfcase

    /* Type fix all arguments. This is a hack. */
    argv = (e4__usize *)&task->sp[1];
    for (i = 0; i < data->argc; ++i) {
        #define _tfcase(t, v)   \
            if (data->typev[data->argc - 1 - i] == t)   \
                *((v *)aligned_here) = (v)argv[i];
        #define _tfcase_signed(t, v)    \
            if (data->typev[data->argc - 1 - i] == t)   \
                *((v *)aligned_here) = e4__USIZE_IS_NEGATIVE(argv[i]) ? \
                    -(v)e4__USIZE_NEGATE(argv[i]) : (v)argv[i];

        _tfcases_nonvoid()
        else {
            e4__exception_throw(task, e4__E_BADFFITYPE);
            e4__execute_ret(task);
            return;
        }

        #undef _tfcase_signed
        #undef _tfcase

        /* Simply assigning aligned_here should be sufficient, but let's
           just be extra sure that strict aliasing optimizations don't
           try to thwart our plans. */
        memcpy(&argv[i], aligned_here, sizeof(e4__usize));
        e4__stack_push(task, (e4__cell)&argv[i]);
    }

    /* Make foreign call. */
    ffi_call(&data->cif, FFI_FN(data->func), aligned_here, &task->sp[1]);

    /* Type fix return value. */
    #define _tfcase(t, v)   \
        if (data->rtype == t)   \
            type_fixed_rv = (e4__usize)*((v *)aligned_here);
    #define _tfcase_signed(t, v)    \
        if (data->rtype == t) { \
            register v tmp = *((v *)aligned_here);  \
            type_fixed_rv = tmp < 0 ?   \
                e4__USIZE_NEGATE((e4__usize)-tmp) : (e4__usize)tmp; \
        }

    _tfcases_nonvoid()
    else if (data->rtype != &ffi_type_void) {
        /* We've already checked that return type is one of these
           things, so this should be unreachable. */
        e4__exception_throw(task, e4__E_BUG);
        e4__execute_ret(task);
        return;
    }

    #undef _tfcase_signed
    #undef _tfcase

    /* Remove junk from the stack. */
    task->sp += data->argc * 2;

    /* Push type fixed return value, if necessary. */
    if (data->rtype != &ffi_type_void)
        e4__stack_push(task, (e4__cell)type_fixed_rv);

    e4__execute_ret(task);
}

static void e4__builtin_FFI_COMMA(struct e4__task *task, e4__cell user)
{
    register e4__usize val;
    register e4__cell ct;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    ct = e4__stack_pop(task);
    val = (e4__usize)e4__stack_pop(task);

    #define _tfcase(t, v)   \
        if (ct == (e4__cell)t) {    \
            register v *c = (v *)task->here;    \
            *c++ = (v)val;  \
            task->here = (e4__cell)c;   \
        }
    #define _tfcase_signed(t, v)    \
        if (ct == (e4__cell)t) {    \
            register v *c = (v *)task->here;    \
            *c++ = e4__USIZE_IS_NEGATIVE(val) ? \
                -(v)e4__USIZE_NEGATE(val) : (v)val; \
            task->here = (e4__cell)c;   \
        }

    _tfcases_nonvoid()
    else
        e4__exception_throw(task, e4__E_BADFFITYPE);

    #undef _tfcase_signed
    #undef _tfcase

    e4__execute_ret(task);
}

static void e4__builtin_FFI_DEF(struct e4__task *task, e4__cell user)
{
    register const char *word;
    register e4__u8 len;
    register e4__cell return_type, arg_type, *argv;
    register e4__usize argc, i, res;
    register ffi_cif *cif;
    register ffi_type **type_vector;
    register ffi_status ffi_res;
    register void *func;

    #define _tfcase(t, v)   \
        if (arg_type == (e4__cell)t) (void)NULL;
    #define _tfcase_signed _tfcase

    _e4__BUILTIN_EXPECT_DEPTH(task, 3);
    _e4__BUILTIN_LOOKAHEAD(task, word, len);

    func = (void *)e4__stack_pop(task);
    return_type = e4__stack_pop(task);
    argc = (e4__usize)e4__stack_pop(task);

    _e4__BUILTIN_EXPECT_DEPTH(task, argc);

    arg_type = return_type;
    _tfcases_nonvoid()
    else if (arg_type != (e4__cell)&ffi_type_void) {
        e4__exception_throw(task, e4__E_BADFFITYPE);
        e4__execute_ret(task);
        return;
    }

    if (e4__task_compiling(task)) {
        e4__exception_throw(task, e4__E_NESTEDCOMPILE);
        e4__execute_ret(task);
        return;
    }

    e4__dict_entry(task, word, len, 0, NULL, NULL);

    cif = (ffi_cif *)e4__task_allot(task, e4__mem_aligned(sizeof(ffi_cif)));
    e4__compile_cell(task, (e4__cell)func);
    e4__compile_cell(task, return_type);
    e4__compile_cell(task, (e4__cell)argc);

    type_vector = (ffi_type **)task->here;

    /* Arguments must be stored in the opposite of the order they are
       in on the stack, so it is necessary to process them in this
       way. */
    argv = (e4__cell *)&task->sp[0];
    for (i = argc; i > 0; --i) {
        /* Validate argument type. */
        arg_type = argv[i];

        _tfcases_nonvoid()
        else {
            res = e4__E_BADFFITYPE;
            goto out_forget;
        }

        e4__compile_cell(task, arg_type);
    }

    /* Remove arguments from the stack. */
    task->sp += argc;

    /* Prepare libffi cif. */
    ffi_res = ffi_prep_cif(cif, FFI_DEFAULT_ABI, (unsigned int)argc,
            (ffi_type *)return_type, type_vector);
    if (ffi_res != FFI_OK) {
        if (ffi_res == FFI_BAD_TYPEDEF || ffi_res == FFI_BAD_ARGTYPE)
            res = e4__E_BADFFITYPE;
        else if (ffi_res == FFI_BAD_ABI) {
            /* XXX: We've selected the default ABI, so this should never
               happen. */
            res = e4__E_BADFFIABI;
        } else
            res = e4__E_UNKNOWNFFIERR;

        goto out_forget;
    }

    if ((res = e4__compile_start(task, task->dict->xt, e4__COMP_DOES))) {
        goto out_forget;
    }

    e4__compile_cell(task, (e4__cell)&e4__BUILTIN_XT[e4__B_FFI_CALL]);

    if ((res = e4__compile_finish(task))) {
        /* XXX: This should never happen. */
        goto out_forget;
    }

    #undef _tfcase_signed
    #undef _tfcase

    e4__execute_ret(task);
    return;

out_forget:
    e4__dict_forget(task, task->dict->name, task->dict->nbytes);
    e4__exception_throw(task, res);
    e4__execute_ret(task);
}

static void e4__builtin_FFI_FETCH(struct e4__task *task, e4__cell user)
{
    register e4__cell dt, addr;
    register e4__usize val = 0;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    dt = e4__stack_pop(task);
    addr = e4__stack_pop(task);

    #define _tfcase(t, v)   \
        if (dt == (e4__cell)t) \
            val = (e4__usize)*((v *)addr);
    #define _tfcase_signed(t, v)    \
        if (dt == (e4__cell)t)  {   \
            register v tmp = *((v *)addr);  \
            val = tmp < 0 ?   \
                e4__USIZE_NEGATE((e4__usize)-tmp) : (e4__usize)tmp; \
        }


    _tfcases_nonvoid()
    else {
        e4__exception_throw(task, e4__E_BADFFITYPE);
        e4__execute_ret(task);
        return;
    }

    #undef _tfcase_signed
    #undef _tfcase

    e4__stack_push(task, (e4__cell)val);

    e4__execute_ret(task);
}

static void e4__builtin_FFI_STORE(struct e4__task *task, e4__cell user)
{
    register e4__cell dt, addr;
    register e4__usize val;

    _e4__BUILTIN_EXPECT_DEPTH(task, 3);

    dt = e4__stack_pop(task);
    addr = e4__stack_pop(task);
    val = (e4__usize)e4__stack_pop(task);

    #define _tfcase(t, v)   \
        if (dt == (e4__cell)t)  \
            *((v *)addr) = (v)val;
    #define _tfcase_signed(t, v)    \
        if (dt == (e4__cell)t)  \
            *((v *)addr) = e4__USIZE_IS_NEGATIVE(val) ? \
                -(v)e4__USIZE_NEGATE(val) : (v)val;

    _tfcases_nonvoid()
    else
        e4__exception_throw(task, e4__E_BADFFITYPE);

    #undef _tfcase_signed
    #undef _tfcase

    e4__execute_ret(task);
}

static void e4__builtin_FFI_UNITS(struct e4__task *task, e4__cell user)
{
    register e4__cell ut;
    register e4__usize val;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    ut = e4__stack_pop(task);
    val = (e4__usize)e4__stack_pop(task);

    #define _tfcase(t, v)   \
        if (ut == (e4__cell)t) val *= sizeof(v);
    #define _tfcase_signed _tfcase

    _tfcases_nonvoid()
    else {
        e4__exception_throw(task, e4__E_BADFFITYPE);
        e4__execute_ret(task);
        return;
    }

    #undef _tfcase_signed
    #undef _tfcase

    e4__stack_push(task, (e4__cell)val);

    e4__execute_ret(task);
}

#undef _tfcases_nonvoid

/* vim: set ft=c: */
