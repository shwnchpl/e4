#include "e4.h"
#include <string.h>

/* TODO: Create some kind of friendly user facing C API that
   can actually create a dictionary entry in the right place and update
   here, etc. Returns the number of cells of the dictionary entry. */
e4__usize e4__dict_entry(void *here, struct e4__dict_header *prev,
        const char *name, e4__u8 nbytes, e4__u8 flags, e4__code_ptr code,
        void *user)
{
    /* FIXME: Allow passing -1 for strlen of name. */
    register struct e4__dict_header *header = here;
    register e4__usize sz;

    header->link = prev;
    header->flags = flags;
    header->nbytes = nbytes;
    header->name = (const char *)(&header[1]);

    memcpy(&header[1], name, nbytes);

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

struct e4__dict_header* e4__dict_lookup(struct e4__dict_header *dict,
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
