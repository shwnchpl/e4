#include "e4.h"
#include "e4t.h"
#include <string.h>

struct e4t__term_obuf {
    char buf[2][512];
    e4__usize offset;
    e4__u8 active;
};

static struct e4t__term_obuf e4t__term_ob = {0,};

static e4__usize e4t__term_obuf_type(void *user, const char *buf, e4__usize n)
{
    struct e4t__term_obuf *b = &e4t__term_ob;

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
        NULL,
        NULL,
        e4t__term_obuf_type,
    };

    e4__task_io_init(task, &io_func);
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

    e4__io_type(task, "Hello, ", -1);
    e4t__ASSERT_MATCH("Hello, ", e4t__term_obuf_peek());
    e4t__ASSERT_MATCH("Hello, ", e4t__term_obuf_peek());

    e4__io_type(task, "World!", -1);
    e4t__ASSERT_MATCH("Hello, World!", e4t__term_obuf_peek());
    e4t__ASSERT_MATCH("Hello, World!", e4t__term_obuf_consume());
    e4t__ASSERT_MATCH("", e4t__term_obuf_peek());
    e4t__ASSERT_MATCH("", e4t__term_obuf_consume());
}
