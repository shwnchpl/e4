#include "e4.h"
#include "../e4t.h" /* FIXME: Add this to an include path? */

#include <string.h>

static void e4t__test_util_exceptionformat(void)
{
    e4__usize len = 0;

    /* Test formatting a few exceptions. */
    e4t__ASSERT_MATCH(e4__num_format_exception(e4__E_OK, NULL), "ok");
    e4t__ASSERT_MATCH(e4__num_format_exception(e4__E_STKUNDERFLOW, NULL),
            "stack underflow");
    e4t__ASSERT_MATCH(e4__num_format_exception(1345, NULL), "unknown");

    /* Test that, when requested, length is set as expected. */
    e4t__ASSERT_MATCH(e4__num_format_exception(e4__E_RSTKUNDERFLOW, &len),
            "return stack underflow");
    e4t__ASSERT_EQ(len, 22);
}

static void e4t__test_util_math(void)
{
    /* XXX: Parts of this test only work correctly on a 64 bit system
       that represents negative numbers using two's complement. */
    e4t__ASSERT_EQ(e4__USIZE_NEGATE((e4__usize)5), -5);
    e4t__ASSERT_EQ(e4__USIZE_IS_NEGATIVE((e4__usize)-5), 1);

    e4t__ASSERT_EQ(e4__num_sdiv((e4__usize)-10, (e4__usize)3), -3);
    e4t__ASSERT_EQ(e4__num_sdiv((e4__usize)10, (e4__usize)-3), -3);
    e4t__ASSERT_EQ(e4__num_sdiv((e4__usize)-10, (e4__usize)-3), 3);
    e4t__ASSERT_EQ(e4__num_sdiv((e4__usize)10, (e4__usize)3), 3);

    e4t__ASSERT_EQ(e4__num_smod((e4__usize)10, (e4__usize)7), 3);
    e4t__ASSERT_EQ(e4__num_smod((e4__usize)-10, (e4__usize)7), -3);
    e4t__ASSERT_EQ(e4__num_smod((e4__usize)10, (e4__usize)-7), 3);
    e4t__ASSERT_EQ(e4__num_smod((e4__usize)-10, (e4__usize)-7), -3);

    e4t__ASSERT_EQ(e4__mem_aligned(0), 0);
    e4t__ASSERT_EQ(e4__mem_aligned(1), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(2), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(3), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(4), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(5), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(6), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(7), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(8), 8);
    e4t__ASSERT_EQ(e4__mem_aligned(9), 16);
}

static void e4t__test_util_mem_dict(void)
{
    char buf[4096] = {0,};
    char *here = buf;
    e4__usize wrote;
    struct e4__dict_header *dict = NULL;
    const struct e4__dict_header *last;

    #define _d(s, c)    \
        do {    \
            wrote = e4__mem_dict_entry(here, dict, s, sizeof(s) - 1, 0, c,  \
                    NULL);  \
            dict = (struct e4__dict_header *)here;  \
            here += wrote;  \
        } while (0)
    #define _l(s)   e4__mem_dict_lookup(dict, s, sizeof(s) - 1)
    #define _s(d, p)    e4__mem_dict_suggest(d, p, sizeof(p) - 1)

    /* Test basic lookup. */
    _d("first-entry", (e4__code_ptr)0xabcde);
    e4t__ASSERT_EQ(_l("first-entry"), dict);
    e4t__ASSERT_EQ(_l("first-entry")->xt->code, 0xabcde);
    e4t__ASSERT_EQ((e4__usize)here % sizeof(e4__cell), 0);

    _d("second-entry", (e4__code_ptr)0x12345);
    e4t__ASSERT_EQ(_l("second-entry"), dict);
    e4t__ASSERT_EQ(_l("second-entry")->xt->code, 0x12345);

    e4t__ASSERT(_l("first-entry"));
    e4t__ASSERT_EQ(_l("first-entry")->xt->code, 0xabcde);
    e4t__ASSERT_EQ(_l("FIRST-entry")->xt->code, 0xabcde);

    e4t__ASSERT_EQ((e4__usize)_l("not-present"), (e4__usize)NULL);

    /* Test prefix suggestion. */
    _d("prefix-first", (e4__code_ptr)0x11111);
    _d("prefix-second", (e4__code_ptr)0x22222);
    _d("prefix-third", (e4__code_ptr)0x33333);

    e4t__ASSERT((last = _s(dict, "prefix")));
    e4t__ASSERT_EQ(last->xt->code, 0x33333);
    e4t__ASSERT((last = _s(last->link, "prefix")));
    e4t__ASSERT_EQ(last->xt->code, 0x22222);
    e4t__ASSERT((last = _s(last->link, "prefix")));
    e4t__ASSERT_EQ(last->xt->code, 0x11111);
    e4t__ASSERT(!(last = _s(last->link, "prefix")));

    #undef _s
    #undef _l
    #undef _d
}

