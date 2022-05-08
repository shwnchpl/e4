/* DLFCN word list definitions. */

/* WARNING: This file is *only* meant to be included by builtin.c.
   It depends on internal declarations and definitions provided by that
   file and it is *not* designed to work correctly in any other
   context. */

static void e4__builtin_DLCLOSE(struct e4__task *task, e4__cell user)
{
    register void *handle;
    register e4__usize io_res;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    handle = (void *)e4__stack_pop(task);
    if ((io_res = e4__io_dlclose(task, handle)))
        e4__exception_throw(task, io_res);

    e4__execute_ret(task);
}

static void e4__builtin_DLOPEN(struct e4__task *task, e4__cell user)
{
    register const char *path;
    register e4__usize io_res;
    void *handle;

    _e4__BUILTIN_EXPECT_DEPTH(task, 1);

    path = (const char *)e4__stack_pop(task);
    if ((io_res = e4__io_dlopen(task, path, &handle))) {
        e4__exception_throw(task, io_res);
        e4__execute_ret(task);
        return;
    }

    e4__stack_push(task, (e4__cell)handle);

    e4__execute_ret(task);
}

static void e4__builtin_DLSYM(struct e4__task *task, e4__cell user)
{
    register const char *sym_name;
    register void *handle;
    register e4__usize io_res;
    void *addr;

    _e4__BUILTIN_EXPECT_DEPTH(task, 2);

    handle = (void *)e4__stack_pop(task);
    sym_name = (char *)e4__stack_pop(task);

    if ((io_res = e4__io_dlsym(task, handle, sym_name, &addr))) {
        e4__exception_throw(task, io_res);
        e4__execute_ret(task);
        return;
    }

    e4__stack_push(task, (e4__cell)addr);

    e4__execute_ret(task);
}

/* vim: set ft=c: */
