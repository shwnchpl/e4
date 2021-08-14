#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

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
    struct e4__dict_header *header;

    e4t__term_obuf_consume();

    /* Test simple creation with allot allocation. */
    e4t__ASSERT_OK(e4__evaluate(task, "create foo 1 cells allot 531 foo !", -1));
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
    e4t__ASSERT_EQ(e4__evaluate(task, ": set-flom to flom ;", -1), e4__E_UNDEFWORD);
    e4__compile_cancel(task);
    e4t__ASSERT_EQ(e4__evaluate(task, ": set-flom to ;", -1), e4__E_INVNAMEARG);
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

/* Covers ABORT */
static void e4t__test_builtin_exceptions(void)
{
    struct e4__task *task = e4t__transient_task();

    /* FIXME: Add more builtin exception related tests as more
       exception related words are added. */

    e4t__ASSERT_EQ(e4__evaluate(task, "abort", -1), e4__E_ABORT);
}

/* Covers FORGET and look-ahead idiom (which uses builtin WORD) */
static void e4t__test_builtin_forget(void)
{
    struct e4__task *task = e4t__transient_task();

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

/* Covers . ? CR BASE EMIT PAD U. WORDS */
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

/* Covers + - . / /MOD MOD U. */
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

    #undef _e
}

/* Covers @ ! ALLOT CELLS HERE */
static void e4t__test_builtin_memmanip(void)
{
    struct e4__task *task = e4t__transient_task();
    e4__usize slot;

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

/* Covers REFILL WORD */
static void e4t__test_builtin_parseword(void)
{
    struct e4__task *task = e4t__transient_task();
    char *res;
    e4__u8 len;

    e4t__term_ibuf_feed("first second third", -1);

    e4__builtin_exec(task, e4__B_REFILL);
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__BF_TRUE);

    e4__builtin_exec(task, e4__B_WORD, ' ');
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 5);
    e4t__ASSERT(!e4__mem_strncasecmp(res, "first", len));

    e4__builtin_exec(task, e4__B_WORD, ' ');
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 6);
    e4t__ASSERT(!e4__mem_strncasecmp(res, "second", len));

    e4__builtin_exec(task, e4__B_WORD, ' ');
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 5);
    e4t__ASSERT(!e4__mem_strncasecmp(res, "third", len));

    e4__builtin_exec(task, e4__B_WORD, ' ');
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    res = (char *)e4__stack_pop(task);
    len = *res++;
    e4t__ASSERT_EQ(len, 0);
}

/* Covers CLEAR .S DROP DUP OVER ROT SWAP TUCK ROLL QUIT */
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

    e4t__ASSERT_OK(e4__evaluate(task, "1 2 3 4 5 4 roll .s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<5> 1 3 4 5 2 ");

    e4t__ASSERT_EQ(e4__evaluate(task, "1 2 3 quit 4 5", -1), e4__E_QUIT);
    e4t__ASSERT_OK(e4__evaluate(task, ".s clear", -1));
    e4t__ASSERT_MATCH(e4t__term_obuf_consume(), "<3> 1 2 3 ");
}

/* Covers BASE HERE PAD */
static void e4t__test_builtin_uservars(void)
{
    struct e4__task *task = e4t__transient_task();

    e4t__ASSERT_OK(e4__evaluate(task, "base", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__task_uservar(task, e4__UV_BASE));

    e4t__ASSERT_OK(e4__evaluate(task, "here", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__task_uservar(task, e4__UV_HERE));

    e4t__ASSERT_OK(e4__evaluate(task, "pad", -1));
    e4t__ASSERT_EQ(e4__stack_depth(task), 1);
    e4t__ASSERT_EQ(e4__stack_pop(task), e4__task_uservar(task, e4__UV_PAD));
}

void e4t__test_builtin(void)
{
    e4t__test_builtin_constants();
    e4t__test_builtin_data();
    e4t__test_builtin_defer();
    e4t__test_builtin_does();
    e4t__test_builtin_exceptions();
    e4t__test_builtin_forget();
    e4t__test_builtin_immediate();
    e4t__test_builtin_io();
    e4t__test_builtin_logic();
    e4t__test_builtin_math();
    e4t__test_builtin_memmanip();
    e4t__test_builtin_parsenum();
    e4t__test_builtin_parseword();
    e4t__test_builtin_stackmanip();
    e4t__test_builtin_uservars();
}
