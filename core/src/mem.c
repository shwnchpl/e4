#include "e4.h"

#include <string.h>
#include <ctype.h>

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
    /* FIXME: Clean this up. */
    if (sz % sizeof(void *))
        sz += (sizeof(void *) - (sz % sizeof(void *)));

    header->xt = (struct e4__execute_token *)((e4__u8 *)here + sz);
    header->xt->user = user;
    header->xt->code = code;

    sz += sizeof(*header->xt) - sizeof(header->xt->data);

    /* XXX: Really this should be impossible. */
    if (sz % sizeof(void *))
        sz += (sizeof(void *) - (sz % sizeof(void *)));

    return sz;
}

struct e4__dict_header* e4__mem_dict_lookup(struct e4__dict_header *dict,
        const char *name, e4__u8 nbytes)
{
    while (dict) {
        if (nbytes == dict->nbytes &&
                !e4__mem_strncasecmp(dict->name, name, nbytes))
            return dict;
        dict = dict->link;
    }

    return NULL;
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
    register e4__bool negate;
    register e4__usize result = 0;
    register const char *start = buf;

    /* XXX: Ambiguous behavior. This implementation clamps to the
       acceptable range. */
    if (base < 2) base = 2;
    if (base > 36) base = 36;

    if (flags & e4__F_CHAR_LITERAL && length > 2 && buf[0] == '\'' &&
            buf[2] == '\'') {
        *out = buf[1];
        return 3;
    }

    if (flags & e4__F_NEG_PREFIX && *buf == '-') {
        negate = 1;
        ++buf;
    }

    if (flags & e4__F_BASE_PREFIX)
        switch (*buf++) {
            case '0':
                switch (*buf++) {
                    case 'x': case 'X': base = 16; break;
                    case 'o': case 'O': base = 8; break;
                    case 'b': case 'B': base = 2; break;
                    default: buf -= 2; break;
                }
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

e4__usize e4__mem_parse(const char *buf, char delim, e4__usize size,
        e4__usize flags, const char **out)
{
    /* out is always set. */
    register e4__usize length;

    if (flags & e4__F_SKIP_LEADING)
        while (size && *buf == delim)
            ++buf, --size;

    *out = buf;

    /* FIXME: Consider wrapping the end on newline behavior around some
       kind of flag. */
    length = 0;
    while (size && *buf != delim && *buf != '\n')
        ++buf, --size, ++length;

    return length;
}
