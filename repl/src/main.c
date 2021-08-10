#include "e4.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <histedit.h>

struct repl_data {
    EditLine *el;
    History *hist;
    HistEvent ev;
};

static char* repl_prompt(EditLine *el)
{
    return "";
}

static e4__usize repl_accept(void *user, char *buf, e4__usize *n)
{
    int d = 0;
    struct repl_data *rd = user;
    const char *ebuf = el_gets(rd->el, &d);
    e4__usize num = d;

    if (num > *n)
        num = *n;

    if (buf == NULL || num == 0) {
        memcpy(buf, "quit", 4);
        *n = 4;
        el_push(rd->el, "bye\n");
    } else {
        memcpy(buf, ebuf, num);
        *n = num;
        history(rd->hist, &rd->ev, H_ENTER, ebuf);
    }

    return e4__E_OK;
}

static e4__usize repl_key(void *user, char *buf)
{
    struct repl_data *rd = user;
    return el_getc(rd->el, buf) != -1 ? e4__E_OK : e4__E_FAILURE;
}

static e4__usize repl_type(void *user, const char *buf, e4__usize n)
{
    fprintf(stdout, "%.*s", (int)n, buf);
    return e4__E_OK;
}

int main(int argc, char **argv)
{
    static char task_buffer[4096];
    struct repl_data rd;
    struct e4__task *task;
    struct e4__io_func io_src = {
        NULL,
        repl_accept,
        repl_key,
        repl_type
    };
    EditLine *el = NULL;
    History *hist;

    hist = history_init();
    history(hist, &rd.ev, H_SETSIZE, 100);

    el = el_init(*argv, stdin, stdout, stderr);

    el_set(el, EL_EDITOR, "vi");
    el_set(el, EL_SIGNAL, 1);
    el_set(el, EL_HIST, history, hist);
    el_set(el, EL_PROMPT, repl_prompt);

    el_source(el, NULL);

    rd.el = el;
    rd.hist = hist;

    io_src.user = &rd;
    task = e4__task_create(task_buffer, sizeof(task_buffer));
    e4__task_io_init(task, &io_src);

    e4__evaluate_quit(task);

    history_end(hist);
    el_end(el);

    return 0;
}
