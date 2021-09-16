#define _POSIX_C_SOURCE 199309L
#include "e4.h"

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/poll.h>
#include <unistd.h>

#include <histedit.h>

struct repl_data {
    EditLine *el;
    History *hist;
    HistEvent ev;
    struct e4__task *task;
    sigset_t ss;
    const struct e4__dict_header *sug;
    char sug_prefix[256];
    short sug_prefix_len;
    e4__bool winch;
    e4__bool interrupted;
    e4__bool no_jump;
};

static char* repl_prompt(EditLine *el)
{
    return "";
}

static void repl_sighandler_thenunblock(struct e4__task *task, e4__cell user);

static void repl_sighandler(int sig, siginfo_t *si, void *ucontext)
{
    static struct repl_data *rd = NULL;

    /* Technically, it is undefined behavior to longjmp out of a
       nested signal handler, which is a signal handler that runs while
       another signal handler is running. With only this signal handler,
       that should never happen.

       The only other concern is that it's undefined behavior to
       longjmp out of a signal handler that has interrupted a non-async
       safe function (ie. a function that makes use of static state in
       some way or a standard library function that is not async-safe;
       see https://man7.org/linux/man-pages/man7/signal-safety.7.html).
       Fortunately, e4 itself does not contain any non-async-safe code.
       Unfortunately, libedit does. For that reason, whenever executing
       libedit code, signals are either blocked or no_jump is set. When
       no_jump is set, and SIGSEGV is received, the program exits with
       EXIT_FAILURE. When no_jump is set and SIGINT is received, the
       interrupted flag is set. This is checked after executing libedit
       code where no_jump has been set and if necessary the error is
       safely propagated from that context.

       This is a little bit of a hack, but it is safe. */

    switch (sig) {
        case SIGUSR1:
            rd = si->si_ptr;
            return;
        case SIGWINCH:
            el_resize(rd->el);
            rd->winch = 1;
            return;
        case SIGSEGV:
            if (rd && !rd->no_jump)
                e4__exception_throw_then(rd->task, e4__E_INVMEMADDR,
                        repl_sighandler_thenunblock, rd);
            break;
        case SIGINT:
            if (rd) {
                if (rd->no_jump) {
                    rd->interrupted = 1;
                    return;
                }

                e4__exception_throw_then(rd->task, e4__E_USERINTERRUPT,
                        repl_sighandler_thenunblock, rd);
            }
            break;
        default:
            break;
    }

    /* Either exceptions weren't enabled or we couldn't jump. This is an
       error state. Simply exit the program. */
    exit(EXIT_FAILURE);
}

