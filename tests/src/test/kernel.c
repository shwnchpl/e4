#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

static void e4t__test_kernel_evaluate(void)
{
    struct e4__task *task = e4t__transient_task();

    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 4 5 4 rll .s clear", -1, 0),
            e4__E_UNDEFWORD);
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "");
}

void e4t__test_kernel(void)
{
    e4t__test_kernel_evaluate();
}
