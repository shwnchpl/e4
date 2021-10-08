#include "e4.h"

#include <string.h>
#include <ctype.h>

e4__usize e4__mem_aligned(e4__usize n)
{
    if (n % sizeof(e4__cell))
        n += (sizeof(e4__cell) - (n % sizeof(e4__cell)));
    return n;
}

void e4__mem_cbuf_init(struct e4__cbuf *cbuf, char *buf, e4__usize sz)
{
    cbuf->offset = 0;
    cbuf->sz = sz;
    cbuf->buf = buf;
}

/* Pushes into a circular buffer initialized with e4__mem_cbuf_init.
   Guaranteed not to smash the previous M entries of size N or less
   where M is equal to buffer size divided by N.
   Returns NULL if an entry is too large to fit in the buffer or size is
   zero. */
char* e4__mem_cbuf_push(struct e4__cbuf *cbuf, const char *buf, e4__usize sz)
{
    register char *entry;

    if (sz > cbuf->sz || !sz)
        return NULL;

    if (sz > cbuf->sz - cbuf->offset)
        cbuf->offset = 0;

    entry = &cbuf->buf[cbuf->offset];
    memmove(entry, buf, sz);
    cbuf->offset += sz;

    return entry;
}

/* Same semantics as e4__mem_cbuf_push, except buf is escaped using
   e4__mem_strnescape before it is pushed into the buffer. Escaped size,
   rather than sz itself, is considered when determining whether or not
   the requested submission will fit into the circular buffer, so a
   submission that is too large will not only fail to insert into the
   circular buffer successfully, it may also fill the circular buffer
   with garbage. As such, when NULL is returned and sz was not zero, all
   previous circular buffer entries should be considered invalidated.
   Escaped size is written back to sz only on success. */
char* e4__mem_cbuf_epush(struct e4__cbuf *cbuf, const char *buf, e4__usize *sz)
{
    register e4__usize start_offset;
    register const char *chunk;
    e4__usize len = *sz;
    e4__usize chunk_len;
    char scratch[2];

    if (!len)
        return NULL;

    start_offset = cbuf->offset;
    while ((chunk = e4__mem_strnescape(&buf, &len, &chunk_len, scratch))) {
        if (chunk_len > cbuf->sz - cbuf->offset) {
            if (!start_offset ||
                    chunk_len > cbuf->sz - (cbuf->offset - start_offset)) {
                /* The thing we're trying to insert is larger than the
                   buffer itself. Time to fail. */
                cbuf->offset = 0;
                return NULL;
            } else {
                /* The thing we're trying to insert is too large to fit
                   at the end of the buffer. We need to wrap around. */
                cbuf->offset -= start_offset;
                memmove(cbuf->buf, &cbuf->buf[start_offset], cbuf->offset);
                start_offset = 0;
            }
        }
        memmove(&cbuf->buf[cbuf->offset], chunk, chunk_len);
        cbuf->offset += chunk_len;
    }

    *sz = cbuf->offset - start_offset;

    return &cbuf->buf[start_offset];
}

/* FIXME: This function is unused internally. Consider removing. */
e4__usize e4__mem_cells(e4__usize n)
{
    return n * sizeof(e4__cell);
}

e4__usize e4__mem_dict_entry(void *here, struct e4__dict_header *prev,
        const char *name, e4__u8 nbytes, e4__u8 flags, e4__code_ptr code,
        void *user)
{
    /* FIXME: Allow passing -1 for strlen of name? */
    register struct e4__dict_header *header = here;
    register e4__usize sz;

    /* Copy name before *ANYTHING* else. Since the word parsing
       scratch area is HERE, we may be about to write over it. */
    memmove(&header[1], name, nbytes);

    header->link = prev;
    header->flags = flags;
    header->nbytes = nbytes;
    header->name = (const char *)(&header[1]);

    sz = sizeof(*header) + nbytes;

    /* Align size. */
    sz = e4__mem_aligned(sz);

    header->xt = (struct e4__execute_token *)((e4__u8 *)here + sz);
    header->xt->user = user;
    header->xt->code = code;

    sz += sizeof(*header->xt) - sizeof(header->xt->data);

    /* XXX: Really this should be impossible. */
    if (sz % sizeof(void *))
        sz += (sizeof(void *) - (sz % sizeof(void *)));

    return sz;
}

const struct e4__dict_header* e4__mem_dict_lookup(
        const struct e4__dict_header *dict, const char *name, e4__u8 nbytes)
{
    while (dict) {
        if (nbytes == dict->nbytes &&
                !e4__mem_strncasecmp(dict->name, name, nbytes))
            return dict;
        dict = dict->link;
    }

    return NULL;
}

