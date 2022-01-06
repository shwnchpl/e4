#include "e4.h"

#if defined(e4__INCLUDE_POSIX_HOOKS)

    #if defined(e4__INCLUDE_FACILITY_EXT)

        #include <time.h>

        e4__usize e4__posix_ms(void *user, e4__usize ms)
        {
            struct timespec ts;

            ts.tv_sec = ms / 1000;
            ts.tv_nsec = (ms % 1000) * 1000000;

            return nanosleep(&ts, NULL) ? e4__E_FAILURE : e4__E_OK;
        }

        e4__usize e4__posix_unixtime(void *user, e4__usize *t)
        {
            e4__ASSERT(sizeof(time_t) <= sizeof(e4__usize));

            time_t ut;

            ut = time(NULL);
            if (ut < 0)
                return e4__E_FAILURE;

            *t = (e4__usize)ut;

            return e4__E_OK;
        }

    #endif /* defined(e4__INCLUDE_FACILITY_EXT) */

    #if defined(e4__INCLUDE_FILE) || defined(e4__INCLUDE_FILE_EXT)

        #include <errno.h>
        #include <fcntl.h>
        #include <string.h>
        #include <unistd.h>

        e4__usize e4__posix_file_close(void *user, e4__usize fd,
                e4__usize *ior)
        {
            const int res = close(fd);

            if (res) {
                *ior = errno;
                return e4__E_CLOSE_FILE;
            }

            return e4__E_OK;
        }

        e4__usize e4__posix_file_open(void *user, const char *path,
                e4__usize sz, e4__usize perm, e4__usize *fd, e4__usize *ior)
        {
            char pathz[e4__POSIX_PATH_MAX];
            int res, flags;

            switch (perm & e4__F_READWRITE) {
                case e4__F_READWRITE:
                    flags = O_RDWR;
                    break;
                case e4__F_READ:
                    flags = O_RDONLY;
                    break;
                case e4__F_WRITE:
                    flags = O_WRONLY;
                    break;
                default:
                    return e4__E_INVNUMARG;
            }

            /* XXX: It's a bit unfortunate to use a large chunk of stack
               space (4K by default) in this way, however in most cases
               it should be sufficient and it's better than resorting to
               dynamic memory. On a constrained embedded system, a
               similar technique with a far smaller path limit could
               safely be employed. */
            if (path[sz - 1]) {
                if (sz >= e4__POSIX_PATH_MAX)
                    return e4__E_LONGFILEPATH;
                memcpy(pathz, path, sz);
                pathz[sz] = '\0';
                path = pathz;
            }

            res = open(path, flags);
            if (!res) {
                /* FIXME: In theory, this could return 0 if STDIN has been
                   closed. This would cause issues elsewhere, so we treat
                   it as a failure here, but that also isn't correct. */
                return e4__E_BUG;
            } else if (res < 0) {
                *ior = errno;
                return e4__E_OPEN_FILE;
            }

            *fd = res;

            return e4__E_OK;
        }

        e4__usize e4__posix_file_read(void *user, e4__usize fd, char *buf,
                e4__usize *n, e4__usize *ior)
        {
            const int count = read(fd, buf, *n);

            if (count < 0) {
                *ior = errno;
                return e4__E_READ_FILE;
            }

            *n = count;

            return count ? e4__E_OK : e4__E_EOF;
        }

    #endif /* defined(e4__INCLUDE_FILE) || defined(e4__INCLUDE_FILE_EXT) */

    #if defined(e4__INCLUDE_FILE)

        e4__usize e4__posix_file_create(void *user, const char *path,
                e4__usize sz, e4__usize perm, e4__usize *fd, e4__usize *ior)
        {
            /* FIXME: STUB! */
            return e4__E_UNSUPPORTED;
        }

        e4__usize e4__posix_file_delete(void *user, const char *path,
                e4__usize sz, e4__usize *ior)
        {
            /* FIXME: STUB! */
            return e4__E_UNSUPPORTED;
        }

        e4__usize e4__posix_file_position(void *user, e4__usize fd,
                e4__usize *pos, e4__usize *ior)
        {
            /* FIXME: STUB! */
            return e4__E_UNSUPPORTED;
        }

        e4__usize e4__posix_file_reposition(void *user, e4__usize fd,
                e4__usize pos, e4__usize *ior)
        {
            /* FIXME: STUB! */
            return e4__E_UNSUPPORTED;
        }

        e4__usize e4__posix_file_resize(void *user, e4__usize fd, e4__usize sz,
                e4__usize *ior)
        {
            /* FIXME: STUB! */
            return e4__E_UNSUPPORTED;
        }

        e4__usize e4__posix_file_size(void *user, e4__usize fd, e4__usize *sz,
                e4__usize *ior)
        {
            /* FIXME: STUB! */
            return e4__E_UNSUPPORTED;
        }

    #endif /* defined(e4__INCLUDE_FILE) */

    #if defined(e4__INCLUDE_FILE_EXT)

        e4__usize e4__posix_file_flush(void *user, e4__usize fd,
                e4__usize *ior)
        {
            /* FIXME: STUB! */
            return e4__E_UNSUPPORTED;
        }

        e4__usize e4__posix_file_rename(void *user, const char *old_path,
                e4__usize old_sz, const char *new_path, e4__usize new_sz,
                e4__usize *ior)
        {
            /* FIXME: STUB! */
            return e4__E_UNSUPPORTED;
        }

    #endif /* defined(e4__INCLUDE_FILE_EXT) */

#endif /* defined(e4__INCLUDE_POSIX_HOOKS) */
