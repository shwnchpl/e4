#include "e4.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <histedit.h>

struct repl_data {
    EditLine *el;
    History *hist;
    HistEvent ev;
    struct e4__task *task;
    const struct e4__dict_header *sug;
    char sug_prefix[256];
    short sug_prefix_len;
};

static char* repl_prompt(EditLine *el)
{
    return "";
}

static int repl_wgetc_wrapper(EditLine *el, wchar_t *wc)
{
    int res;
    void *client_data;
    struct repl_data *rd;

    el_get(el, EL_CLIENTDATA, &client_data);
    rd = (struct repl_data *)client_data;

    /* Restore original handler and call el_wgetc again. */
    el_set(el, EL_GETCFN, EL_BUILTIN_GETCFN);
    res = el_wgetc(el, wc);

    /* Restore wrapper. */
    el_set(el, EL_GETCFN, repl_wgetc_wrapper);

    if (res == 1 && *wc != '\x0e' && *wc != '\t')
        rd->sug = NULL;

    return res;
}

static unsigned char repl_suggest(EditLine *el, int ch)
{
    void *client_data;
    struct repl_data *rd;
    const LineInfo *li = el_line(el);
    const char *word;
    int len;

    el_get(el, EL_CLIENTDATA, &client_data);
    rd = (struct repl_data *)client_data;

    for (word = rd->sug ? li->cursor - 1 : li->cursor;
            word > li->buffer && word[-1] != ' '; --word)
        ;

    len = li->cursor - word;

    if (!rd->sug) {
        if (len > sizeof(rd->sug_prefix) - 1)
            return CC_REDISPLAY;
        memcpy(rd->sug_prefix, word, len);
        rd->sug_prefix[len] = '\0';
        rd->sug_prefix_len = len;
    }

    el_deletestr(el, len);

    rd->sug = e4__dict_suggest(rd->task, rd->sug, rd->sug_prefix,
            rd->sug_prefix_len);
    if (rd->sug) {
        char scratch[256] = {0};
        len = rd->sug->nbytes > 255 ? 255 : rd->sug->nbytes;
        memcpy(scratch, rd->sug->name, len);
        scratch[len] = '\0';
        el_insertstr(el, scratch);
        el_insertstr(el, " ");
    } else
        el_insertstr(el, rd->sug_prefix);

    return CC_REDISPLAY;
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
    struct repl_data rd = {0};
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
    el_set(el, EL_CLIENTDATA, &rd);

    el_set(el, EL_PROMPT, repl_prompt);
    el_set(el, EL_GETCFN, repl_wgetc_wrapper);
    el_set(el, EL_ADDFN, "e4-suggest", "suggest an e4 word", repl_suggest);

    /* Default bindings. */
    el_set(el, EL_BIND, "\t", "e4-suggest", NULL);
    el_set(el, EL_BIND, "^N", "e4-suggest", NULL);

    el_source(el, NULL);

    /* FIXME: Also source ~/.e4-editrc to allow for e4 specific
       overrides. For example, to disable tab completion by macro
       binding the tab key to two or four spaces. */

    rd.el = el;
    rd.hist = hist;

    io_src.user = &rd;
    task = e4__task_create(task_buffer, sizeof(task_buffer));
    rd.task = task;
    e4__task_io_init(task, &io_src);

    e4__evaluate_quit(task);

    history_end(hist);
    el_end(el);

    return 0;
}