static void repl_sighandler_thenunblock(struct e4__task *task, e4__cell user)
{
    struct repl_data *rd = (struct repl_data *)e4__DEREF(user);
    sigprocmask(SIG_UNBLOCK, &rd->ss, NULL);
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

static int repl_wgetc(EditLine *el, wchar_t *wc)
{
    int res;
    void *client_data;
    struct repl_data *rd;
    char c = 0;

    /* The libedit manual strongly recommends not implementing your own
       wgetc function, however we need SIGWINCH to work correctly
       without libedit's builtin signal handlers and tab-completion is
       a nice feature to be able to implement here. Furthermore, only
       ASCII input is handled currently anyhow, so the implementation
       isn't actually terribly complicated. */

    el_get(el, EL_CLIENTDATA, &client_data);
    rd = (struct repl_data *)client_data;

    rd->winch = 0;
    while ((res = read(STDIN_FILENO, &c, 1)) == -1) {
        if (!rd->winch)
            break;
        rd->winch = 0;
    }

    *wc = c;

    if (res == 1 && *wc != '\x0e' && *wc != '\t')
        rd->sug = NULL;

    return res;
}

static e4__usize repl_accept(void *user, char *buf, e4__usize *n)
{
    int res = 0;
    struct repl_data *rd = user;
    const char *ebuf;
    e4__usize num;

    /* Always set suggestion state before calling el_gets, just in case
       we were interrupted while getting input. */
    rd->sug = NULL;

    /* Do not jump out of signal handlers when executing libedit code
       as doing so is not always async safe. */
    rd->interrupted = 0;
    rd->no_jump = 1;
    ebuf = el_gets(rd->el, &res);
    rd->no_jump = 0;

    if (rd->interrupted)
        return e4__E_USERINTERRUPT;

    if (res < 0)
        return e4__E_FAILURE;

    num = res;

    if (num > *n)
        num = *n;

    if (buf == NULL || num == 0) {
        /* Simply display a newline and return e4__E_BYE, which should
           tear everything down gracefully. */
        putchar('\n');
        return e4__E_BYE;
    } else {
        memcpy(buf, ebuf, num);
        *n = num;

        /* Since calling history itself is not a blocking operation,
           simply block signals for the call's duration. */
        sigprocmask(SIG_BLOCK, &rd->ss, NULL);
        res = history(rd->hist, &rd->ev, H_ENTER, ebuf);
        sigprocmask(SIG_UNBLOCK, &rd->ss, NULL);

        if (res < 0)
            return e4__E_FAILURE;
    }

    return e4__E_OK;
}

static e4__usize repl_key(void *user, char *buf)
{
    struct repl_data *rd = user;
    int res;

    rd->no_jump = 1;
    rd->interrupted = 0;
    res = el_getc(rd->el, buf);
    rd->no_jump = 0;

    if (rd->interrupted)
        return e4__E_USERINTERRUPT;

    if (res < 1)
        return e4__E_FAILURE;

    return e4__E_OK;
}

static e4__usize repl_keyq(void *user, e4__usize *bflag)
{
    struct pollfd pfd;
    int pres;
    struct repl_data *rd = user;

    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;

    /* Set terminal to raw mode. By default, libedit keeps the terminal
       in cooked mode and we don't do anything to change that, so it is
       safe to simply assume we are in cooked mode when this function
       is being called and we should restore the terminal to cooked mode
       before we return. */
    el_set(rd->el, EL_PREP_TERM, 1);
    pres = poll(&pfd, 1, 0);
    el_set(rd->el, EL_PREP_TERM, 0);

    if (pres < 0)
        return e4__E_FAILURE;
    else if (!pres || !(pfd.revents & POLLIN))
        *bflag = e4__BF_FALSE;
    else
        *bflag = e4__BF_TRUE;

    return e4__E_OK;
}

static e4__usize repl_type(void *user, const char *buf, e4__usize n)
{
    while (n > 0) {
        const ssize_t wrote = write(STDOUT_FILENO, buf, n);
        if (wrote < 0)
            return e4__E_FAILURE;
        n -= wrote;
    }

    return e4__E_OK;
}

static e4__usize repl_unixtime(void *user, e4__usize *t)
{
    e4__ASSERT(sizeof(time_t) <= sizeof(e4__usize));

    time_t ut;

    ut = time(NULL);
    if (ut < 0)
        return e4__E_FAILURE;

    *t = (e4__usize)ut;

    return e4__E_OK;
}

int main(int argc, char **argv)
{
    static char task_buffer[4096];
    struct e4__task *task;
    union sigval sv;
    struct sigaction sa = {0};
    struct sigaction old_sa[3];
    struct repl_data rd = {0};
    struct e4__io_func io_src = {
        NULL,
        repl_accept,
        repl_key,
        repl_type,
        repl_keyq,
        repl_unixtime,
    };
    EditLine *el = NULL;
    History *hist;

    hist = history_init();
    history(hist, &rd.ev, H_SETSIZE, 100);

    el = el_init(*argv, stdin, stdout, stderr);

    el_set(el, EL_EDITOR, "vi");
    el_set(el, EL_SIGNAL, 0);
    el_set(el, EL_HIST, history, hist);
    el_set(el, EL_CLIENTDATA, &rd);

    el_set(el, EL_PROMPT, repl_prompt);
    el_set(el, EL_ADDFN, "e4-suggest", "suggest an e4 word", repl_suggest);
    el_set(el, EL_GETCFN, repl_wgetc);

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

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = repl_sighandler;
    sigemptyset(&sa.sa_mask);

    /* XXX: Hack to push context into the signal handler. */
    sigaction(SIGUSR1, &sa, &old_sa[0]);
    sv.sival_ptr = &rd;
    sigqueue(getpid(), SIGUSR1, sv);
    sigaction(SIGUSR1, &old_sa[0], NULL);

    /* All signals we're handling with the handler function when our
       handler is invoked. */
    sigemptyset(&rd.ss);
    sigaddset(&rd.ss, SIGINT);
    sigaddset(&rd.ss, SIGSEGV);
    sigaddset(&rd.ss, SIGWINCH);
    sa.sa_mask = rd.ss;

    sigaction(SIGINT, &sa, &old_sa[0]);
    sigaction(SIGSEGV, &sa, &old_sa[1]);
    sigaction(SIGWINCH, &sa, &old_sa[2]);

    e4__evaluate_quit(task);

    sigaction(SIGWINCH, &old_sa[2], NULL);
    sigaction(SIGSEGV, &old_sa[1], NULL);
    sigaction(SIGINT, &old_sa[0], NULL);

    history_end(hist);
    el_end(el);

    return 0;
}
