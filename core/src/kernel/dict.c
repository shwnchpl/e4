#include "e4.h"
#include "e4-internal.h"

#include <string.h>

void e4__dict_entry(struct e4__task *task, const char *name, e4__u8 nbytes,
        e4__u8 flags, e4__code_ptr code, void *user)
{
    /* FIXME: Check that we aren't out of space and return some kind of
       error code? */
    const e4__usize wrote = e4__mem_dict_entry(task->here, task->dict, name,
            nbytes, flags, code, user);
    task->dict = (struct e4__dict_header *)task->here;
    task->here += wrote / sizeof(e4__cell);
}

e4__usize e4__dict_forget(struct e4__task *task, const char *name,
        e4__u8 nbytes)
{
    struct e4__dict_header *const header = e4__mem_dict_lookup(task->dict,
            name, nbytes);

    /* XXX: Ambiguous condition. Can't forget a builtin. */
    if (!header || (header->flags & e4__F_BUILTIN))
        return e4__E_INVFORGET;

    task->dict = header->link;
    task->here = (e4__cell)header;

    return e4__E_OK;
}

/* Look up a dictionary entry. Will not check the top entry if it
   has the e4__F_COMPILING flag set. */
struct e4__dict_header* e4__dict_lookup(struct e4__task *task,
        const char *name, e4__u8 nbytes)
{
    return e4__mem_dict_lookup(task->dict->flags & e4__F_COMPILING ?
            task->dict->link : task->dict, name, nbytes);
}
