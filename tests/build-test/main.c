#include "e4.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* XXX: Most of these are currently unused, and some will never be
   used, but define them here now to make the logic behind wordset
   digest values clear. */
#define CORE_EXT_BIT        0
#define BLOCK_BIT           1
#define BLOCK_EXT_BIT       2
#define DOUBLE_BIT          3
#define DOUBLE_EXT_BIT      4
#define EXCEPTION_BIT       5
#define EXCEPTION_EXT_BIT   6
#define FACILITY_BIT        7
#define FACILITY_EXT_BIT    8
#define FILE_BIT            9
#define FILE_EXT_BIT        10
#define FLOAT_BIT           11
#define FLOAT_EXT_BIT       12
#define LOCALS_BIT          13
#define LOCALS_EXT_BIT      14
#define MEMORY_BIT          15
#define MEMORY_EXT_BIT      16
#define TOOLS_BIT           17
#define TOOLS_EXT_BIT       18
#define SEARCH_BIT          19
#define SEARCH_EXT_BIT      20
#define STRING_BIT          21
#define STRING_EXT_BIT      22
#define XCHAR_BIT           23
#define XCHAR_EXT_BIT       24
#define POSIX_HOOKS_BIT     25

/* ASCII escape sequences for simple colored output. */
#define COLOR_RESET         "\033[0m"
#define COLOR_RED           "\033[0;31m"
#define COLOR_GREEN         "\033[0;32m"

int main(int argc, char **argv)
{
    static char task_buffer[4096];
    struct e4__task *task;
    const char *name;

    task = e4__task_create(task_buffer, sizeof(task_buffer));

    if (!(name = strrchr(argv[0], '/')) || !*++name)
        name = argv[0];

    /* Are things working on a basic level? */
    if (e4__evaluate(task, "2 dup +", -1) || e4__stack_depth(task) != 1 ||
            (e4__usize)e4__stack_pop(task) != 4) {
        fprintf(stderr, COLOR_RED "%s: Failed - "
                "evaluate(\"2 dup +\") did not yield expected results.\n"
                COLOR_RESET, name);
        exit(EXIT_FAILURE);
    }

    /* Ensure that we built with the wordset combo requested. */
    if (argc > 1) {
        long expected = strtol(argv[1], NULL, 0);
        long wordset = 0;

        #if defined(e4__INCLUDE_CORE_EXT)
            wordset |= (1 << CORE_EXT_BIT);
        #endif
        #if defined(e4__INCLUDE_EXCEPTION)
            wordset |= (1 << EXCEPTION_BIT);
        #endif
        #if defined(e4__INCLUDE_FACILITY)
            wordset |= (1 << FACILITY_BIT);
        #endif
        #if defined(e4__INCLUDE_FACILITY_EXT)
            wordset |= (1 << FACILITY_EXT_BIT);
        #endif
        #if defined(e4__INCLUDE_FILE)
            wordset |= (1 << FILE_BIT);
        #endif
        #if defined(e4__INCLUDE_FILE_EXT)
            wordset |= (1 << FILE_EXT_BIT);
        #endif
        #if defined(e4__INCLUDE_POSIX_HOOKS)
            wordset |= (1 << POSIX_HOOKS_BIT);
        #endif
        #if defined(e4__INCLUDE_TOOLS)
            wordset |= (1 << TOOLS_BIT);
        #endif
        #if defined(e4__INCLUDE_TOOLS_EXT)
            wordset |= (1 << TOOLS_EXT_BIT);
        #endif

        if (wordset != expected) {
            fprintf(stderr, COLOR_RED "%s: Failed - "
                    "expected wordset digest %#lx, got %#lx.\n" COLOR_RESET,
                    name, expected, wordset);
            exit(EXIT_FAILURE);
        }
    }

    fprintf(stderr, COLOR_GREEN"%s: Passed!\n" COLOR_RESET, name);

    return 0;
}
