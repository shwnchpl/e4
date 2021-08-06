#include "e4.h"
#include "e4t.h"

#include <stdio.h>
#include <stdlib.h>

unsigned long e4t__assert_attemptcount = 0;
unsigned long e4t__assert_failcount = 0;

/* Returns a temporary task struct. Valid only until this function
   is next called. */
struct e4__task* e4t__transient_task(void)
{
    static char buffer[4096];
    struct e4__task *task;

    task = e4__task_create(buffer, sizeof(buffer));
    e4t__term_io_init(task);

    return task;
}

int main(void)
{
    /* Test asserts themselves. Sanity check. */
    e4t__ASSERT_OK(e4__E_OK);
    e4t__ASSERT_EQ(923816, 923816);
    e4t__ASSERT_MATCH("Dade Murphy", "Dade Murphy");
    e4t__ASSERT_MATCH("d-link", "D-LINK");
    e4t__ASSERT(e4__mem_strncasecmp("El Psy Kongroo", "El Psy Kongaly", -1));

    /* Test e4t utilities. */
    e4t__term_selftest();

    /* Call into actual tests functions. */
    e4t__test_builtin();
    e4t__test_compile();
    e4t__test_execute();
    e4t__test_kernel();

    fprintf(stderr, "Tests complete! (%lu of %lu passed; %lu failed)\n",
            e4t__assert_attemptcount - e4t__assert_failcount,
            e4t__assert_attemptcount, e4t__assert_failcount);

    /* Return success only if all tests passed. */
    return !e4t__assert_failcount ? EXIT_SUCCESS : EXIT_FAILURE;
}
