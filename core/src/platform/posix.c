#include "e4.h"

#include <time.h>

#if defined(e4__INCLUDE_POSIX_HOOKS)

    #if defined(e4__INCLUDE_FACILITY_EXT)

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

#endif /* defined(e4__INCLUDE_POSIX_HOOKS) */
