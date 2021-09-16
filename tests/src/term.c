#include "e4.h"
#include "e4t.h"

#include <string.h>

struct e4t__term_ibuf {
    const char *buf;
    e4__usize offset;
    e4__usize length;
};

struct e4t__term_obuf {
    char buf[2][512];
    e4__usize offset;
    e4__u8 active;
};

static struct e4t__term_ibuf e4t__term_ib = {0,};
static struct e4t__term_obuf e4t__term_ob = {0,};

static e4__usize e4t__term_ibuf_accept(void *user, char *buf, e4__usize *n)
{
    struct e4t__term_ibuf *b = &e4t__term_ib;

    if (!b->buf)
        *n = 0;
    else if (*n > b->length - b->offset)
        *n = b->length - b->offset;

    memcpy(buf, &b->buf[b->offset], *n);
    b->offset += *n;

    return e4__E_OK;
}

static e4__usize e4t__term_ibuf_key(void *user, char *buf)
{
    struct e4t__term_ibuf *b = &e4t__term_ib;

    if (!b->buf || b->offset >= b->length)
        return e4__E_FAILURE;

    *buf = b->buf[b->offset++];

    return e4__E_OK;
}

static e4__usize e4t__term_ibuf_keyq(void *user, e4__usize *bflag)
{
    struct e4t__term_ibuf *b = &e4t__term_ib;

    if (!b->buf)
        return e4__E_FAILURE;

    *bflag = (b->offset >= b->length) ? e4__BF_FALSE : e4__BF_TRUE;

    return e4__E_OK;
}

static e4__usize e4t__term_obuf_type(void *user, const char *buf, e4__usize n)
{
    struct e4t__term_obuf *b = &e4t__term_ob;

    /* XXX: Rather than returning an error, simply copy what we can. */
    if (b->offset + n > (sizeof(b->buf[0]) - 1))
        n = sizeof(b->buf[0]) - 1 - b->offset;

    memcpy(&b->buf[b->active][b->offset], buf, n);
    b->offset += n;
    b->buf[b->active][b->offset] = '\0';

    return e4__E_OK;
}

void e4t__term_io_init(struct e4__task *task)
{
    struct e4__io_func io_func = {
        NULL,
        e4t__term_ibuf_accept,
        e4t__term_ibuf_key,
        e4t__term_obuf_type,
        e4t__term_ibuf_keyq,
    };

    e4__task_io_init(task, &io_func);
}

/* "Sets" a string as the input buffer. This string will be read
   from by the test input buffer handlers until it is exhausted,
   at which point the accept handler will return 0 length strings
   and the key handler will return e4__E_FAILURE. */
void e4t__term_ibuf_feed(const char *buf, e4__usize length)
{
    struct e4t__term_ibuf *b = &e4t__term_ib;

    b->buf = buf;
    b->length = length != (e4__usize)-1 ? length : strlen(buf);
    b->offset = 0;
}

/* "Erases" whatever is currently in the output buffer and returns
   a pointer to what was there as a NUL terminated C-string. This
   data is only valid until this function is next called. */
const char* e4t__term_obuf_consume(void)
{
    struct e4t__term_obuf *b = &e4t__term_ob;
    unsigned old_active = b->active;

    b->offset = 0;
    b->active = (old_active + 1) % 2;
    b->buf[b->active][b->offset] = '\0';

    return b->buf[old_active];
}

/* Returns a pointer to the current contents of the output buffer as
   a NUL terminated C-string. This data is valid until obuf_consume has
   been called, but it will continue to grow any time an e4 builtin that
   types output is called. */
const char* e4t__term_obuf_peek(void)
{
    struct e4t__term_obuf *b = &e4t__term_ob;
    return b->buf[b->active];
}

void e4t__term_selftest(void)
{
    struct e4__task *task = e4t__transient_task();
    char buf[32];
    e4__usize len;
    e4__usize key_avail;

    e4t__ASSERT_OK(e4__io_type(task, "Hello, ", -1));
    e4t__ASSERT_MATCH("Hello, ", e4t__term_obuf_peek());
    e4t__ASSERT_MATCH("Hello, ", e4t__term_obuf_peek());

    e4t__ASSERT_OK(e4__io_type(task, "World!", -1));
    e4t__ASSERT_MATCH("Hello, World!", e4t__term_obuf_peek());
    e4t__ASSERT_MATCH("Hello, World!", e4t__term_obuf_consume());
    e4t__ASSERT_MATCH("", e4t__term_obuf_peek());
    e4t__ASSERT_MATCH("", e4t__term_obuf_consume());

    e4t__term_ibuf_feed("three six nine", -1);

    len = 5;
    e4t__ASSERT_OK(e4__io_accept(task, buf, &len));
    e4t__ASSERT_EQ(len, 5);
    buf[len] = '\0';
    e4t__ASSERT_MATCH(buf, "three");

    len = 7;
    e4t__ASSERT_OK(e4__io_accept(task, buf, &len));
    e4t__ASSERT_EQ(len, 7);
    buf[len] = '\0';
    e4t__ASSERT_MATCH(buf, " six ni");

    e4t__ASSERT_OK(e4__io_keyq(task, &key_avail));
    e4t__ASSERT_EQ(key_avail, e4__BF_TRUE);
    e4t__ASSERT_OK(e4__io_key(task, buf));
    e4t__ASSERT_EQ(buf[0], 'n');

    e4t__ASSERT_OK(e4__io_keyq(task, &key_avail));
    e4t__ASSERT_EQ(key_avail, e4__BF_TRUE);
    e4t__ASSERT_OK(e4__io_key(task, buf));
    e4t__ASSERT_EQ(buf[0], 'e');

    e4t__ASSERT_OK(e4__io_keyq(task, &key_avail));
    e4t__ASSERT_EQ(key_avail, e4__BF_FALSE);
    e4t__ASSERT_EQ(e4__io_key(task, buf), e4__E_FAILURE);
    e4t__ASSERT_EQ(buf[0], 'e');

    len = 7;
    e4t__ASSERT_OK(e4__io_accept(task, buf, &len));
    e4t__ASSERT_EQ(len, 0);
    e4t__ASSERT_EQ(buf[0], 'e');
}
