#include "e4.h"
#include "kernel/e4-internal.h"

#include <stdlib.h>

/* C89 does not require that function pointers and void pointers be
   the same size. We do. */
e4__ASSERT(sizeof(void *) == sizeof(e4__code_ptr));

/* Similarly, C89 does not require that void* and void** be the same
   size. In practice, there is virtually no reason why they would
   ever not be, so we require that they are. */
e4__ASSERT(sizeof(void *) == sizeof(e4__cell));

/* Finally, we require an integer type that is capable of holding
   any pointer value. This is the most likely assert to fire, because
   while it's extremely unlikely that any given C compiler for any
   given architecture *does* provide an integral type that is the
   same size as the pointer type constrained by the two asserts above,
   the C89 standard itself does *not* provide a good way to detect
   what type that might happen to be.

   C99 provides uintptr_t and if __STDC_VERSION__ indicates that type
   is likely available, e4 tries to use it. If this is not the case,
   e4 attempts to deduce the appropriate type with a series of hacks
   that will probably work fine if your compiler is based on GCC
   Clang, or MSVC. If even this doesn't work, which is likely because
   your compiler or system is somewhat esoteric, then e4 tries using
   size_t as a last ditch effort. In the unlikely event that none of
   this works, which is probably the reason why you're reading this
   comment, the following assert will prevent an unusable system from
   ever compiling.

   If you're seeing this assert fire and you want to get get e4
   building, you have a couple of options. The first and easiest thing
   to try is to provide an appropriate e4__WORDSIZE preprocessor
   definition to e4 using your build system. e4__WORDSIZE should be set
   to the number of bits used to store a void* on your target system.
   For instance, on a 32 bit architecture e4__WORDSIZE should be set
   to 32, while on a 64 bit architecture e4__WORDSIZE should be set to
   64.

   If your system is truly quirky and even this does not work, or if
   things are broken for some other reason, the last resort is to
   simply provide the appropriate type definition yourself. If the
   preprocessor identifier e4__LOAD_CONFIG is defined, e4 will attempt
   to include a file named e4-config.h in all relevant locations. In
   this header, an appropriate e4__usize type can be defined. If this is
   done, the preprocessor identifier e4__USIZE_DEFINED should also be
   defined to indicate to e4 that it is no longer responsible for
   providing the e4__usize type.

   For example, assuming your system is 39 bit and your system headers
   provide some uint39_t type, then using your build system
   e4__LOAD_CONFIG should be defined and made available to e4 and
   e4-config.h should be made to contain the following:

        typedef uint39_t e4__usize;
        #define e4__USIZE_DEFINED

   If this is not possible on your system because there literally does
   not exist any integral type that is the same width as a pointer
   type, then it will not be possible to compile e4 for your system
   without making significant modifications. Thus far, no such system
   has been encountered, however if you believe you have found one,
   please don't hesitate to contact whoever is currently maintaining
   this code and instruct them to update this comment. */
e4__ASSERT(sizeof(e4__cell) == sizeof(e4__usize));

/* Ensure that e4__VERSION_NUMBER is a proper integral semantic version
   number with 3 base-10 digits for minor and patch numbers and that
   e4__VERSION_NUMBER has not been updated carelessly or erroneously. */
e4__ASSERT(e4__VERSION_NUMBER == (0 * 100000 + 1 * 1000 + 0 * 1));

/* Ensure the e4__NARGS macro works correctly across its target domain. */
e4__ASSERT(e4__NARGS(1) == 1);
e4__ASSERT(e4__NARGS(1, 2) == 2);
e4__ASSERT(e4__NARGS(1, 2, 3) == 3);
e4__ASSERT(e4__NARGS(1, 2, 3, 4) == 4);
e4__ASSERT(e4__NARGS(1, 2, 3, 4, 5) == 5);
e4__ASSERT(e4__NARGS(1, 2, 3, 4, 5, 6) == 6);
e4__ASSERT(e4__NARGS(1, 2, 3, 4, 5, 6, 7) == 7);
e4__ASSERT(e4__NARGS(1, 2, 3, 4, 5, 6, 7, 8) == 8);
e4__ASSERT(e4__NARGS(1, 2, 3, 4, 5, 6, 7, 8, 9) == 9);
e4__ASSERT(e4__NARGS(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) == 10);

/* The hack used to access user table variables requires that fields in
   the task struct are aligned at e4__cell width. */
/* FIXME: Add asserts for all fields accessed this way once field
   offset IDs are defined. */
e4__ASSERT(&((struct e4__task *)NULL)->here ==
        &((e4__cell *)NULL)[e4__UV_HERE]);
e4__ASSERT(&((struct e4__task *)NULL)->pad == &((e4__cell *)NULL)[e4__UV_PAD]);
e4__ASSERT(&((struct e4__task *)NULL)->base_ptr ==
        &((e4__cell *)NULL)[e4__UV_BASE]);
