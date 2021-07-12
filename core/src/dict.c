#include "e4-debug.h"
#include <string.h>

void* e4__dict_entry(void *here, struct e4__dict_header *prev, char *name,
        unsigned short nbytes, void *code, void *user, unsigned short flags)
{
    register struct e4__dict_header *header = here;
    register unsigned long sz;

    header->link = prev;
    header->flags = flags;
    header->nbytes = nbytes;
    memcpy(header->name, name, nbytes);

    sz = sizeof(*header) - sizeof(header->name) + nbytes;

    /* Align size. */
    /* FIXME: Clean this up. */
    if (sz % sizeof(void*))
        sz += (sizeof(void*) - (sz % sizeof(void*)));

    header->footer = (struct e4__dict_footer*)((char*)here + sz);
    header->footer->user = user;
    header->footer->code = code;

    return here + sz + sizeof(*header->footer) - sizeof(header->footer->data);
}

struct e4__dict_header* e4__dict_lookup(struct e4__dict_header *dict,
        char *name, unsigned short nbytes)
{
    while (dict) {
        if (nbytes == dict->nbytes && !memcmp(dict->name, name, nbytes))
            return dict;
        dict = dict->link;
    }

    return NULL;
}