const struct e4__dict_header* e4__mem_dict_suggest(
        const struct e4__dict_header *dict, const char *prefix, e4__u8 pbytes)
{
    while (dict) {
        if (pbytes <= dict->nbytes &&
                !e4__mem_strncasecmp(dict->name, prefix, pbytes))
            return dict;
        dict = dict->link;
    }

    return NULL;
}

/* XXX: Buffer must be able to store at least 80 characters. Specifying
   size is not an option. */
e4__usize e4__mem_dump(const char **p, e4__usize *len, char *buffer)
{
    /* For the purposes of this function, aligned to 8 bytes is
       aligned. */
    register const char *cursor = (const char *)(((e4__usize)*p / 8) * 8);
    register const char *line_end = cursor + 16;
    register const char *p_end = *p + *len;
    register e4__u8 offset = 15;
    register e4__usize v = (e4__usize)cursor;
    char digit;

    /* Format address. */
    do {
        digit = v % 16;
        buffer[offset] = digit < 10 ? '0' + digit : 'a' + digit - 10;
        v /= 16;
    } while (offset-- > 0);

    offset = 16;

    buffer[offset++] = ' ';
    buffer[offset++] = ' ';

    while (cursor < line_end) {
        if (!((e4__usize)cursor % 8))
            buffer[offset++] = ' ';

        if (cursor < *p || cursor >= p_end) {
            buffer[offset++] = '?';
            buffer[offset++] = '?';
        } else {
            v = *cursor;
            digit = v % 16;
            buffer[offset + 1] = digit < 10 ? '0' + digit : 'a' + digit - 10;

            v /= 16;
            digit = v % 16;
            buffer[offset] = digit < 10 ? '0' + digit : 'a' + digit - 10;

            offset += 2;
        }

        if ((e4__usize)cursor % 2)
            buffer[offset++] = ' ';

        ++cursor;
    }

    buffer[offset++] = ' ';
    buffer[offset++] = ' ';

    cursor -= 16;

    while (cursor < line_end) {
        if (cursor < *p || cursor >= p_end || *cursor < 0x20 ||
                *cursor > 0x7e)
            buffer[offset++] = '.';
        else
            buffer[offset++] = *cursor;

        ++cursor;
    }

    buffer[offset++] = '\n';

    cursor = (e4__usize)cursor > (e4__usize)p_end ? p_end : cursor;
    *len -= cursor - *p;
    *p = cursor;

    return offset;
}

/* XXX: Scratch must be at least two bytes wide. */
const char* e4__mem_strnescape(const char **str, e4__usize *len,
        e4__usize *chunk_len, char *scratch)
{
    const char *begin = *str;

    if (!*len)
        return NULL;

    *chunk_len = 0;

    if (*begin == '\\') {
        /* Parse escaped chunk. Ambiguous conditions are treated as
           literal strings. */

        *chunk_len = 1;

        if (*len < 2)
            goto parse_normal;

        switch (begin[1]) {
            case 'a': *scratch = '\a'; break;
            case 'b': *scratch = '\b'; break;
            case 'e': *scratch = 27; break;
            case 'f': *scratch = '\f'; break;
            case 'l': *scratch = '\n'; break;
            case 'n': *scratch = '\n'; break;
            case 'q': /* fall through */
            case '"': *scratch = '"'; break;
            case 'r': *scratch = '\r'; break;
            case 't': *scratch = '\t'; break;
            case 'v': *scratch = '\v'; break;
            case 'z': *scratch = '\0'; break;
            case '\\': *scratch = '\\'; break;
            case 'm':
                scratch[0] = '\r';
                scratch[1] = '\n';
                *chunk_len = 2;
                break;
            case 'x': {
                char l, r;

                if (*len < 4)
                    goto parse_normal;

                l = tolower(begin[2]);
                r = tolower(begin[3]);

                if (!((l >= '0' && l <= '9') || ((l >= 'a' && l <= 'f'))) ||
                        !((r >= '0' && r <= '9') || ((r >= 'a' && r <= 'f'))))
                    goto parse_normal;

                l = l <= '9' ? l - '0' : l - 'a' + 10;
                r = r <= '9' ? r - '0' : r - 'a' + 10;

                *scratch = l * 16 + r;

                *str += 2;
                *len -= 2;

                break;
            }
            default:
                /* Ambiguous condition. Treat as literal. */
                goto parse_normal;
        }

        *len -= 2;
        *str += 2;

        return scratch;
    }

parse_normal:

    /* Parse normal chunk. */
    while (*chunk_len < *len && begin[*chunk_len] != '\\')
        ++*chunk_len;

    *len -= *chunk_len;
    *str += *chunk_len;

    return begin;
}

