#define _POSIX_C_SOURCE 200809L
#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* Covers *SLASH *SLASH-MOD FM/MOD M* SM/DIV S>D UM* UM/MOD */
static void e4t__test_builtin_doublemath(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test S>D. */
    e4t__ASSERT_OK(e4__evaluate(task, "0 s>d", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "57 s>d", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_EQ(e4__stack_pop(task), 57);

    e4t__ASSERT_OK(e4__evaluate(task, "-30 s>d", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -30);

    /* Test M* and UM*. */
    e4t__ASSERT_OK(e4__evaluate(task, "5 -20 M*", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -100);

    e4t__ASSERT_OK(e4__evaluate(task, "0 20 M*", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "-1 1 rshift 4 M*", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -4);

    e4t__ASSERT_OK(e4__evaluate(task, "-1 2 UM*", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -2);

    e4t__ASSERT_OK(e4__evaluate(task, "-1 -1 UM*", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);

    /* Test FM/MOD, UM/MOD, and SM/REM. The utility functions behind
       these builtins are covered more extensively in util.c, so just
       make sure those utilities are being invoked correctly. */
    e4t__ASSERT_OK(e4__evaluate(task, "10 s>d 7 fm/mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);

    e4t__ASSERT_OK(e4__evaluate(task, "-10 s>d 7 fm/mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);

    e4t__ASSERT_OK(e4__evaluate(task, "10 s>d -7 fm/mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -4);

    e4t__ASSERT_OK(e4__evaluate(task, "-10 s>d -7 fm/mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -3);

    e4t__ASSERT_OK(e4__evaluate(task, "0 s>d 1 um/mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "1 s>d 1 um/mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "1 s>d 2 um/mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);

    e4t__ASSERT_OK(e4__evaluate(task, "3 s>d 2 um/mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);

    e4t__ASSERT_OK(e4__evaluate(task, "10 s>d 7 sm/rem", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);

    e4t__ASSERT_OK(e4__evaluate(task, "-10 s>d 7 sm/rem", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -3);

    e4t__ASSERT_OK(e4__evaluate(task, "10 s>d -7 sm/rem", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);

    e4t__ASSERT_OK(e4__evaluate(task, "-10 s>d -7 sm/rem", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -3);

    /* Test *SLASH and *SLASH-MOD. */
    e4t__ASSERT_OK(e4__evaluate(task, ": max-int -1 1 rshift ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": min-int max-int negate 1- ;", -1));

    e4t__ASSERT_EQ(e4__evaluate(task, "1 0 0 */mod", -1), e4__E_DIVBYZERO);
    e4t__ASSERT_EQ(e4__evaluate(task, "max-int 2 1 */mod", -1),
            e4__E_RSLTOUTORANGE);
    e4t__ASSERT_EQ(e4__evaluate(task, "min-int 2 1 */mod", -1),
            e4__E_RSLTOUTORANGE);

    e4t__ASSERT_OK(e4__evaluate(task, "0 2 1 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 1 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "2 2 1 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "-1 2 1 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "-2 2 1 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "0 2 -1 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 -1 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "2 2 -1 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "-1 2 -1 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "-2 2 -1 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "2 2 2 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "-1 2 -1 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "-2 2 -2 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "7 2 3 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);

    e4t__ASSERT_OK(e4__evaluate(task, "7 2 -3 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);

    e4t__ASSERT_OK(e4__evaluate(task, "-7 2 3 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -4);
    e4t__ASSERT_EQ(e4__stack_pop(task), -2);

    e4t__ASSERT_OK(e4__evaluate(task, "-7 2 -3 */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), -2);

    e4t__ASSERT_OK(e4__evaluate(task, "max-int 2 max-int */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "min-int 2 min-int */mod", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_EQ(e4__evaluate(task, "1 0 0 */", -1), e4__E_DIVBYZERO);
    e4t__ASSERT_EQ(e4__evaluate(task, "max-int 2 1 */", -1),
            e4__E_RSLTOUTORANGE);
    e4t__ASSERT_EQ(e4__evaluate(task, "min-int 2 1 */", -1),
            e4__E_RSLTOUTORANGE);

    e4t__ASSERT_OK(e4__evaluate(task, "0 2 1 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 1 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);

    e4t__ASSERT_OK(e4__evaluate(task, "2 2 1 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);

    e4t__ASSERT_OK(e4__evaluate(task, "-1 2 1 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -2);

    e4t__ASSERT_OK(e4__evaluate(task, "-2 2 1 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -4);

    e4t__ASSERT_OK(e4__evaluate(task, "0 2 -1 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 -1 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -2);

    e4t__ASSERT_OK(e4__evaluate(task, "2 2 -1 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -4);

    e4t__ASSERT_OK(e4__evaluate(task, "-1 2 -1 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);

    e4t__ASSERT_OK(e4__evaluate(task, "-2 2 -1 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);

    e4t__ASSERT_OK(e4__evaluate(task, "2 2 2 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);

    e4t__ASSERT_OK(e4__evaluate(task, "-1 2 -1 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);

    e4t__ASSERT_OK(e4__evaluate(task, "-2 2 -2 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);

    e4t__ASSERT_OK(e4__evaluate(task, "7 2 3 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);

    e4t__ASSERT_OK(e4__evaluate(task, "7 2 -3 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -4);

    e4t__ASSERT_OK(e4__evaluate(task, "-7 2 3 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -4);

    e4t__ASSERT_OK(e4__evaluate(task, "-7 2 -3 */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);

    e4t__ASSERT_OK(e4__evaluate(task, "max-int 2 max-int */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);

    e4t__ASSERT_OK(e4__evaluate(task, "min-int 2 min-int */", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
}

/* Covers ( \ */
static void e4t__test_builtin_comments(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that comments work correctly when interpreting. */
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 ( this is a comment) 3", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);
    e4t__ASSERT_OK(e4__evaluate(task, "( no need to follow with space)3", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);
    e4t__ASSERT_OK(e4__evaluate(task, "5 4 ( happy to term at EOL", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "3", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    e4t__ASSERT_OK(e4__evaluate(task, "5 4 \\ term at EOL", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "3", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    /* Test that comments work correctly when compiling. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 1 2 ( this is a comment) 3 ; foo",
            -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);
    e4t__ASSERT_OK(e4__evaluate(task,
            ": foo ( no need to follow with space)3 ; foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo 5 4 ( term at EOL", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "3 ; foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    e4t__ASSERT_OK(e4__evaluate(task, ": foo 5 4 \\ term at EOL", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "3 ; foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
}

/* Covers BL FALSE TRUE */
static void e4t__test_builtin_constants(void)
{
    struct e4__task *task = e4t__transient_task();

    e4t__ASSERT_OK(e4__evaluate(task, "bl", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), ' ');

    e4t__ASSERT_OK(e4__evaluate(task, "true", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);

    e4t__ASSERT_OK(e4__evaluate(task, "false", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
}

/* Covers , ' ! ? >BODY ALLOT CELLS CONSTANT CREATE EXECUTE TO VALUE
   VARIABLE */
static void e4t__test_builtin_data(void)
{
    struct e4__task *task = e4t__transient_task();
    const struct e4__dict_header *header;

    e4t__term_obuf_consume();

    /* Test simple creation with allot allocation. */
    e4t__ASSERT_OK(e4__evaluate(task, "create foo 1 cells allot 531 foo !",
            -1));
    e4t__ASSERT((header = e4__dict_lookup(task, "foo", 3)));
    e4t__ASSERT_EQ(header->xt->data[0], 531);

    header->xt->data[0] = (e4__cell)7393;
    e4t__ASSERT_OK(e4__evaluate(task, "foo ?", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "7393 ");

    /* Test creation with comma allocation. */
    e4t__ASSERT_OK(e4__evaluate(task, "create bar 1234 , 5678 ,", -1));
    e4t__ASSERT((header = e4__dict_lookup(task, "bar", 3)));
    e4t__ASSERT_EQ(header->xt->data[0], 1234);
    e4t__ASSERT_EQ(header->xt->data[1], 5678);

    /* Test constant creation. */
    e4t__ASSERT_OK(e4__evaluate(task, "124816 constant bas bas", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 124816);

    /* Test variable creation. */
    e4t__ASSERT_OK(e4__evaluate(task, "variable quux 152938 quux !", -1));
    e4t__ASSERT((header = e4__dict_lookup(task, "quux", 4)));
    e4t__ASSERT_EQ(header->xt->data[0], 152938);

    /* Check that lookup of a word that hasn't been defined fails. */
    e4t__ASSERT_EQ(e4__evaluate(task, "' notaword", -1), e4__E_UNDEFWORD);

    /* Test variable lookup and modification. */
    e4t__ASSERT_OK(e4__evaluate(task, "' quux", -1));
    e4t__ASSERT_EQ(header->xt, e4__stack_peek(task));
    e4t__ASSERT_OK(e4__evaluate(task, ">body 523407 swap !", -1));
    e4t__ASSERT_EQ(header->xt->data[0], 523407);

    /* Test execution of looked up values. */
    e4t__ASSERT_OK(e4__evaluate(task, "51 49 ' + execute", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 100);
    e4t__ASSERT_OK(e4__evaluate(task, "35 constant foo ' foo execute", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 35);

    /* Test creating and updating a mutable constant (ie. value). */
    e4t__ASSERT_OK(e4__evaluate(task, "35 value foo foo", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 35);
    e4t__ASSERT_OK(e4__evaluate(task, "45 to foo foo", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 45);

    e4t__ASSERT_EQ(e4__evaluate(task, ": set-flom to", -1), e4__E_ZLNAME);
    e4__compile_cancel(task);
    e4t__ASSERT_EQ(e4__evaluate(task, ": set-flom to flom ;", -1),
            e4__E_UNDEFWORD);
    e4__compile_cancel(task);
    e4t__ASSERT_EQ(e4__evaluate(task, ": set-flom to ;", -1),
            e4__E_INVNAMEARG);
    e4__compile_cancel(task);

    e4__stack_clear(task);

    /* Test that attempting to run TO on something other than a value
       or an invalid name doesn't work. */
    e4t__ASSERT_OK(e4__evaluate(task, "35 constant bar", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "25 to bar", -1), e4__E_INVNAMEARG);
    e4t__ASSERT_EQ(e4__evaluate(task, "25 to notaname", -1), e4__E_UNDEFWORD);
}

/* Covers :NONAME ABORT DEFER DEFER! DEFER@ */
static void e4t__test_builtin_defer(void)
{
    struct e4__task *task = e4t__transient_task();

    e4t__ASSERT_OK(e4__evaluate(task, "defer foo", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "foo", -1), e4__E_ABORT);
    e4t__ASSERT_OK(e4__evaluate(task, "' foo defer@", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), &e4__BUILTIN_XT[e4__B_ABORT]);

    e4t__ASSERT_OK(e4__evaluate(task, ":noname 2 2 + ; ' foo defer!", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);
}

/* Covers , @ ' ! >BODY CREATE DOES> (interpret) PLUS */
static void e4t__test_builtin_does(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that DOES> behaves correctly at interpret time. */
    e4t__ASSERT_OK(e4__evaluate(task, "create f 20 , does> @ 55 + ; f", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 75);
    e4t__ASSERT_OK(e4__evaluate(task, "103 ' f >body ! f", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 158);
}

/* Covers ENVIRONMENT? */
static void e4t__test_builtin_environmentq(void)
{
    struct e4__task *task = e4t__transient_task();

    e4t__ASSERT_OK(e4__evaluate(task, "s\" junk\" environment?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "s\" /counted-string\" environment?",
            -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 255);

    e4t__ASSERT_OK(e4__evaluate(task, "s\" /hold\" environment?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__PNO_MIN_SZ);

    e4t__ASSERT_OK(e4__evaluate(task, "s\" /pad\" environment?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 208);

    e4t__ASSERT_OK(e4__evaluate(task, "s\" address-unit-bits\" environment?",
            -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__USIZE_BIT);

    e4t__ASSERT_OK(e4__evaluate(task, "s\" floored\" environment?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, "s\" max-char\" environment?",
            -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 255);

    e4t__ASSERT_OK(e4__evaluate(task, "s\" max-d\" environment?",
            -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), (e4__usize)-1 >> 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);

    e4t__ASSERT_OK(e4__evaluate(task, "s\" max-n\" environment?",
            -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), (e4__usize)-1 >> 1);

    e4t__ASSERT_OK(e4__evaluate(task, "s\" max-u\" environment?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);

    e4t__ASSERT_OK(e4__evaluate(task, "s\" max-ud\" environment?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);

    e4t__ASSERT_OK(e4__evaluate(task, "s\" return-stack-cells\" environment?",
            -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 52);

    e4t__ASSERT_OK(e4__evaluate(task, "s\" stack-cells\" environment?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 51);
}

/* Covers EVALUATE S" SOURCE-ID */
static void e4t__test_builtin_evaluate(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that calling EVALUATE evaluates provided strings. */
    e4t__ASSERT_OK(e4__evaluate(task, ": define-foo s\" : foo 2 2 + ;\" ;",
            -1));
    e4t__ASSERT_OK(e4__evaluate(task, "define-foo evaluate", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);

    /* Test that SOURCE-ID is set correctly when evaluating. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo s\" source-id\" ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo evaluate", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__SID_STR);
}

/* Covers ABORT ABORTQ BYE CATCH THROW QUIT */
static void e4t__test_builtin_exceptions(void)
{
    struct e4__task *task = e4t__transient_task();
    e4__usize len;
    const char *msg = NULL;

    /* XXX: Detailed stack and return stack preservation semantics are
       covered by e4t__test_kernel_exceptions in kernel.c. */

    /* Test that ABORT throws e4__E_ABORT. */
    e4t__ASSERT_EQ(e4__evaluate(task, "abort", -1), e4__E_ABORT);

    /* Test that QUIT throws e4__E_QUIT. */
    e4t__ASSERT_EQ(e4__evaluate(task, "quit", -1), e4__E_QUIT);

    /* Test that BYE throws e4__E_BYE. */
    e4t__ASSERT_EQ(e4__evaluate(task, "bye", -1), e4__E_BYE);

    /* Test that it's possible to throw an arbitrary exception using
       the THROW builtin. */
    e4t__ASSERT_EQ(e4__evaluate(task, "1133469 throw", -1), 1133469);

    /* Test that it's possible to catch exceptions other than QUIT and
       BYE using the CATCH builtin. */
    e4t__ASSERT_OK(e4__evaluate(task, "' abort catch", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__USIZE_NEGATE(1));

    e4t__ASSERT_OK(e4__evaluate(task, ":noname 3456 throw ; catch", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3456);

    e4t__ASSERT_EQ(e4__evaluate(task, "' quit catch", -1), e4__E_QUIT);
    e4t__ASSERT_EQ(e4__evaluate(task, "' bye catch", -1), e4__E_BYE);

    /* Test that when nothing goes wrong, e4__E_OK is placed on top
       of the stack. */
    e4t__ASSERT_OK(e4__evaluate(task, ":noname 5 ; catch", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__E_OK);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    /* Test that ABORTQ throws -2 and sets last_abortq appropriately,
       without writing to the output buffer. */
    e4t__ASSERT_OK(e4__evaluate(task, ": foo s\" An abort message.\" ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "31", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "foo abortq", -1), e4__E_ABORTQ);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 31);
    e4t__ASSERT_EQ((len = e4__task_last_abortq(task, &msg)), 17);
    e4t__ASSERT(msg);
    e4t__ASSERT(!e4__mem_strncasecmp("An abort message.", msg, len));
}

/* Covers BIN R/O R/W W/O */
static void e4t__test_builtin_file_constants(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that constants produce expected results. */
    e4t__ASSERT_OK(e4__evaluate(task, "r/o", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__F_READ);
    e4t__ASSERT_OK(e4__evaluate(task, "r/w", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__F_READWRITE);
    e4t__ASSERT_OK(e4__evaluate(task, "w/o", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__F_WRITE);

    /* Test that BIN modifies values as appropriate. */
    e4t__ASSERT_OK(e4__evaluate(task, "r/o bin", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__F_READ | e4__F_BIN);
    e4t__ASSERT_OK(e4__evaluate(task, "r/w bin", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__F_READWRITE | e4__F_BIN);
    e4t__ASSERT_OK(e4__evaluate(task, "w/o bin", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__F_WRITE | e4__F_BIN);
}

/* Covers CLOSE-FILE, INCLUDED, INCLUDE-FILE, OPEN-FILE */
static void e4t__test_builtin_file_include(void)
{
    struct e4__task *task = e4t__transient_task();
    char buf[64] = {0};
    char path[] = "/tmp/e4-XXXXXX";
    int fd;

    /* Test that attempting to open, close, or include a file doesn't
       exist fails as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, "s\" /some/fake/file\" r/o open-file",
            -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), ENOENT);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_OK(e4__evaluate(task, "32576 close-file", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), EBADF);

    e4t__ASSERT_EQ(e4__evaluate(task, "s\" /some/fake/file\" included", -1),
            e4__E_OPEN_FILE);
    e4t__ASSERT_EQ(e4__evaluate(task, "234958 include-file", -1),
            e4__E_CLOSE_FILE);

    /* Create a temporary file and write some data into it. */
    fd = mkstemp(path);
    e4t__ASSERT(fd >= 0);
    e4t__ASSERT_EQ(write(fd, ": foo 3 7 + ;\n", 14), 14);
    e4t__ASSERT_EQ(write(fd, "foo 15", 6), 6);
    e4t__ASSERT_EQ(close(fd), 0);

    /* Attempt to open a file as read only. */
    sprintf(buf, "s\" %s\"", path);
    e4t__ASSERT_OK(e4__evaluate(task, buf, -1));
    e4t__ASSERT_OK(e4__evaluate(task, "r/o open-file", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    /* Attempt to close a file that has been opened. */
    e4t__ASSERT_OK(e4__evaluate(task, "close-file", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    /* Attempt to open and include a file, checking to see that doing so
       has the expected side effects. */
    e4t__ASSERT_OK(e4__evaluate(task, buf, -1));
    e4t__ASSERT_OK(e4__evaluate(task, "r/o open-file", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4__stack_dup(task);
    e4t__ASSERT_OK(e4__evaluate(task, "include-file", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));

    /* Attempt to close a file that has already been closed. */
    e4t__ASSERT_OK(e4__evaluate(task, "close-file", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), EBADF);

    /* Attempt to include an unopened file based on path rather than
       descriptor. */
    e4t__ASSERT_OK(e4__evaluate(task, buf, -1));
    e4t__ASSERT_OK(e4__evaluate(task, "included", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));

    /* Write bad code into the file to be included. */
    fd = open(path, O_WRONLY);
    e4t__ASSERT(fd >= 0);
    e4t__ASSERT(lseek(fd, 0, SEEK_END) >= 0);
    e4t__ASSERT_EQ(write(fd, "\nfoo bar", 8), 8);
    e4t__ASSERT_EQ(close(fd), 0);

    /* Check to see that exceptions that occur within a file are
       percolated up as expected, but that other side effects are still
       present as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, buf, -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "included", -1), e4__E_UNDEFWORD);
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));

    /* Delete the temporary file. */
    e4t__ASSERT_EQ(unlink(path), 0);
}

/* Covers CLOSE-FILE, INCLUDED, INCLUDE-FILE, OPEN-FILE, file
   exceptions, and IO errors in a nested context. */
static void e4t__test_builtin_file_nested(void)
{
    struct e4__task *task = e4t__transient_task();
    struct e4__file_exception fex = { e4__E_BUG, };
    char buf0[64] = {0};
    char buf1[64] = {0};
    char path0[] = "/tmp/e4-foo-XXXXXX";
    char path1[] = "/tmp/e4-bar-XXXXXX";
    int fd0, fd1;

    /* Create two temporary files and write code into one to include the
       other. */
    fd0 = mkstemp(path0);
    e4t__ASSERT(fd0 >= 0);
    fd1 = mkstemp(path1);
    e4t__ASSERT(fd1 >= 0);

    sprintf(buf0, "s\" %s\"", path0);
    sprintf(buf1, "s\" %s\"", path1);

    e4t__ASSERT_EQ(write(fd0, "12345\n", 6), 6);
    e4t__ASSERT_EQ(write(fd0, buf1, strlen(buf1)), strlen(buf1));
    e4t__ASSERT_EQ(write(fd0, " included\n", 10), 10);
    e4t__ASSERT_EQ(write(fd0, "54321", 5), 5);
    e4t__ASSERT_EQ(close(fd0), 0);

    e4t__ASSERT_EQ(write(fd1, "67890\n", 6), 6);
    e4t__ASSERT_EQ(close(fd1), 0);

    /* Evaluate the first file and see that it also evaluates the
       second. */
    e4t__ASSERT_OK(e4__evaluate(task, buf0, -1));
    e4t__ASSERT_OK(e4__evaluate(task, "included", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 54321);
    e4t__ASSERT_EQ(e4__stack_pop(task), 67890);
    e4t__ASSERT_EQ(e4__stack_pop(task), 12345);

    /* Add code to create an IO error in the second file. See that this
       exception is percolated up and that the IO error is set. */
    fd1 = open(path1, O_WRONLY);
    e4t__ASSERT(fd1 >= 0);
    e4t__ASSERT(lseek(fd1, 0, SEEK_END) >= 0);
    e4t__ASSERT_EQ(write(fd1, "s\" /some/fake/file/blah\" included", 33),
            33);
    e4t__ASSERT_EQ(close(fd1), 0);

    e4t__ASSERT_OK(e4__evaluate(task, buf0, -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "included", -1), e4__E_OPEN_FILE);
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 67890);
    e4t__ASSERT_EQ(e4__stack_pop(task), 12345);

    e4t__ASSERT_OK(e4__task_ior(task, 0));
    e4t__ASSERT_EQ(e4__task_fex(task, &fex), e4__E_OPEN_FILE);
    e4t__ASSERT_EQ(fex.ex, e4__E_OPEN_FILE);
    e4t__ASSERT_EQ(fex.ior, ENOENT);
    e4t__ASSERT_EQ(fex.line, 2);
    e4t__ASSERT_EQ(fex.path_sz, strlen(path1));
    e4t__ASSERT(!memcmp(fex.path, path1, strlen(path1)));
    e4t__ASSERT_EQ(fex.buf_sz, 33);
    e4t__ASSERT(!memcmp(fex.buf, "s\" /some/fake/file/blah\" included", 33));

    /* Execute the included file using INCLUDE-FILE instead and see
       that path and path_sz are set correctly in the associated IO
       exception. */
    e4t__ASSERT_EQ(truncate(path0, 0), 0);
    fd1 = open(path0, O_WRONLY);
    e4t__ASSERT(fd0 >= 0);
    e4t__ASSERT_EQ(write(fd0, "12345\n", 6), 6);
    e4t__ASSERT_EQ(write(fd0, buf1, strlen(buf1)), strlen(buf1));
    e4t__ASSERT_EQ(write(fd0, " r/o open-file drop include-file\n", 33), 33);
    e4t__ASSERT_EQ(write(fd0, "54321", 5), 5);
    e4t__ASSERT_EQ(close(fd0), 0);

    e4t__ASSERT_OK(e4__evaluate(task, buf0, -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "included", -1), e4__E_OPEN_FILE);
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 67890);
    e4t__ASSERT_EQ(e4__stack_pop(task), 12345);

    e4t__ASSERT_OK(e4__task_ior(task, 0));
    e4t__ASSERT_EQ(e4__task_fex(task, &fex), e4__E_OPEN_FILE);
    e4t__ASSERT_EQ(fex.ex, e4__E_OPEN_FILE);
    e4t__ASSERT_EQ(fex.ior, ENOENT);
    e4t__ASSERT_EQ(fex.line, 2);
    e4t__ASSERT_EQ(fex.path_sz, 0);
    e4t__ASSERT_EQ(fex.path, NULL);
    e4t__ASSERT_EQ(fex.buf_sz, 33);
    e4t__ASSERT(!memcmp(fex.buf, "s\" /some/fake/file/blah\" included", 33));

    /* Update the second file to recursively include itself and see
       that the appropriate exception is thrown. */
    e4t__ASSERT_EQ(truncate(path1, 0), 0);
    fd1 = open(path1, O_WRONLY);
    e4t__ASSERT(fd1 >= 0);
    e4t__ASSERT_EQ(write(fd1, "67890\n", 6), 6);
    e4t__ASSERT_EQ(write(fd1, buf1, strlen(buf1)), strlen(buf1));
    e4t__ASSERT_EQ(write(fd1, " included\n", 10), 10);
    e4t__ASSERT_EQ(close(fd1), 0);

    e4t__ASSERT_OK(e4__evaluate(task, buf0, -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "included", -1), e4__E_INCFOVERFLOW);
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 67890);
    e4t__ASSERT_EQ(e4__stack_pop(task), 12345);

    e4t__ASSERT_OK(e4__task_ior(task, 0));
    e4t__ASSERT_EQ(e4__task_fex(task, &fex), e4__E_INCFOVERFLOW);
    e4t__ASSERT_EQ(fex.ex, e4__E_INCFOVERFLOW);
    e4t__ASSERT_EQ(fex.ior, 0);
    e4t__ASSERT_EQ(fex.line, 2);
    e4t__ASSERT_EQ(fex.path_sz, strlen(path1));
    e4t__ASSERT(!memcmp(fex.path, path1, strlen(path1)));
    e4t__ASSERT_EQ(fex.buf_sz, 32);
    e4t__ASSERT(!memcmp(fex.buf, "s\" /tmp/e4-bar-", 15));

    /* Delete the temporary files. */
    e4t__ASSERT_EQ(unlink(path0), 0);
    e4t__ASSERT_EQ(unlink(path1), 0);
}

/* Covers FIND */
static void e4t__test_builtin_find(void)
{
    struct e4__task *task = e4t__transient_task();

    e4t__ASSERT_OK(e4__evaluate(task, ": fake-name c\" faake-name\" ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "fake-name dup find", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__stack_pop(task));

    e4t__ASSERT_OK(e4__evaluate(task, ": dup-name c\" dup\" ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "dup-name find", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_EQ(e4__stack_pop(task), &e4__BUILTIN_XT[e4__B_DUP]);

    e4t__ASSERT_OK(e4__evaluate(task, ": begin-name c\" begin\" ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "begin-name find", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), &e4__BUILTIN_XT[e4__B_BEGIN]);
}

/* Covers FORGET, MARKER and look-ahead idiom (which uses builtin
   WORD) */
static void e4t__test_builtin_forget(void)
{
    struct e4__task *task = e4t__transient_task();
    e4__cell here;

    /* Test that FORGET fails on zero length names. */
    e4t__ASSERT_EQ(e4__evaluate(task, "forget", -1), e4__E_ZLNAME);
    e4t__ASSERT_EQ(e4__evaluate(task, "forget   ", -1), e4__E_ZLNAME);

    /* Test that FORGET fails on invalid words and builtins. */
    e4t__ASSERT_EQ(e4__evaluate(task, "forget notaword", -1),
            e4__E_INVFORGET);
    e4t__ASSERT_EQ(e4__evaluate(task, "forget dup", -1), e4__E_INVFORGET);

    e4__dict_entry(task, "foo", 3, 0, NULL, NULL);
    e4__dict_entry(task, "bar", 3, 0, NULL, NULL);

    e4t__ASSERT(e4__dict_lookup(task, "foo", 3));
    e4t__ASSERT(e4__dict_lookup(task, "bar", 3));

    /* Test actually forgetting a word. */
    e4t__ASSERT_OK(e4__evaluate(task, "forget bar", -1));
    e4t__ASSERT_EQ(e4__dict_lookup(task, "bar", 3), NULL);

    /* Test that contents of buffer after look-ahead are intact
       and executed as appropriate. */
    e4t__ASSERT_OK(e4__evaluate(task, "forget    foo 17 dup", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 17);
    e4t__ASSERT_EQ(e4__stack_pop(task), 17);

    /* Test that creating and executing a marker restores HERE. */
    here = e4__task_uservar(task, e4__UV_HERE);
    e4t__ASSERT_OK(e4__evaluate(task, "marker bar", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "20 cells allot", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": quux 1 1 + ;", -1));
    e4t__ASSERT(e4__task_uservar(task, e4__UV_HERE) != here);
    e4t__ASSERT_OK(e4__evaluate(task, "bar", -1));
    e4t__ASSERT_EQ(e4__task_uservar(task, e4__UV_HERE), here);
}

/* Covers [ELSE] [IF] [THEN] */
static void e4t__test_builtin_immed_cond(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that [IF]/[THEN] works while evaluating. */
    e4t__ASSERT_OK(e4__evaluate(task, "1 [if] 5 [then]", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
    e4t__ASSERT_OK(e4__evaluate(task, "0 [if] 5 [then]", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    /* Test that whatever is between [IF]/[THEN] need not be valid
       syntax when [IF]'s condition is false. */
    e4t__ASSERT_OK(e4__evaluate(task, "0 [if] just some junk [then]", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "1 [if] just some junk [then]", -1),
            e4__E_UNDEFWORD);

    /* Test that [IF]/[THEN] may span multiple input lines. */
    e4t__ASSERT_OK(e4__evaluate(task, "0 [if] just some junk\n"
            "even more junk\nand finally done [then] 5", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    /* Test that nested [IF]/[THEN] works as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, "1 [if] 5 1 [if] 10 [then] 15 [then]",
            -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    e4t__ASSERT_OK(e4__evaluate(task, "1 [if] 5 0 [if] 10 [then] 15 [then]",
            -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    e4t__ASSERT_OK(e4__evaluate(task, "0 [if] 5 0 [if] 10 [then] 15 [then]",
            -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    /* Test that [IF]/[ELSE]/[THEN] works while evaluating. */
    e4t__ASSERT_OK(e4__evaluate(task, "1 [if] 5 [else] 7 [then]", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
    e4t__ASSERT_OK(e4__evaluate(task, "0 [if] 5 [else] 7 [then]", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 7);

    /* Test that whatever is between [ELSE]/[THEN] need not be valid
       syntax when the initial [IF]'s condition was true. */
    e4t__ASSERT_OK(e4__evaluate(task, "1 [if] [else] just some junk [then]",
            -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "0 [if] [else] just some junk [then]",
            -1), e4__E_UNDEFWORD);

    /* Test that [IF]/[ELSE]/[THEN] may span multiple input lines. */
    e4t__ASSERT_OK(e4__evaluate(task, "1 [if] 10 [else] just some junk\n"
            "even more junk\nand finally done [then] 5", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);

    /* Test that nested [IF]/[ELSE]/[THEN] works as expected. */
    e4t__ASSERT_OK(e4__evaluate(task,
            "1 [if] 5 1 [if] 10 [else] 17 [then] 15 [else] 7 [then]", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    e4t__ASSERT_OK(e4__evaluate(task,
            "1 [if] 5 0 [if] 10 [else] 17 [then] 15 [else] 7 [then]", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 3);
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);
    e4t__ASSERT_EQ(e4__stack_pop(task), 17);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    e4t__ASSERT_OK(e4__evaluate(task,
            "0 [if] 5 1 [if] 10 [else] 17 [then] 15 [else] 7 [then]", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 7);
}

/* Covers ['] TO and ancillary compilation words */
static void e4t__test_builtin_immediate(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that ['] behaves as expected. */
    e4t__ASSERT_EQ(e4__evaluate(task, "['] dup", -1), e4__E_COMPONLYWORD);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo ['] dup ; foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), &e4__BUILTIN_XT[e4__B_DUP]);

    /* Test that the compilation semantics of TO are correct. */
    e4t__ASSERT_OK(e4__evaluate(task, "35 value foo foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 35);
    e4t__ASSERT_OK(e4__evaluate(task, ": set-foo to foo ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "55 set-foo foo", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 55);
}

/* Covers . ? ." .( CR BASE COUNT EMIT PAD SPACE SPACES TYPE U. WORDS */
static void e4t__test_builtin_io(void)
{
    /* XXX: Parts of this test only work correctly on a 64 bit
       system that represents negative numbers using two's
       complement. */

    struct e4__task *task = e4t__transient_task();

    e4t__term_obuf_consume();

    /* Test that basic with . works . */
    e4t__ASSERT_OK(e4__evaluate(task, "17 .", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "17 ");
    e4t__ASSERT_OK(e4__evaluate(task, "-17 .", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-17 ");

    /* Test that . respects base. */
    e4t__ASSERT_OK(e4__evaluate(task, "17 10 17 0x10 base ! . base ! .", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "11 17 ");

    /* Test basic output with ?. */
    e4t__ASSERT_OK(e4__evaluate(task, "123 pad ! pad ?", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "123 ");
    e4t__ASSERT_OK(e4__evaluate(task, "456 pad ! pad ?", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "456 ");

    /* Test that ? respects base. */
    e4t__ASSERT_OK(e4__evaluate(task,
                "10 17 pad ! 0x10 base ! pad ? base ! pad ?", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "11 17 ");

    /* Test that CR works correctly. */
    e4t__ASSERT_OK(e4__evaluate(task, "cr", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "\n");
    e4t__ASSERT_OK(e4__evaluate(task, "cr cr", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "\n\n");

    /* Test that U. behaves as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, "-1 u.", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "18446744073709551615 ");
    e4t__ASSERT_OK(e4__evaluate(task, "-25 u.", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "18446744073709551591 ");

    /* Test that EMIT behaves as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, "'0' 'o' 'f' emit emit emit", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "fo0");

    /* Test that newly created words appear at the front of WORDS. */
    e4t__ASSERT_OK(e4__evaluate(task, ": flom345 ; : blom938 ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "words", -1), 0);
    e4t__ASSERT(!e4__mem_strncasecmp(e4t__term_obuf_consume(),
                "blom938 flom345 ", 16));

    /* Test DECIMAL and HEX convenience words. */
    e4t__ASSERT_OK(e4__evaluate(task, "hex base @ decimal base @", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_EQ(e4__stack_pop(task), 16);

    /* Test COUNT and TYPE. */
    e4__stack_push(task, (e4__cell)"hack the planet");
    e4t__ASSERT_OK(e4__evaluate(task, "15 type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "hack the planet");

    e4__stack_push(task, (e4__cell)"\031I know you play the game.");
    e4t__ASSERT_OK(e4__evaluate(task, "count 2dup type", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 25);
    e4t__ASSERT_EQ(*((char *)e4__stack_pop(task)), 'I');
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "I know you play the game.");

    /* Test ." and .( runtime semeantics. */
    e4t__ASSERT_OK(e4__evaluate(task, ".\" What a nice message!\"", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "What a nice message!");
    e4t__ASSERT_OK(e4__evaluate(task, ".( Wow, even more!?)", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "Wow, even more!?");
    e4t__ASSERT_OK(e4__evaluate(task,
            ": foo .( Right out of the gate!) 2 2 + ;", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "Right out of the gate!");

    /* Test that SPACE and SPACES work as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, "space", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), " ");
    e4t__ASSERT_OK(e4__evaluate(task, "5 spaces", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "     ");
}

/* Covers ACCEPT */
static void e4t__test_builtin_io_accept(void)
{
    struct e4__task *task = e4t__transient_task();

    /* XXX: The kernel API behind ACCEPT is covered in term.c. This test
       serves mainly to verify that the builtin itself invokes this API
       correctly. */

    e4t__ASSERT_OK(e4__evaluate(task, "create buf 80 chars allot", -1));
    e4t__term_ibuf_feed("okay here we go", -1);
    e4t__ASSERT_OK(e4__evaluate(task, "buf dup 80 accept", -1));

    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);

    e4t__ASSERT(!memcmp(e4__stack_pop(task), "okay here we go", 15));
}

/* Covers DUMP */
static void e4t__test_builtin_io_dump(void)
{
    /* XXX: Parts of this test only work correctly on a 64 bit system
       where unsigned long values have 8 bit alignment. */

    static const char *test_str =
            "test \x88\x99\xaa string with some \x01 \x02 \x03 data";

    char expected[240] = {0,};
    unsigned long buffer[8];
    struct e4__task *task = e4t__transient_task();
    char *unaligned_buf = ((char *)buffer) + 3;
    e4__usize len = strlen(test_str);

    /* XXX: It would violate strict aliasing rules if we ever looked at
       buffer again after modifying it using unaligned_buf. So long as
       we don't do that, everything should be fine. */
    memcpy(unaligned_buf, test_str, len);

    e4t__term_obuf_consume();

    e4__stack_push(task, (e4__cell)unaligned_buf);
    e4__stack_push(task, (e4__cell)len);
    e4t__ASSERT_OK(e4__evaluate(task, "dump", -1));

    #define _f  "%016lx   "
    sprintf(expected,
            _f "???? ??74 6573 7420  8899 aa20 7374 7269   ...test ... stri\n"
            _f "6e67 2077 6974 6820  736f 6d65 2001 2002   ng with some . .\n"
            _f "2003 2064 6174 61??  ???? ???? ???? ????    . data.........\n",
            (unsigned long)&buffer[0],
            (unsigned long)&buffer[2],
            (unsigned long)&buffer[4]);
    #undef _f

    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), expected);
}

/* Covers . .( ." .S CR DUMP EMIT KEY TYPE U. WORDS */
static void e4t__test_builtin_io_error(void)
{
    struct e4__task *task = e4t__transient_task();
    struct e4__io_func io_func = {0,};

    /* Clear all test related IO handlers. */
    e4__task_io_init(task, &io_func);

    /* Test that all words that can raise IO errors do so when
       no IO handler is available. */
    e4t__ASSERT_EQ(e4__evaluate(task, "1337 .", -1), e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__evaluate(task, ".( Hello, World!)", -1),
            e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__evaluate(task, ".\" Hello again, World!\"", -1),
            e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__evaluate(task, ".s", -1), e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__evaluate(task, "cr", -1), e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__evaluate(task, "here 5 cells dump", -1),
            e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__evaluate(task, "51 emit", -1), e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__evaluate(task, "key", -1), e4__E_UNSUPPORTED);
    e4__stack_push(task, (e4__cell)"Just some junk.");
    e4__stack_push(task, (e4__cell)10);
    e4t__ASSERT_EQ(e4__evaluate(task, "type", -1), e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__evaluate(task, "5 u.", -1), e4__E_UNSUPPORTED);
    e4t__ASSERT_EQ(e4__evaluate(task, "words", -1), e4__E_UNSUPPORTED);
}

/* Covers AT-XY KEY? MS PAGE */
static void e4t__test_builtin_io_facility(void)
{
    struct e4__task *task = e4t__transient_task();

    e4t__term_obuf_consume();

    e4t__ASSERT_OK(e4__evaluate(task, "page", -1));
    e4t__ASSERT(!strcmp(e4t__term_obuf_consume(), "\033[2J\033[0;0H"));

    e4t__ASSERT_OK(e4__evaluate(task, "0 0 at-xy ", -1));
    e4t__ASSERT(!strcmp(e4t__term_obuf_consume(), "\033[0;0H"));

    e4t__ASSERT_OK(e4__evaluate(task, "30 15 at-xy ", -1));
    e4t__ASSERT(!strcmp(e4t__term_obuf_consume(), "\033[15;30H"));

    e4t__ASSERT_OK(e4__evaluate(task, "15 30 at-xy ", -1));
    e4t__ASSERT(!strcmp(e4t__term_obuf_consume(), "\033[30;15H"));

    e4t__ASSERT_OK(e4__evaluate(task, "key?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);

    e4t__term_ibuf_feed("foo", -1);

    e4t__ASSERT_OK(e4__evaluate(task, "key?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "key", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 'f');

    e4t__ASSERT_OK(e4__evaluate(task, "key?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "key", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 'o');

    e4t__ASSERT_OK(e4__evaluate(task, "key?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "key", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 'o');

    e4t__ASSERT_OK(e4__evaluate(task, "key?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);

    e4t__ASSERT_OK(e4__evaluate(task, "39145 ms", -1));
    e4t__ASSERT_EQ(e4t__term_lastms_get(), 39145);
}

/* Covers <# #> # #S HOLD HOLDS SIGN */
static void e4t__test_builtin_io_pno(void)
{
    struct e4__task *task = e4t__transient_task();

    e4t__term_obuf_consume();

    /* Test that <# #> produces an empty string. */
    e4t__ASSERT_OK(e4__evaluate(task, "0 0 <# #>", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4__stack_pop(task);

    /* Test that <# # #> formats the least significant digit and leaves
       the remaining digits in the double on the top of the stack. */
    e4t__ASSERT_OK(e4__evaluate(task, "539 0 <# #", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_EQ(e4__stack_pop(task), 53);
    e4__stack_push(task, (e4__cell)0);
    e4__stack_push(task, (e4__cell)0);
    e4t__ASSERT_OK(e4__evaluate(task, "#> type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "9");

    /* Test that #S formats all remaining digits and leaves 0 0 on top
       of the stack. */
    e4t__ASSERT_OK(e4__evaluate(task, "539 0 <# #", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_EQ(e4__stack_pop(task), 53);
    e4__stack_push(task, (e4__cell)53);
    e4__stack_push(task, (e4__cell)0);
    e4t__ASSERT_OK(e4__evaluate(task, "#s", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4__stack_push(task, (e4__cell)0);
    e4__stack_push(task, (e4__cell)0);
    e4t__ASSERT_OK(e4__evaluate(task, "#> type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "539");

    /* Test that formatting respects base as expected. */
    e4t__ASSERT_OK(e4__evaluate(task,
            "2 base ! #539 0 <# #s #> decimal type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1000011011");

    /* Test that it is possible to format numbers that actually use
       two cells. */
    e4t__ASSERT_OK(e4__evaluate(task, "-1 31934 um* <# #s #> type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "589078325249840821273410");

    /* Test that HOLD and HOLDS work as expected in isolation. */
    e4t__ASSERT_OK(e4__evaluate(task,
            "0 0 <# 'o' hold 'o' hold 'f' hold #> type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "foo");
    e4t__ASSERT_OK(e4__evaluate(task, ": price s\" Price: $\" ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "0 0 <# price holds #> type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "Price: $");

    /* Test that SIGN works as expected in isolation. */
    e4t__ASSERT_OK(e4__evaluate(task, "0 0 <# 0 sign #> type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "");
    e4t__ASSERT_OK(e4__evaluate(task, "0 0 <# -1 sign #> type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-");

    /* Test that SIGN can be used to format signed numbers. */
    e4t__ASSERT_OK(e4__evaluate(task,
            ": dabs dup 0< if "
                "invert swap invert 1+ dup 0= if "
                    "swap 1+ swap "
                "then swap "
            "then ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task,
            ": #n dup >r dabs #s r> sign ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "-539 -1 <# #n #> type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-539");
    e4t__ASSERT_OK(e4__evaluate(task, "539 0 <# #n #> type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "539");
    e4t__ASSERT_OK(e4__evaluate(task, "-53 -79 <# #n #> type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-1438846037749345026101");
    e4t__ASSERT_OK(e4__evaluate(task, "53 78 <# #n #> type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1438846037749345026101");

    /* Test combining several PNO words. */
    e4t__ASSERT_OK(e4__evaluate(task,
            ": price. s>d <# # # '.' hold #s price holds #> type ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "1995 price.", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "Price: $19.95");
    e4t__ASSERT_OK(e4__evaluate(task, "325819 price.", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "Price: $3258.19");
    e4t__ASSERT_OK(e4__evaluate(task, "2 price.", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "Price: $0.02");
}

/* Covers = < > <> 0< 0> 0<> 0= AND INVERT NEGATE OR U< U> XOR */
static void e4t__test_builtin_logic(void)
{
    /* XXX: Parts of this test only work correctly on a 64 bit
       system that represents negative numbers using two's
       complement. */

    struct e4__task *task = e4t__transient_task();

    /* Test basic equality words. */
    e4t__ASSERT_OK(e4__evaluate(task, "5 5 =", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "5 4 =", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "5 5 <>", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "5 4 <>", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);

    /* Test negation related words. */
    e4t__ASSERT_OK(e4__evaluate(task, "5 negate", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), -5);
    e4t__ASSERT_OK(e4__evaluate(task, "-5 negate", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
    e4t__ASSERT_OK(e4__evaluate(task, "0 negate", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    /* Test zero comparison related words. */
    e4t__ASSERT_OK(e4__evaluate(task, "-153 0<", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "135 0<", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "0 0<", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "-153 0>", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "135 0>", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "0 0>", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);

    e4t__ASSERT_OK(e4__evaluate(task, "0 0=", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "50 0=", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "0 0<>", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "5 0<>", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);

    /* Test bitwise operators. */
    e4t__ASSERT_OK(e4__evaluate(task, "12 3 or", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 15);
    e4t__ASSERT_OK(e4__evaluate(task, "7 3 or", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 7);
    e4t__ASSERT_OK(e4__evaluate(task, "8 7 and", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_OK(e4__evaluate(task, "10394 15 and", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_OK(e4__evaluate(task, "15 5 xor", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_OK(e4__evaluate(task, "301415 1234 xor", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 302517);
    e4t__ASSERT_OK(e4__evaluate(task, "0 invert", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_OK(e4__evaluate(task, "0 invert", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_OK(e4__evaluate(task, "5 invert", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), -6);
    e4t__ASSERT_OK(e4__evaluate(task, "-7 invert", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 6);

    /* Test signed comparison. */
    e4t__ASSERT_OK(e4__evaluate(task, "0 0 <", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "0 1 <", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "1 0 <", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "-1 0 <", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "1 -1 <", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "50 40 <", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "-50 -40 <", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "9223372036854775807 1 <", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "1 9223372036854775807 <", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);

    e4t__ASSERT_OK(e4__evaluate(task, "0 0 >", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "0 1 >", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "1 0 >", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "-1 0 >", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "1 -1 >", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "50 40 >", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "-50 -40 >", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "-9223372036854775808 1 >", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "1 -9223372036854775808 >", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);

    /* Test unsigned comparison. */
    e4t__ASSERT_OK(e4__evaluate(task, "0 0 u<", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "0 1 u<", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "1 0 u<", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "-1 0 u<", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "1 -1 u<", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);

    e4t__ASSERT_OK(e4__evaluate(task, "0 0 u>", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "0 1 u>", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
    e4t__ASSERT_OK(e4__evaluate(task, "1 0 u>", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "-1 0 u>", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_OK(e4__evaluate(task, "1 -1 u>", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_FALSE);
}

/* Covers LOOP_INC */
static void e4t__test_builtin_loop_inc(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that LOOP_INC correctly reports new index and when this new
       index has crossed the boundary between the loop limit minus one
       and the loop limit as described in section 6.1.0140 of the Forth
       2012 standard. */
    e4t__term_obuf_consume();

    e4t__ASSERT_OK(e4__evaluate(task, "1 9 10 LOOP_INC .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<2> -1 10 ");
    e4t__ASSERT_OK(e4__evaluate(task, "5 9 10 LOOP_INC .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<2> -1 14 ");
    e4t__ASSERT_OK(e4__evaluate(task, "1 10 10 LOOP_INC .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<2> 0 11 ");
    e4t__ASSERT_OK(e4__evaluate(task, "1 12 10 LOOP_INC .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<2> 0 13 ");
    e4t__ASSERT_OK(e4__evaluate(task, "-1 -10 -10 LOOP_INC .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<2> -1 -11 ");
    e4t__ASSERT_OK(e4__evaluate(task, "-1 -11 -10 LOOP_INC .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<2> 0 -12 ");
    e4t__ASSERT_OK(e4__evaluate(task, "-1 -9 -10 LOOP_INC .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<2> 0 -10 ");
    e4t__ASSERT_OK(e4__evaluate(task, "-3 -9 -10 LOOP_INC .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<2> -1 -12 ");
}

/* Covers + - . / 1+ 1- 2* 2/ /MOD ABS LSHIFT MAX MIN MOD RSHIFT U. */
static void e4t__test_builtin_math(void)
{
    struct e4__task *task = e4t__transient_task();

    /* XXX: Parts of this test only work correctly on a 64 bit
       system with symmetric (rather than floored) division. */

    #define _e(s)   e4__evaluate(task, s, sizeof(s) - 1)

    e4t__term_obuf_consume();

    e4t__ASSERT_EQ(_e("+"), e4__E_STKUNDERFLOW);
    e4t__ASSERT_EQ(_e("1 +"), e4__E_STKUNDERFLOW);
    e4t__ASSERT_EQ(_e("-"), e4__E_STKUNDERFLOW);
    e4t__ASSERT_EQ(_e("1 -"), e4__E_STKUNDERFLOW);

    e4t__ASSERT_OK(_e("0  5 + ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "5 ");
    e4t__ASSERT_OK(_e("5  0 + ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "5 ");
    e4t__ASSERT_OK(_e("0 -5 + ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-5 ");
    e4t__ASSERT_OK(_e("-5  0 + ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-5 ");
    e4t__ASSERT_OK(_e("1  2 + ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "3 ");
    e4t__ASSERT_OK(_e("1 -2 + ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-1 ");
    e4t__ASSERT_OK(_e("-1  2 + ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1 ");
    e4t__ASSERT_OK(_e("-1 -2 + ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-3 ");
    e4t__ASSERT_OK(_e("-1  1 + ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");

    e4t__ASSERT_OK(_e("0  5 - . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-5 ");
    e4t__ASSERT_OK(_e("5  0 - . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "5 ");
    e4t__ASSERT_OK(_e("0 -5 - . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "5 ");
    e4t__ASSERT_OK(_e("-5  0 - . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-5 ");
    e4t__ASSERT_OK(_e("1  2 - . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-1 ");
    e4t__ASSERT_OK(_e("1 -2 - . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "3 ");
    e4t__ASSERT_OK(_e("-1  2 - . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-3 ");
    e4t__ASSERT_OK(_e("-1 -2 - . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1 ");
    e4t__ASSERT_OK(_e("0  1 - . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-1 ");

    /* Test basic multiplication functionality. */
    e4t__ASSERT_OK(_e("0 0 * . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("0 1 * . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("1 0 * . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("1 2 * . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "2 ");
    e4t__ASSERT_OK(_e("2 1 * . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "2 ");
    e4t__ASSERT_OK(_e("3 3 * . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "9 ");
    e4t__ASSERT_OK(_e("-3 3 * . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-9 ");
    e4t__ASSERT_OK(_e("3 -3 * . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-9 ");
    e4t__ASSERT_OK(_e("-3 -3 * . "));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "9 ");

    /* Test that there is no overflow when multiplying two signed or
       unsigned numbers requiring less than or equal to 64 bits to
       represent. */
    e4t__ASSERT_OK(_e("0xffffffff 0xffffffff * u."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "18446744065119617025 ");
    e4t__ASSERT_OK(_e("4611686018427387904 -2 * ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-9223372036854775808 ");
    e4t__ASSERT_OK(_e("-4611686018427387904 2 * ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-9223372036854775808 ");
    e4t__ASSERT_OK(_e("-4611686018427387903 -2 * ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "9223372036854775806 ");

    /* Test division and modulo words. */
    e4t__ASSERT_OK(_e("0 1 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("1 1 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1 ");
    e4t__ASSERT_OK(_e("2 1 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "2 ");
    e4t__ASSERT_OK(_e("-1 1 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-1 ");
    e4t__ASSERT_OK(_e("-2 1 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-2 ");
    e4t__ASSERT_OK(_e("0 -1 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("1 -1 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-1 ");
    e4t__ASSERT_OK(_e("2 -1 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-2 ");
    e4t__ASSERT_OK(_e("-1 -1 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1 ");
    e4t__ASSERT_OK(_e("-2 -1 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "2 ");
    e4t__ASSERT_OK(_e("2 2 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1 ");
    e4t__ASSERT_OK(_e("-1 -1 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1 ");
    e4t__ASSERT_OK(_e("-2 -2 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1 ");
    e4t__ASSERT_OK(_e("7 3 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "2 ");
    e4t__ASSERT_OK(_e("7 -3 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-2 ");
    e4t__ASSERT_OK(_e("-7 3 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-2 ");
    e4t__ASSERT_OK(_e("-7 -3 / ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "2 ");

    e4t__ASSERT_OK(_e("0 1 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("1 1 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("2 1 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("-1 1 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("-2 1 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("0 -1 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("1 -1 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("2 -1 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("-1 -1 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("-2 -1 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("2 2 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("-1 -1 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("-2 -2 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 ");
    e4t__ASSERT_OK(_e("7 3 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1 ");
    e4t__ASSERT_OK(_e("7 -3 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1 ");
    e4t__ASSERT_OK(_e("-7 3 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-1 ");
    e4t__ASSERT_OK(_e("-7 -3 mod ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-1 ");

    e4t__ASSERT_OK(_e("0 1 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 0 ");
    e4t__ASSERT_OK(_e("1 1 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 1 ");
    e4t__ASSERT_OK(_e("2 1 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 2 ");
    e4t__ASSERT_OK(_e("-1 1 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 -1 ");
    e4t__ASSERT_OK(_e("-2 1 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 -2 ");
    e4t__ASSERT_OK(_e("0 -1 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 0 ");
    e4t__ASSERT_OK(_e("1 -1 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 -1 ");
    e4t__ASSERT_OK(_e("2 -1 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 -2 ");
    e4t__ASSERT_OK(_e("-1 -1 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 1 ");
    e4t__ASSERT_OK(_e("-2 -1 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 2 ");
    e4t__ASSERT_OK(_e("2 2 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 1 ");
    e4t__ASSERT_OK(_e("-1 -1 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 1 ");
    e4t__ASSERT_OK(_e("-2 -2 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "0 1 ");
    e4t__ASSERT_OK(_e("7 3 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1 2 ");
    e4t__ASSERT_OK(_e("7 -3 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "1 -2 ");
    e4t__ASSERT_OK(_e("-7 3 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-1 -2 ");
    e4t__ASSERT_OK(_e("-7 -3 /mod swap . ."));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "-1 2 ");

    /* Test that division by zero throws the expected exception. */
    e4t__ASSERT_EQ(_e("10 0 /"), e4__E_DIVBYZERO);
    e4t__ASSERT_EQ(_e("10 0 mod"), e4__E_DIVBYZERO);
    e4t__ASSERT_EQ(_e("10 0 /mod"), e4__E_DIVBYZERO);

    /* Test that shift operators work as expected. */
    e4t__ASSERT_OK(_e("1 5 lshift"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 32);
    e4t__ASSERT_OK(_e("5 0 lshift"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
    e4t__ASSERT_OK(_e("234 4 rshift"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 14);
    e4t__ASSERT_OK(_e("16 2 rshift"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 4);

    e4t__ASSERT_OK(_e("15 2/"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 7);
    e4t__ASSERT_OK(_e("7 2/"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 3);
    e4t__ASSERT_OK(_e("3 2*"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 6);
    e4t__ASSERT_OK(_e("6 2*"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 12);

    e4t__ASSERT_OK(_e("-15 2/"));
    e4t__ASSERT_EQ(e4__stack_pop(task), -8);
    e4t__ASSERT_OK(_e("-7 2/"));
    e4t__ASSERT_EQ(e4__stack_pop(task), -4);
    e4t__ASSERT_OK(_e("-3 2*"));
    e4t__ASSERT_EQ(e4__stack_pop(task), -6);
    e4t__ASSERT_OK(_e("-6 2*"));
    e4t__ASSERT_EQ(e4__stack_pop(task), -12);

    e4t__ASSERT_OK(_e("-1 2/"));
    e4t__ASSERT_EQ(e4__stack_pop(task), -1);
    e4t__ASSERT_OK(_e("-1 1 rshift 1+ dup 2* ="));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);

    /* Test that increment and decrement words work correctly. */
    e4t__ASSERT_OK(_e("5 1+"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 6);
    e4t__ASSERT_OK(_e("6 1+"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 7);
    e4t__ASSERT_OK(_e("-50 1-"));
    e4t__ASSERT_EQ(e4__stack_pop(task), -51);
    e4t__ASSERT_OK(_e("-51 1-"));
    e4t__ASSERT_EQ(e4__stack_pop(task), -52);

    /* Test that MIN and MAX work as expected. */
    e4t__ASSERT_OK(_e("-5 0 MAX"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_OK(_e("5 0 MAX"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
    e4t__ASSERT_OK(_e("5 50 MAX"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 50);
    e4t__ASSERT_OK(_e("-5 0 MIN"));
    e4t__ASSERT_EQ(e4__stack_pop(task), -5);
    e4t__ASSERT_OK(_e("5 0 MIN"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_OK(_e("5 50 MIN"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    /* Test that ABS works as expected. */
    e4t__ASSERT_OK(_e("-234 abs"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 234);
    e4t__ASSERT_OK(_e("234 abs"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 234);
    e4t__ASSERT_OK(_e("0 abs"));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    #undef _e
}

/* Covers @ ! +! 2@ 2! ALIGN ALIGNED ALLOT BUFFER: C, C@ C! CELLS CREATE
   ERASE FILL HERE MOVE */
static void e4t__test_builtin_memmanip(void)
{
    e4__usize slot, here0, here1;
    char *mem;
    struct e4__task *task = e4t__transient_task();

    /* XXX: Parts of this test only work correctly on a little endian
       64 bit system. */

    /* Test that CHARS and CELLS return the appropriate counts. */
    e4t__ASSERT_OK(e4__evaluate(task, "51 cells", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 51 * sizeof(e4__cell));
    e4t__ASSERT_OK(e4__evaluate(task, "51 chars", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 51 * sizeof(e4__u8));

    /* Test that char and cell advancing pointers work as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, "8 cell+", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 8 + sizeof(e4__cell));
    e4t__ASSERT_OK(e4__evaluate(task, "8 char+", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 8 + sizeof(e4__u8));

    /* Test ! and @ work correctly. */
    e4__stack_push(task, (e4__cell)&slot);
    e4t__ASSERT_OK(e4__evaluate(task, "dup 4739 swap !", -1));
    e4t__ASSERT_EQ(slot, 4739);
    slot = 5193;
    e4t__ASSERT_OK(e4__evaluate(task, "@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 5193);

    /* Test that CELLS returns the appropriate size. */
    e4t__ASSERT_OK(e4__evaluate(task, "1 cells 5 cells -10 cells", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), -10 * sizeof(e4__cell));
    e4t__ASSERT_EQ(e4__stack_pop(task), 5 * sizeof(e4__cell));
    e4t__ASSERT_EQ(e4__stack_pop(task), sizeof(e4__cell));

    /* Test that ALLOT and HERE interact as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, "here 1 cells allot here", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task),
            (e4__usize)e4__stack_pop(task) + sizeof(e4__cell));
    e4t__ASSERT_OK(e4__evaluate(task, "here -1 cells allot here", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task),
            (e4__usize)e4__stack_pop(task) - sizeof(e4__cell));
    e4t__ASSERT_OK(e4__evaluate(task, "here 7 allot here", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), (e4__usize)e4__stack_pop(task) + 7);
    e4t__ASSERT_OK(e4__evaluate(task, "here -7 allot here", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), (e4__usize)e4__stack_pop(task) - 7);

    /* Test that alignment builtins work as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, "0 aligned", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_OK(e4__evaluate(task, "1 aligned", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 8);
    e4t__ASSERT_OK(e4__evaluate(task, "2 aligned", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 8);
    e4t__ASSERT_OK(e4__evaluate(task, "3 aligned", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 8);
    e4t__ASSERT_OK(e4__evaluate(task, "4 aligned", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 8);
    e4t__ASSERT_OK(e4__evaluate(task, "5 aligned", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 8);
    e4t__ASSERT_OK(e4__evaluate(task, "6 aligned", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 8);
    e4t__ASSERT_OK(e4__evaluate(task, "7 aligned", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 8);
    e4t__ASSERT_OK(e4__evaluate(task, "8 aligned", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 8);
    e4t__ASSERT_OK(e4__evaluate(task, "9 aligned", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 16);

    e4t__ASSERT_OK(e4__evaluate(task, "here", -1));
    e4t__ASSERT(!((e4__usize)e4__stack_pop(task) % sizeof(e4__cell)));
    e4t__ASSERT_OK(e4__evaluate(task, "3 allot here", -1));
    here0 = (e4__usize)e4__stack_pop(task);
    e4t__ASSERT(here0 % sizeof(e4__cell));
    e4t__ASSERT_OK(e4__evaluate(task, "align here", -1));
    here1 = (e4__usize)e4__stack_pop(task);
    e4t__ASSERT(!(here1 % sizeof(e4__cell)));
    e4t__ASSERT(here1 > here0);
    here0 = here1;

    e4t__ASSERT_OK(e4__evaluate(task, "align here", -1));
    here1 = (e4__usize)e4__stack_pop(task);
    e4t__ASSERT(!(here1 % sizeof(e4__cell)));
    e4t__ASSERT(here1 == here0);

    /* Test basic character/byte allocation and access. */
    e4t__ASSERT_OK(e4__evaluate(task,
            "create foo here 'h' c, 'e' c, 'l' c, 'l' c, 'o' c, align here",
            -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__stack_pop(task) + 1);
    e4t__ASSERT_OK(e4__evaluate(task, "foo 0 chars + c@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 'h');
    e4t__ASSERT_OK(e4__evaluate(task, "foo 1 chars + c@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 'e');
    e4t__ASSERT_OK(e4__evaluate(task, "foo 2 chars + c@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 'l');
    e4t__ASSERT_OK(e4__evaluate(task, "foo 3 chars + c@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 'l');
    e4t__ASSERT_OK(e4__evaluate(task, "foo 4 chars + c@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 'o');

    /* Test that bytes are cleared when a cell is written over them. */
    e4t__ASSERT_OK(e4__evaluate(task, "0 foo !", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo 0 chars + c@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_OK(e4__evaluate(task, "foo 1 chars + c@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_OK(e4__evaluate(task, "foo 2 chars + c@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_OK(e4__evaluate(task, "foo 3 chars + c@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_OK(e4__evaluate(task, "foo 4 chars + c@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    /* Test that setting bytes functions correctly. */
    e4t__ASSERT_OK(e4__evaluate(task, "0x20 foo 0 chars + c!", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "0x40 foo 2 chars + c!", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "0x60 foo 4 chars + c!", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo 0 chars + c@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0x20);
    e4t__ASSERT_OK(e4__evaluate(task, "foo 2 chars + c@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0x40);
    e4t__ASSERT_OK(e4__evaluate(task, "foo 4 chars + c@", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0x60);
    e4t__ASSERT_OK(e4__evaluate(task, "foo @", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0x6000400020);

    /* Test that 2@, 2!, and !+ work as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, "create foo 10 , 20 ,", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo 2@", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_EQ(e4__stack_pop(task), 20);

    e4t__ASSERT_OK(e4__evaluate(task, "50 100 foo 2!", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo @ foo cell+ @", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 50);
    e4t__ASSERT_EQ(e4__stack_pop(task), 100);

    e4t__ASSERT_OK(e4__evaluate(task, "25 foo !", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "225 foo +!", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo @", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 250);

    /* Test ERASE and FILL. */
    #define _c(m)   \
        do {    \
            int i;  \
            for (i = 0; i < 10; ++i)    \
                m[i] = i;   \
        } while (0)

    e4t__ASSERT_OK(e4__evaluate(task, "create foo 10 chars allot foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    mem = (char *)e4__stack_pop(task);
    _c(mem);
    e4t__ASSERT_OK(e4__evaluate(task, "foo 5 chars erase", -1));
    e4t__ASSERT(!memcmp(mem, "\0\0\0\0\0\x05\x06\x07\x08\x09", 10));
    e4t__ASSERT_OK(e4__evaluate(task, "foo 3 chars 'f' fill", -1));
    e4t__ASSERT(!memcmp(mem, "fff\0\0\x05\x06\x07\x08\x09", 10));

    /* Test MOVE with no overlap, overlap on either side, and complete
       overlap (ie. identical source and destination). */
    _c(mem);
    e4t__ASSERT_OK(e4__evaluate(task, "foo 5 chars + foo 5 chars move", -1));
    e4t__ASSERT(!memcmp(mem, "\x05\x06\x07\x08\x09\x05\x06\x07\x08\x09", 10));

    _c(mem);
    e4t__ASSERT_OK(e4__evaluate(task, "foo 3 chars + foo 5 chars move", -1));
    e4t__ASSERT(!memcmp(mem, "\x03\x04\x05\x06\x07\x05\x06\x07\x08\x09", 10));

    _c(mem);
    e4t__ASSERT_OK(e4__evaluate(task, "foo foo 3 chars + 5 chars move", -1));
    e4t__ASSERT(!memcmp(mem, "\x00\x01\x02\x00\x01\x02\x03\x04\x08\x09", 10));

    _c(mem);
    e4t__ASSERT_OK(e4__evaluate(task, "foo foo 10 chars move", -1));
    e4t__ASSERT(!memcmp(mem, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10));

    #undef _c

    /* Test that BUFFER: works as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, "create foo here forget foo", -1));
    here0 = (e4__usize)e4__stack_pop(task);
    e4t__ASSERT_OK(e4__evaluate(task, "10 cells buffer: foo here foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), here0);
    here1 = (e4__usize)e4__stack_pop(task);
    e4t__ASSERT_EQ(here1 - here0, e4__mem_cells(10));

    /* Test that UNUSED returns the number of unused address units of
       zero if there are none. */
    task = e4t__transient_task();
    e4t__ASSERT_OK(e4__evaluate(task, "unused", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4__stack_pop(task);
    e4t__ASSERT_OK(e4__evaluate(task, "unused allot unused", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_EQ(e4__evaluate(task, "1 allot", -1), e4__E_DICTOVERFLOW);

    /* Calling ALLOT with a positive number should now raise an
       exception, so directly call the appropriate kernel API
       instead. */
    e4__task_allot_unchecked(task, e4__mem_cells(1));

    e4t__ASSERT_OK(e4__evaluate(task, "unused", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_OK(e4__evaluate(task, "-1 cells allot", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "unused", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_OK(e4__evaluate(task, "-1 cells allot", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "unused", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__mem_cells(1));

    /* Calling ALLOT with a negative number that is too large should
       underflow. */
    task = e4t__transient_task();
    e4t__ASSERT_EQ(e4__evaluate(task, "-1 allot", -1), e4__E_DICTUNDERFLOW);
}

/* Covers >NUMBER and BASE uservar */
static void e4t__test_builtin_parsenum(void)
{
    struct e4__task *task = e4t__transient_task();
    e4__cell base = e4__task_uservar(task, e4__UV_BASE);
    e4__usize rcount;
    const char *remaining;
    e4__usize result;

    /* FIXME: Add more to this test once double-cell numbers have been
       implemented. */
    e4t__ASSERT_EQ(e4__DEREF(base), 10);

    #define _push(s, n) \
        do {    \
            e4__stack_push(task, (e4__cell)n);  \
            e4__stack_push(task, (e4__cell)0);  \
            e4__stack_push(task, (e4__cell)s);  \
            e4__stack_push(task, (e4__cell)(sizeof(s) - 1));    \
        } while (0)
    #define _pop() \
        do {    \
            rcount = (e4__usize)e4__stack_pop(task);    \
            remaining = (const char *)e4__stack_pop(task);  \
            e4__stack_pop(task);    \
            result = (e4__usize)e4__stack_pop(task);    \
        } while (0)

    _push("42", 0);
    e4t__ASSERT_OK(e4__evaluate(task, ">NUMBER", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    _pop();
    e4t__ASSERT_EQ(result, 42);
    e4t__ASSERT_EQ(rcount, 0);

    _push("42 f", 0);
    e4t__ASSERT_OK(e4__evaluate(task, ">NUMBER", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    _pop();
    e4t__ASSERT_EQ(result, 42);
    e4t__ASSERT_EQ(rcount, 2);
    e4t__ASSERT(!e4__mem_strncasecmp(remaining, " f", rcount));

    _push("-10", 50);
    e4t__ASSERT_OK(e4__evaluate(task, ">NUMBER", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    _pop();
    e4t__ASSERT_EQ(result, 500);
    e4t__ASSERT_EQ(rcount, 3);
    e4t__ASSERT(!e4__mem_strncasecmp(remaining, "-10", rcount));

    e4__DEREF(base) = (e4__cell)16;
    _push("ff-3", 0);
    e4t__ASSERT_OK(e4__evaluate(task, ">NUMBER", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    _pop();
    e4t__ASSERT_EQ(result, 255);
    e4t__ASSERT_EQ(rcount, 2);
    e4t__ASSERT(!e4__mem_strncasecmp(remaining, "-3", rcount));
    e4__DEREF(base) = (e4__cell)10;

    _push("1000", 4);
    e4t__ASSERT_OK(e4__evaluate(task, ">NUMBER", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    _pop();
    e4t__ASSERT_EQ(result, 1040);
    e4t__ASSERT_EQ(rcount, 0);

    _push("'a'", 0);
    e4t__ASSERT_OK(e4__evaluate(task, ">NUMBER", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    _pop();
    e4t__ASSERT_EQ(result, 0);
    e4t__ASSERT_EQ(rcount, 3);
    e4t__ASSERT(!e4__mem_strncasecmp(remaining, "'a'", rcount));

    #undef _pop
    #undef _push
}

/* Covers [CHAR] CHAR REFILL WORD */
static void e4t__test_builtin_parseword(void)
{
    struct e4__task *task = e4t__transient_task();
    char *res;
    e4__u8 len;

    e4t__term_ibuf_feed("first second third", -1);

    e4__builtin_exec(task, e4__B_REFILL);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);

    e4__stack_push(task, (e4__cell)(e4__usize)' ');
    e4__builtin_exec(task, e4__B_WORD);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 5);
    e4t__ASSERT(!e4__mem_strncasecmp(res, "first", len));

    e4__stack_push(task, (e4__cell)(e4__usize)' ');
    e4__builtin_exec(task, e4__B_WORD);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 6);
    e4t__ASSERT(!e4__mem_strncasecmp(res, "second", len));

    e4__stack_push(task, (e4__cell)(e4__usize)' ');
    e4__builtin_exec(task, e4__B_WORD);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 5);
    e4t__ASSERT(!e4__mem_strncasecmp(res, "third", len));

    e4__stack_push(task, (e4__cell)(e4__usize)' ');
    e4__builtin_exec(task, e4__B_WORD);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 0);

    /* Test that [CHAR] and CHAR work as expected. */
    e4t__ASSERT_EQ(e4__evaluate(task, "char", -1), e4__E_ZLNAME);
    e4t__ASSERT_OK(e4__evaluate(task, "char f", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 'f');
    e4t__ASSERT_OK(e4__evaluate(task, "char bar", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 'b');


    e4t__ASSERT_EQ(e4__evaluate(task, "[char]", -1), e4__E_COMPONLYWORD);
    e4t__ASSERT_EQ(e4__evaluate(task, ": foo [char]", -1), e4__E_ZLNAME);
    e4__compile_cancel(task);
    e4__stack_clear(task);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo [char] b ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": bar [char] quux ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 'b');
    e4t__ASSERT_OK(e4__evaluate(task, "bar", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 'q');
}

/* Covers >R 2>R 2R> 2R@ R> R@ */
static void e4t__test_builtin_rstackmanip(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that attempting to interpret return stack manipulation words
       fails and that >r reports data stack underflow correctly. */
    e4t__ASSERT_EQ(e4__evaluate(task, "r@", -1), e4__E_COMPONLYWORD);
    e4t__ASSERT_EQ(e4__evaluate(task, "r>", -1), e4__E_COMPONLYWORD);
    e4t__ASSERT_EQ(e4__evaluate(task, ">r", -1), e4__E_COMPONLYWORD);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo >r ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "foo", -1), e4__E_STKUNDERFLOW);
    e4t__ASSERT_OK(e4__evaluate(task, "forget foo", -1));

    /* Test that attempting invalid return stack manipulations throws
       exceptions as appropriate. */
    e4t__ASSERT_OK(e4__evaluate(task, ": marker-hack ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": foo r> ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "foo", -1), e4__E_RSTKUNDERFLOW);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo r> r> ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "foo", -1), e4__E_RSTKUNDERFLOW);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo r> r> r> ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "foo", -1), e4__E_RSTKUNDERFLOW);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo >r ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "1 foo", -1), e4__E_RSTKIMBALANCE);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo >r >r ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 foo", -1), e4__E_RSTKIMBALANCE);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo >r >r >r ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 foo", -1), e4__E_RSTKIMBALANCE);
    e4t__ASSERT_OK(e4__evaluate(task, "forget marker-hack", -1));

    /* Test that these exceptions are thrown even when a thread is
       being run within another thread. */
    e4t__ASSERT_OK(e4__evaluate(task, ": marker-hack ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": foo r> ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": bar foo ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "bar", -1), e4__E_RSTKUNDERFLOW);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo r> r> ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": bar foo ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "bar", -1), e4__E_RSTKUNDERFLOW);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo r> r> r> ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": bar foo ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "bar", -1), e4__E_RSTKUNDERFLOW);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo >r ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": bar foo ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "1 bar", -1), e4__E_RSTKIMBALANCE);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo >r >r ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": bar foo ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 bar", -1), e4__E_RSTKIMBALANCE);
    e4t__ASSERT_OK(e4__evaluate(task, ": foo >r >r >r ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": bar foo ;", -1));
    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 bar", -1), e4__E_RSTKIMBALANCE);
    e4t__ASSERT_OK(e4__evaluate(task, "forget marker-hack", -1));

    /* Test that moving to and from the return stack works correctly. */
    e4t__ASSERT_OK(e4__evaluate(task, ": my-dup >r r@ r> ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "5 my-dup", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);

    /* Test that 2-variants of return stack manipulation words work
       correctly. */
    e4t__ASSERT_OK(e4__evaluate(task, ": my-swap >r >r 2r> ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "10 20 my-swap", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 10);
    e4t__ASSERT_EQ(e4__stack_pop(task), 20);

    e4t__ASSERT_OK(e4__evaluate(task, ": my-other-swap 2>r r> r> ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "30 40 my-other-swap", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 30);
    e4t__ASSERT_EQ(e4__stack_pop(task), 40);

    e4t__ASSERT_OK(e4__evaluate(task,
            ": my-other-other-swap >r >r 2r@ r> r> drop drop ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "50 60 my-other-other-swap", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 50);
    e4t__ASSERT_EQ(e4__stack_pop(task), 60);

    e4t__ASSERT_OK(e4__evaluate(task, ": my-2dup 2>r 2r@ 2r> ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "5 7 my-2dup", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 4);
    e4t__ASSERT_EQ(e4__stack_pop(task), 7);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
    e4t__ASSERT_EQ(e4__stack_pop(task), 7);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
}

/* Covers .S 2DROP ?DUP 2DUP 2OVER 2SWAP CLEAR DROP DUP OVER NIP PICK
   ROT SWAP TUCK ROLL QUIT */
static void e4t__test_builtin_stackmanip(void)
{
    struct e4__task *task = e4t__transient_task();

    /* Test that underflows are expressed properly. */

    e4t__ASSERT_EQ(e4__evaluate(task, "drop", -1), e4__E_STKUNDERFLOW);
    e4t__ASSERT_EQ(e4__evaluate(task, "dup", -1), e4__E_STKUNDERFLOW);
    e4t__ASSERT_EQ(e4__evaluate(task, "1 over", -1), e4__E_STKUNDERFLOW);
    e4t__ASSERT_EQ(e4__evaluate(task, "1 over", -1), e4__E_STKUNDERFLOW);
    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 rot", -1), e4__E_STKUNDERFLOW);
    e4t__ASSERT_EQ(e4__evaluate(task, "1 swap", -1), e4__E_STKUNDERFLOW);
    e4t__ASSERT_EQ(e4__evaluate(task, "1 tuck", -1), e4__E_STKUNDERFLOW);
    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 roll", -1), e4__E_STKUNDERFLOW);
    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 roll", -1), e4__E_STKUNDERFLOW);
    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 4 roll", -1), e4__E_STKUNDERFLOW);

    /* Test for behavioral correctness. */

    e4t__ASSERT_OK(e4__evaluate(task, "1 -2 3 .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 1 -2 3 ");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 clear .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<0> ");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 drop .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<2> 1 2 ");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 dup .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 2 3 3 ");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 over .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 2 3 2 ");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 rot .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 2 3 1 ");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 swap .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 1 3 2 ");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 tuck .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 3 2 3 ");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 4 5 0 roll .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<5> 1 2 3 4 5 ");
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 4 5 1 roll .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<5> 1 2 3 5 4 ");
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 4 5 2 roll .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<5> 1 2 4 5 3 ");
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 4 5 3 roll .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<5> 1 3 4 5 2 ");
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 4 5 4 roll .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<5> 2 3 4 5 1 ");
    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 4 5 5 roll .s clear", -1),
            e4__E_STKUNDERFLOW);

    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 quit 4 5", -1), e4__E_QUIT);
    e4t__ASSERT_OK(e4__evaluate(task, ".s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 1 2 3 ");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 nip .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<2> 1 3 ");
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 4 nip .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 1 2 4 ");

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 0 pick .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 2 3 3 ");
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 1 pick .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 2 3 2 ");
    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 2 pick .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 1 2 3 1 ");
    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 3 pick .s clear", -1),
            e4__E_STKUNDERFLOW);

    /* Test that 2-variants of words work as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, "10 20 2drop .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<0> ");
    e4t__ASSERT_OK(e4__evaluate(task, "10 20 2dup .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 10 20 10 20 ");
    e4t__ASSERT_OK(e4__evaluate(task, "10 20 30 40 2over .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<6> 10 20 30 40 10 20 ");
    e4t__ASSERT_OK(e4__evaluate(task, "10 20 30 40 2swap .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<4> 30 40 10 20 ");

    /* Test that ?DUP behaves as expected. */
    e4t__ASSERT_OK(e4__evaluate(task, "0 ?dup", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_OK(e4__evaluate(task, "5 ?dup", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
    e4t__ASSERT_EQ(e4__stack_pop(task), 5);
}

/* Covers TIME&DATE */
static void e4t__test_builtin_timedate(void)
{
    time_t before, after;
    e4__u8 attempts;
    e4__usize res;
    struct e4__task *task = e4t__transient_task();
    struct tm *t;

    /* XXX: The behavior of the utility underlying TIME&DATE is tested
       more extensively in util.c. This is just a simple test to ensure
       the builtin itself works as expected. */

    /* Compare the time reported by TIME&DATE to the time reported by
       gmtime(time(NULL). If the Unix timestamp happens to change
       between our initial call to time and our invocation of the
       TIME&DATE plugin, retry a few times. */
    for (attempts = 0; attempts < 3; ++attempts) {
        before = time(NULL);
        res = e4__evaluate(task, "clear time&date", -1);
        after = time(NULL);
        if (before == after)
            break;
    }

    e4t__ASSERT(attempts < 3);
    e4t__ASSERT_OK(res);
    e4t__ASSERT_EQ(e4__stack_depth(task), 6);

    t = gmtime(&before);
    e4t__ASSERT(t != NULL);

    /* XXX: The follow tests may fail in the event of a leap second if
       somehow gmtime happens to actually handle and report leap
       seconds. Both of these things are very unlikely and if the test
       fails for what appears to be this reason, it should be simple
       enough to verify whether or not a leap second has actually
       occurred. As such, this limitation is acceptable. */

    e4t__ASSERT_EQ(e4__stack_pop(task), t->tm_year + 1900);
    e4t__ASSERT_EQ(e4__stack_pop(task), t->tm_mon + 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), t->tm_mday);
    e4t__ASSERT_EQ(e4__stack_pop(task), t->tm_hour);
    e4t__ASSERT_EQ(e4__stack_pop(task), t->tm_min);
    e4t__ASSERT_EQ(e4__stack_pop(task), t->tm_sec);
}

/* Covers S" and S\" interpretation semantics. */
static void e4t__test_builtin_transientstr(void)
{
    struct e4__task *task = e4t__transient_task();

    /* XXX: Circular buffer wrap-around receives more in-depth coverage
       in e4t__test_util_cbuf. This test focuses instead on the
       additional functionality provided by the S" and S\" words
       themselves (as opposed to the e4__cbuf APIs that they wrap). */

    e4t__term_obuf_consume();

    /* Test that zero length strings work correctly. */
    e4t__ASSERT_OK(e4__evaluate(task, "s\" \"", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_EQ(e4__stack_pop(task), NULL);

    e4t__ASSERT_OK(e4__evaluate(task, "s\\\" \"", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);
    e4t__ASSERT_EQ(e4__stack_pop(task), NULL);

    /* Test that strings work correctly in general. */
    e4t__ASSERT_OK(e4__evaluate(task, "s\" foo bar bas\"", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 11);
    e4t__ASSERT(!e4__mem_strncasecmp((const char *)e4__stack_pop(task),
            "foo bar bas", 11));

    e4t__ASSERT_OK(e4__evaluate(task, "s\\\" foo bar\\nbas\\z\"", -1));
    e4t__ASSERT_EQ(e4__stack_pop(task), 12);
    e4t__ASSERT_MATCH((const char *)e4__stack_pop(task), "foo bar\nbas");

    /* Test that two consecutive calls to S" and/or S\" both work
       correctly. */
    e4t__ASSERT_OK(e4__evaluate(task, "s\" bar\" s\" foo\" type type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "foobar");

    e4t__ASSERT_OK(e4__evaluate(task, "s\\\" bar\" s\" foo\" type type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "foobar");

    e4t__ASSERT_OK(e4__evaluate(task, "s\" bar\" s\\\" foo\" type type", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "foobar");

    e4t__ASSERT_OK(e4__evaluate(task, "s\\\" bar\" s\\\" foo\" type type",
            -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "foobar");

    /* Test that strings that would overflow the buffer raise the
       appropriate exception. */
    e4t__ASSERT_EQ(e4__evaluate(task, "s\" aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"",
            -1), e4__E_PSTROVERFLOW);
    e4t__ASSERT_EQ(e4__evaluate(task, "s\\\" aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"",
            -1), e4__E_PSTROVERFLOW);
}

/* Covers >IN BASE HERE PAD SOURCE SOURCE-ID STATE */
static void e4t__test_builtin_uservars(void)
{
    struct e4__task *task = e4t__transient_task();

    /* XXX: Technically SOURCE is not implemented as a uservar, but the
       test for it is included here since from a behavior standpoint it
       behaves very much like a uservar. */

    /* Test >IN. */
    e4t__ASSERT_OK(e4__evaluate(task, "        >in @", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 14);

    e4t__ASSERT_OK(e4__evaluate(task, "123456 depth over 9 < 35 and + >in !",
            -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 6);
    e4t__ASSERT_EQ(e4__stack_pop(task), 6);
    e4t__ASSERT_EQ(e4__stack_pop(task), 56);
    e4t__ASSERT_EQ(e4__stack_pop(task), 456);
    e4t__ASSERT_EQ(e4__stack_pop(task), 3456);
    e4t__ASSERT_EQ(e4__stack_pop(task), 23456);
    e4t__ASSERT_EQ(e4__stack_pop(task), 123456);

    /* Test BASE. */
    e4t__ASSERT_OK(e4__evaluate(task, "base", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__task_uservar(task, e4__UV_BASE));

    /* Test HERE. */
    e4t__ASSERT_OK(e4__evaluate(task, "here", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__task_uservar(task, e4__UV_HERE));

    /* Test PAD. */
    e4t__ASSERT_OK(e4__evaluate(task, "pad", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__task_uservar(task, e4__UV_PAD));

    /* Test SOURCE. */
    e4t__ASSERT_OK(e4__evaluate(task,
            ": test-source s\" source\" 2dup evaluate >r swap >r = r> r> = ;",
            -1));
    e4t__ASSERT_OK(e4__evaluate(task, "test-source", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 2);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);

    e4t__ASSERT_OK(e4__evaluate(task, "forget test-source", -1));
    e4t__ASSERT_OK(e4__evaluate(task, ": test-source source >in ! drop ;",
            -1));
    e4t__ASSERT_OK(e4__evaluate(task, "test-source 123 456 notevenarealword",
            -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 0);

    /* Test SOURCE-ID. */
    e4t__ASSERT_OK(e4__evaluate(task, "source-id", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__SID_STR);

    /* Test STATE. */
    e4t__ASSERT_OK(e4__evaluate(task, ": compiling-now? state @ ; immediate",
            -1));
    e4t__ASSERT_OK(e4__evaluate(task, "compiling-now?", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 0);

    e4t__ASSERT_OK(e4__evaluate(task, ": foo compiling-now? literal ;", -1));
    e4t__ASSERT_OK(e4__evaluate(task, "foo", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), 1);
}

void e4t__test_builtin(void)
{
    e4t__test_builtin_doublemath();
    e4t__test_builtin_comments();
    e4t__test_builtin_constants();
    e4t__test_builtin_data();
    e4t__test_builtin_defer();
    e4t__test_builtin_does();
    e4t__test_builtin_environmentq();
    e4t__test_builtin_evaluate();
    e4t__test_builtin_exceptions();
    e4t__test_builtin_file_constants();
    e4t__test_builtin_file_include();
    e4t__test_builtin_file_nested();
    e4t__test_builtin_find();
    e4t__test_builtin_forget();
    e4t__test_builtin_immed_cond();
    e4t__test_builtin_immediate();
    e4t__test_builtin_io();
    e4t__test_builtin_io_accept();
    e4t__test_builtin_io_dump();
    e4t__test_builtin_io_error();
    e4t__test_builtin_io_facility();
    e4t__test_builtin_io_pno();
    e4t__test_builtin_logic();
    e4t__test_builtin_loop_inc();
    e4t__test_builtin_math();
    e4t__test_builtin_memmanip();
    e4t__test_builtin_parsenum();
    e4t__test_builtin_parseword();
    e4t__test_builtin_rstackmanip();
    e4t__test_builtin_stackmanip();
    e4t__test_builtin_timedate();
    e4t__test_builtin_transientstr();
    e4t__test_builtin_uservars();
}
