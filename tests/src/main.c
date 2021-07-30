#include "e4.h"
#include "e4t.h"
#include <stdio.h>

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
    /* FIXME: Move this code somewhere better. */
    struct e4__task *task;

    /* Test asserts themselves. Sanity check. */
    e4t__ASSERT_OK(e4__E_OK);
    e4t__ASSERT_EQ(923816, 923816);
    e4t__ASSERT_MATCH("Dade Murphy", "Dade Murphy");
    e4t__ASSERT_MATCH("d-link", "D-LINK");
    e4t__ASSERT(e4__mem_strncasecmp("El Psy Kongroo", "El Psy Kongaly", -1));

    /* Test e4t utilities. */
    e4t__term_selftest();

    /* Call into actual tests functions. */
    /* FIXME: Move these tests somewhere appropriate. */
    task = e4t__transient_task();
    e4t__ASSERT_OK(e4__evaluate(task, "1 -2 3 .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 1 -2 3");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 clear .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<0>");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 drop .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<2> 1 2");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 dup .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 2 3 3");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 over .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 2 3 2");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 rot .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 2 3 1");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 swap .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 1 3 2");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 tuck .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 3 2 3");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 4 5 4 roll .s clear", -1, 0));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<5> 1 3 4 5 2");

    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 4 5 4 rll .s clear", -1, 0),
            e4__E_UNDEFWORD);
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "");

    /* Summarize test results. */
    fprintf(stderr, "Tests complete! (%lu of %lu passed; %lu failed)\n",
            e4t__assert_attemptcount - e4t__assert_failcount,
            e4t__assert_attemptcount, e4t__assert_failcount);

    /* Return success only if all tests passed. */
    return !!e4t__assert_failcount;
}