static void e4t__test_util_mem_dump(void)
{
    /* XXX: Parts of this test only work correctly on a 64 bit system
       where unsigned long values have 8 bit alignment. */

    static const char *test_str =
            "test \x88\x99\xaa string with some \x01 \x02 \x03 data";

    const char *cursor;
    e4__usize wrote_sz;
    unsigned long buffer[8];
    char line[80] = {0,};
    char expected_line[3][80] = {0,};
    char *unaligned_buf = ((char *)buffer) + 3;
    e4__usize remaining = strlen(test_str);

    #define _f  "%016lx   "
    sprintf(expected_line[0],
            _f "???? ??74 6573 7420  8899 aa20 7374 7269   ...test ... stri\n",
            (unsigned long)&buffer[0]);
    sprintf(expected_line[1],
            _f "6e67 2077 6974 6820  736f 6d65 2001 2002   ng with some . .\n",
            (unsigned long)&buffer[2]);
    sprintf(expected_line[2],
            _f "2003 2064 6174 61??  ???? ???? ???? ????    . data.........\n",
            (unsigned long)&buffer[4]);
    #undef _f

    /* XXX: It would violate strict aliasing rules if we ever looked at
       buffer again after modifying it using unaligned_buf. So long as
       we don't do that, everything should be fine. */
    memcpy(unaligned_buf, test_str, remaining);
    cursor = unaligned_buf;

    e4t__ASSERT_EQ(remaining, 36);

    wrote_sz = e4__mem_dump(&cursor, &remaining, line);
    e4t__ASSERT_EQ(wrote_sz, 79);
    e4t__ASSERT_EQ(remaining, 23);
    e4t__ASSERT_MATCH(line, expected_line[0]);

    wrote_sz = e4__mem_dump(&cursor, &remaining, line);
    e4t__ASSERT_EQ(wrote_sz, 79);
    e4t__ASSERT_EQ(remaining, 7);
    e4t__ASSERT_MATCH(line, expected_line[1]);

    wrote_sz = e4__mem_dump(&cursor, &remaining, line);
    e4t__ASSERT_EQ(wrote_sz, 79);
    e4t__ASSERT_EQ(remaining, 0);
    e4t__ASSERT_MATCH(line, expected_line[2]);
}

static void e4t__test_util_numformat(void)
{
    static char buf[31] = {0,};

    #define _f(n, b, f) e4__num_format(n, b, f, buf, sizeof(buf) - 1)
    e4t__ASSERT_MATCH(_f(531, 10, 0), "531");
    e4t__ASSERT_MATCH(_f(0xf3, 16, 0), "f3");
    e4t__ASSERT_MATCH(_f(-0xf3, 16, 0), "ffffffffffffff0d");
    e4t__ASSERT_MATCH(_f(-0xf3, 16, e4__F_SIGNED), "-f3");
    e4t__ASSERT_MATCH(_f(46655, 36, e4__F_SIGNED), "zzz");
    e4t__ASSERT_MATCH(_f(-50, 2, e4__F_SIGNED), "-110010");
    e4t__ASSERT_MATCH(_f(0, 2, e4__F_SIGNED), "0");
    #undef _f
}

static void e4t__test_util_numparse(void)
{
    /* XXX: Parts of this test only work correctly on a system that
       represents negative numbers using two's complement. */
    e4__usize num = 0;

    #define _p(n, b, f) e4__mem_number(n, sizeof(n) - 1, b, f, &num)
    #define _f  (e4__F_CHAR_LITERAL | e4__F_NEG_PREFIX | e4__F_BASE_PREFIX)

    e4t__ASSERT_EQ(_p("0xac", 10, _f), 4);
    e4t__ASSERT_EQ(num, 172);

    e4t__ASSERT_EQ(_p("ac", 16, _f), 2);
    e4t__ASSERT_EQ(num, 172);

    e4t__ASSERT_EQ(_p("ac", 5, _f), 0);
    e4t__ASSERT_EQ(num, 172);

    e4t__ASSERT_EQ(_p("32ac", 5, _f), 2);
    e4t__ASSERT_EQ(num, 17);

    e4t__ASSERT_EQ(_p("$ac", 5, _f), 3);
    e4t__ASSERT_EQ(num, 172);

    e4t__ASSERT_EQ(_p("-$ac", 5, _f), 4);
    e4t__ASSERT_EQ(num, -172);

    e4t__ASSERT_EQ(_p("%011011", 5, _f), 7);
    e4t__ASSERT_EQ(num, 27);

    e4t__ASSERT_EQ(_p("0b011011", 5, _f), 8);
    e4t__ASSERT_EQ(num, 27);

    e4t__ASSERT_EQ(_p("0o777xx", 5, _f), 5);
    e4t__ASSERT_EQ(num, 511);

    e4t__ASSERT_EQ(_p("#777   ", 5, _f), 4);
    e4t__ASSERT_EQ(num, 777);

    e4t__ASSERT_EQ(_p("'a'jieoag", 5, _f), 3);
    e4t__ASSERT_EQ(num, 97);

    e4t__ASSERT_EQ(_p("zOo", 36, _f), 3);
    e4t__ASSERT_EQ(num, 46248);

    e4t__ASSERT_EQ(_p("zOo", 99, _f), 3);
    e4t__ASSERT_EQ(num, 46248);

    e4t__ASSERT_EQ(_p("000", 10, 0), 3);
    e4t__ASSERT_EQ(num, 0);

    e4t__ASSERT_EQ(_p("-5", 10, 0), 0);
    e4t__ASSERT_EQ(num, 0);

    e4t__ASSERT_EQ(_p("$5", 10, 0), 0);
    e4t__ASSERT_EQ(num, 0);

    e4t__ASSERT_EQ(_p("'5'", 10, 0), 0);
    e4t__ASSERT_EQ(num, 0);

    #undef _f
    #undef _p
}

