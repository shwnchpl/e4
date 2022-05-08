/* FFI word list declarations. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

#define _e4__BUILTIN_FFI_DECL() \
    _e4__BUILTIN_THUNK_N(FFI_ALIGN, "FFI-ALIGN")    \
    _e4__BUILTIN_PROC_N(FFI_ALIGNED, "FFI-ALIGNED") \
    _e4__BUILTIN_PROC(FFI_CALL) \
    _e4__BUILTIN_PROC_N(FFI_COMMA, "FFI,")  \
    _e4__BUILTIN_PROC_N(FFI_DEF, "FFI-DEF") \
    _e4__BUILTIN_FFICONST(FFI_DOUBLE, "FFI-DOUBLE", &ffi_type_double,   \
            double) \
    _e4__BUILTIN_PROC_N(FFI_FETCH, "FFI@")  \
    _e4__BUILTIN_FFICONST(FFI_FLOAT, "FFI-FLOAT", &ffi_type_float, float)   \
    _e4__BUILTIN_FFICONST(FFI_POINTER, "FFI-POINTER", &ffi_type_pointer,    \
            void *) \
    _e4__BUILTIN_FFICONST(FFI_SCHAR, "FFI-SCHAR", &ffi_type_schar,  \
            signed char)    \
    _e4__BUILTIN_FFICONST(FFI_SINT, "FFI-SINT", &ffi_type_sint, signed int) \
    _e4__BUILTIN_FFICONST(FFI_SINT8, "FFI-SINT8", &ffi_type_sint8, e4__s8)  \
    _e4__BUILTIN_FFICONST(FFI_SINT16, "FFI-SINT16", &ffi_type_sint16, e4__s16)\
    _e4__BUILTIN_FFICONST(FFI_SINT32, "FFI-SINT32", &ffi_type_sint32, e4__s32)\
    _e4__BUILTIN_FFICONST(FFI_SINT64, "FFI-SINT64", &ffi_type_sint64, e4__s64)\
    _e4__BUILTIN_FFICONST(FFI_SLONG, "FFI-SLONG", &ffi_type_slong,  \
            signed long) \
    _e4__BUILTIN_FFICONST(FFI_SSHORT, "FFI-SSHORT", &ffi_type_sshort,   \
            signed short) \
    _e4__BUILTIN_PROC_N(FFI_STORE, "FFI!")  \
    _e4__BUILTIN_FFICONST(FFI_UCHAR, "FFI-UCHAR", &ffi_type_uchar,  \
            unsigned char)  \
    _e4__BUILTIN_FFICONST(FFI_UINT, "FFI-UINT", &ffi_type_uint, unsigned int) \
    _e4__BUILTIN_FFICONST(FFI_UINT8, "FFI-UINT8", &ffi_type_uint8, e4__u8)  \
    _e4__BUILTIN_FFICONST(FFI_UINT16, "FFI-UINT16", &ffi_type_uint16, e4__u16)\
    _e4__BUILTIN_FFICONST(FFI_UINT32, "FFI-UINT32", &ffi_type_uint32, e4__u32)\
    _e4__BUILTIN_FFICONST(FFI_UINT64, "FFI-UINT64", &ffi_type_uint64, e4__u64)\
    _e4__BUILTIN_FFICONST(FFI_ULONG, "FFI-ULONG", &ffi_type_ulong,  \
            unsigned long)  \
    _e4__BUILTIN_PROC_N(FFI_UNITS, "FFI-UNITS") \
    _e4__BUILTIN_FFICONST(FFI_USHORT, "FFI-USHORT", &ffi_type_ushort,   \
            unsigned short) \
    _e4__BUILTIN_CONSTANT_N(FFI_VOID, "FFI-VOID", &ffi_type_void)

/* vim: set ft=c: */
