#ifndef e4t_H_
#define e4t_H_

#include "e4.h"
#include <stdio.h>

extern unsigned long e4t__assert_attemptcount;
extern unsigned long e4t__assert_failcount;

#define e4t__ASSERT(e)  \
    do {    \
        ++e4t__assert_attemptcount; \
        if (!(e)) {   \
            fprintf(stderr, "Assert failed! (" __FILE__ ":%u) - "   \
                    "\"" #e "\" evaluated to 0\n", __LINE__); \
            ++e4t__assert_failcount;    \
        }   \
    } while (0)

#define e4t__ASSERT_OK(e)  \
    e4t__ASSERT_EQ((e), 0)

#define e4t__ASSERT_EQ(e, v)    \
    do {    \
        e4__usize e_ = (e); \
        e4__usize v_ = (v); \
        ++e4t__assert_attemptcount; \
        if (e_ != v_) {   \
            fprintf(stderr, "Assert failed! (" __FILE__ ":%u) - " \
                    "EXCEPTED: %ld; ACTUAL: %ld\n", __LINE__, v_, e_);  \
            ++e4t__assert_failcount;    \
        }   \
    } while (0)

/* XXX: This expects null terminated strings. */
#define e4t__ASSERT_MATCH(e, v) \
    do {    \
        const char *e_ = (e); \
        const char *v_ = (v); \
        ++e4t__assert_attemptcount; \
        if (e4__mem_strncasecmp(e_, v_, -1)) {   \
            fprintf(stderr, "Assert failed! (" __FILE__ ":%u) - " \
                    "EXCEPTED: \"%s\"; ACTUAL: \"%s\"\n", __LINE__, v_, e_);\
            ++e4t__assert_failcount;    \
        }   \
    } while (0)

#endif /* e4t_H_ */
