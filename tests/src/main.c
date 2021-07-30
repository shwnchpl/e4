#include "e4.h"
#include "e4t.h"
#include <stdio.h>

unsigned long e4t__assert_attemptcount = 0;
unsigned long e4t__assert_failcount = 0;

int main(void)
{
    /* Test asserts themselves. Sanity check. */
    e4t__ASSERT_OK(e4__E_OK);
    e4t__ASSERT_EQ(923816, 923816);
    e4t__ASSERT_MATCH("Dade Murphy", "Dade Murphy");
    e4t__ASSERT_MATCH("d-link", "D-LINK");
    e4t__ASSERT(e4__mem_strncasecmp("El Psy Kongroo", "El Psy Kongaly", -1));

    /* Call into actual tests functions. */

    /* Summarize test results. */
    fprintf(stderr, "Tests complete! (%lu of %lu passed; %lu failed)\n",
            e4t__assert_attemptcount - e4t__assert_failcount,
            e4t__assert_attemptcount, e4t__assert_failcount);

    /* Return success only if all tests passed. */
    return !!e4t__assert_failcount;
}
