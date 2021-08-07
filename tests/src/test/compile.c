#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

/* Covers : ; and linear compilation semantics */
static void e4t__test_compile_linear(void)
{
    struct e4__task *task = e4t__transient_task();
    struct e4__dict_header *header;

    /* Test that attempting to define a word with no name
       throws an exception. */
    e4t__ASSERT_EQ(e4__evaluate(task, ":", -1), e4__E_ZLNAME);
    e4t__ASSERT_EQ(e4__evaluate(task, ":   ", -1), e4__E_ZLNAME);

    /* Test that compiled words are added to the dictionary. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo ;", -1));
    e4t__ASSERT(e4__dict_lookup(task, "foo", 3));
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));

    /* Test that simply numeric compilation words. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 17 ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 17);

    /* Test that compiling over a word works as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 43 ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 43);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 17);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));

    /* Test that compilation state is preserved across multiple
       evaluate calls. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "2", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "3", -1));
    e4t__ASSERT(e4__task_compiling(task));
    e4t__ASSERT_OK(e4__evaluate(task, "4 ;", -1));
    e4t__ASSERT_EQ(e4__task_compiling(task), 0);
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));

    /* Test that simple linear compilation is possible. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 2 5 + ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": bar dup 1 + swap ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 7);
    e4t__ASSERT_OK(e4__evaluate(task, "foo bar", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 7);
    e4t__ASSERT_EQ(e4__stack_pop(task), 8);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));

    /* Test that a stack mismatch at compilation start and end throws
       the appropriate exception and cancels compilation. */
    e4__stack_push(task, (e4__cell)0x1234);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 2", -1));
    e4__stack_push(task, (e4__cell)0x5678);
    e4t__ASSERT_EQ(e4__evaluate(task, ";", -1), e4__E_CSMISMATCH);
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4__stack_pop(task);
    e4__stack_pop(task);
    e4t__ASSERT_EQ(e4__dict_lookup(task, "foo", 3), 0);

    /* Test that compiled code is as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 2 5 + ;", -1));
    e4t__ASSERT((header = e4__dict_lookup(task, "foo", 3)));
    e4t__ASSERT_EQ(header->xt->code, e4__execute_threaded);
    e4t__ASSERT_EQ(header->xt->data[0], &e4__BUILTIN_XT[e4__B_LITERAL]);
    e4t__ASSERT_EQ(header->xt->data[1], 2);
    e4t__ASSERT_EQ(header->xt->data[2], &e4__BUILTIN_XT[e4__B_LITERAL]);
    e4t__ASSERT_EQ(header->xt->data[3], 5);
    e4t__ASSERT_EQ(header->xt->data[4], &e4__BUILTIN_XT[e4__B_PLUS]);
    e4t__ASSERT_EQ(header->xt->data[5], &e4__BUILTIN_XT[e4__B_EXIT]);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));
}

void e4t__test_compile(void)
{
    e4t__test_compile_linear();
}