static void e4t__test_util_wordparse(void)
{
    const char *word;
    e4__usize len;

    #define _p(s, d, f) e4__mem_parse(s, d, strlen(s), f, &word)
    len = _p("   foo bar bas", ' ', e4__F_SKIP_LEADING);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo", len));

    len = _p("   foo, bar,, bas", ',', e4__F_SKIP_LEADING);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "   foo", len));

    len = _p("         ", ' ', e4__F_SKIP_LEADING);
    e4t__ASSERT_EQ(len, 0);

    len = _p("  foo\nbar", ' ', e4__F_SKIP_LEADING);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo", len));

    len = _p("  foo.bar", ' ', e4__F_SKIP_LEADING);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo.bar", len));

    len = _p("  foo.bar", ' ', 0);
    e4t__ASSERT_EQ(len, 0);

    e4t__ASSERT_EQ((len = _p("foo\\\"bar\" bas", '"', 0)), 4);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo\\\"", len));

    e4t__ASSERT_EQ((len = _p("foo\\\"bar\" bas", '"', e4__F_IGNORE_ESC)), 8);
    e4t__ASSERT(!e4__mem_strncasecmp(word, "foo\\\"bar", len));

    #undef _p
}

static void e4t__test_util_strnescape(void)
{
    static const char *escaped_str = "foo\\abar\\b some more \\e\\f\\l"
        "and still more\\mand\\nmore\\q\\r\\tso long\\v\\z\\\"\\\\\\xf3"
        "\\p\\xr9\\";
    char buf[2];
    e4__usize chunk_len;
    const char *cursor = escaped_str;
    e4__usize len = strlen(cursor);
    const char *chunk;

    #define _expect_chunk(s)    \
        do {    \
            chunk = e4__mem_strnescape(&cursor, &len, &chunk_len, buf); \
            e4t__ASSERT(chunk); \
            e4t__ASSERT_EQ(chunk_len, sizeof(s) - 1);   \
            e4t__ASSERT(!e4__mem_strncasecmp(chunk, s, sizeof(s) - 1)); \
        } while (0)

    _expect_chunk("foo");
    _expect_chunk("\a");
    _expect_chunk("bar");
    _expect_chunk("\b");
    _expect_chunk(" some more ");
    _expect_chunk("\033");
    _expect_chunk("\f");
    _expect_chunk("\n");
    _expect_chunk("and still more");
    _expect_chunk("\r\n");
    _expect_chunk("and");
    _expect_chunk("\n");
    _expect_chunk("more");
    _expect_chunk("\"");
    _expect_chunk("\r");
    _expect_chunk("\t");
    _expect_chunk("so long");
    _expect_chunk("\v");
    _expect_chunk("\0");
    _expect_chunk("\"");
    _expect_chunk("\\");
    _expect_chunk("\xf3");
    _expect_chunk("\\p");
    _expect_chunk("\\xr9");
    _expect_chunk("\\");

    chunk = e4__mem_strnescape(&cursor, &len, &chunk_len, buf);
    e4t__ASSERT_EQ(len, 0);
    e4t__ASSERT_EQ(chunk, NULL);

    chunk = e4__mem_strnescape(&cursor, &len, &chunk_len, buf);
    e4t__ASSERT_EQ(len, 0);
    e4t__ASSERT_EQ(chunk, NULL);

    #undef _expect_chunk
}

void e4t__test_util(void)
{
    e4t__test_util_exceptionformat();
    e4t__test_util_math();
    e4t__test_util_mem_dict();
    e4t__test_util_mem_dump();
    e4t__test_util_numformat();
    e4t__test_util_numparse();
    e4t__test_util_wordparse();
    e4t__test_util_strnescape();
}