int e4__mem_strncasecmp(const char *left, const char *right, e4__usize n)
{
    while (n-- > 0 && tolower(*left++) == tolower(*right++))
        if (!left[-1])
            return 0;

    return !++n ? 0 : tolower(left[-1]) - tolower(right[-1]);
}

e4__usize e4__mem_number(const char *buf, e4__usize length, e4__u8 base,
        e4__u8 flags, e4__usize *out)
{
    register e4__bool number;
    register e4__bool negate = 0;
    register e4__usize result = 0;
    register const char *start = buf;

    /* XXX: Ambiguous behavior. This implementation clamps to the
       acceptable range. */
    if (base < 2) base = 2;
    if (base > 36) base = 36;

    if ((flags & e4__F_CHAR_LITERAL) && length > 2 && buf[0] == '\'' &&
            buf[2] == '\'') {
        *out = buf[1];
        return 3;
    }

    if ((flags & e4__F_NEG_PREFIX) && *buf == '-') {
        negate = 1;
        ++buf;
    }

    if ((flags & e4__F_BASE_PREFIX) && length > 1)
        switch (*buf++) {
            case '0':
                if (length > 2)
                    switch (*buf++) {
                        case 'x': case 'X': base = 16; break;
                        case 'o': case 'O': base = 8; break;
                        case 'b': case 'B': base = 2; break;
                        default: buf -= 2; break;
                    }
                else
                    buf -= 1;
                break;
            case '$': base = 16; break;
            case '#': base = 10; break;
            case '%': base = 2; break;
            default: --buf; break;
        }

    for (number = 0; buf - start < length; number = 1, ++buf) {
        register e4__usize d;
        register const char c = tolower(*buf);

        if (isdigit(c))
            d = c - '0';
        else if (c >= 'a' && c <= 'z')
            d = c - 'a' + 10;
        else
            break;

        if (d >= base)
            break;

        result *= base;
        result += d;
    }

    if (number) {
        if (negate)
            result = e4__USIZE_NEGATE(result);
        *out = result;
        return buf - start;
    }

    return 0;
}

e4__usize e4__mem_parse(const char *buf, char delim, e4__usize sz,
        e4__usize flags, const char **out)
{
    /* out is always set. */
    register e4__usize length;

    if (flags & e4__F_SKIP_LEADING)
        while (sz && *buf == delim)
            ++buf, --sz;

    *out = buf;

    /* FIXME: Consider wrapping the end on newline behavior around some
       kind of flag. */
    length = 0;
    while (sz && *buf != delim && *buf != '\n') {
        ++buf, --sz, ++length;

        /* XXX: For the purposes of this implementation, only single
           characters may be escaped and any character may be escaped.
           \ may not be used for any other purpose, including as
           delimiter, in such strings. */
        if ((flags & e4__F_IGNORE_ESC) && sz && buf[-1] == '\\')
            ++buf, --sz, ++length;
    }

    return length;
}

e4__usize e4__mem_pno_digit(char **buf_end, e4__u8 base, struct e4__double *ud)
{
    register e4__usize res;
    e4__usize digit;

    /* XXX: Ambiguous behavior. This implementation clamps to the
       acceptable range. */
    if (base < 2) base = 2;
    else if (base > 36) base = 36;

    if ((res = e4__double_div(*ud, base, 0, ud, &digit)))
        return res;

    **buf_end = digit < 10 ? '0' + digit : 'A' + digit - 10;
    --*buf_end;

    return e4__E_OK;
}

e4__usize e4__mem_pno_digits(char **buf_end, e4__usize len, e4__u8 base,
        e4__u8 flags, struct e4__double *ud)
{
    register e4__usize res = e4__E_OK;
    register e4__bool negative = 0;

    /* This just works with the most negative number since NEGATE ends
       up doing nothing to change the number itself, but we'll prepend
       a minus sign to the number since we know it had its sign bit set
       prior to this operation. */
    if ((flags & e4__F_SIGNED) && (negative = e4__USIZE_IS_NEGATIVE(ud->high)))
        *ud = e4__double_negate(*ud);

    do {
        if (!len) {
            res = e4__E_PNOOVERFLOW;
            break;
        }

        if ((res = e4__mem_pno_digit(buf_end, base, ud)))
            break;

        --len;
    } while (ud->high || ud->low);

    if (negative) {
        if (!len)
            res = e4__E_PNOOVERFLOW;
        else
            e4__mem_pno_hold(buf_end, '-');
    }

    return res;
}

void e4__mem_pno_hold(char **buf_end, char c)
{
    **buf_end = c;
    --*buf_end;
}

void e4__mem_pno_holds(char **buf_end, const char *s, e4__usize len)
{
    *buf_end -= len;
    memmove(*buf_end + 1, s, len);
}
