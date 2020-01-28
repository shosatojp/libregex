#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

enum MATCH_STATE toggle_match_state(enum MATCH_STATE state) {
    return (MS_MATCHED + MS_FAILED) - state;
}

enum MATCH_STATE consume_char(const char** ptr, struct matcher* m) {
    consumer_debug(m, "consumer_char('%c')", m->c);
    if (**ptr == m->c ||
        (m->ignore_case && lower_case(**ptr) == lower_case(m->c))) {
        (*ptr)++;
        return MS_MATCHED;
    } else {
        return MS_FAILED;
    }
}

enum MATCH_STATE consume_span(const char** ptr, struct matcher* m) {
    consumer_debug(m, "consume_span('%c', '%c')", m->c, m->c2);
    char lower;
    if (m->c <= **ptr && **ptr <= m->c2 ||
        (m->ignore_case && lower_case(m->c) <= (lower = lower_case(**ptr)) && lower <= lower_case(m->c2))) {
        (*ptr)++;
        return MS_MATCHED;
    } else {
        return MS_FAILED;
    }
}

enum MATCH_STATE consume_times(const char** ptr, struct matcher* m) {
    consumer_debug(m, "consume_times(%d, %d)", m->u, m->v);
    int count = 0;
    while (count < m->v) {
        struct matcher* m0 = (struct matcher*)array_at(m->ms, 0);
        if (m0->fn(ptr, m0) == MS_MATCHED) {
            count++;
        } else {
            break;
        }
    }
    if (m->u < 0) {
        if (count == m->v) {
            return MS_MATCHED;
        } else {
            return MS_FAILED;
        }
    } else {
        if ((count == 0 && m->u == 0) || (m->u <= count)) {
            return MS_MATCHED;
        } else {
            return MS_FAILED;
        }
    }
}

enum MATCH_STATE consume_plus(const char** ptr, struct matcher* m) {
    consumer_debug(m, "consumer_plus()");
    copy_constants(m);
    const char* init = *ptr;
    struct matcher* m0 = (struct matcher*)array_at(m->ms, 0);
    while (**ptr) {
        const char* _ptr = *ptr;
        enum MATCH_STATE result = m0->fn(&_ptr, m0);
        if (result == MS_MATCHED) {
            (*ptr)++;
            if (m->non_greedy) return MS_MATCHED;
        } else {
            break;
        }
    }
    return (m->type != MT_PLUS || init != *ptr) ? MS_MATCHED : MS_FAILED;
}

enum MATCH_STATE consume_or(const char** ptr, struct matcher* m) {
    consumer_debug(m, "consumer_or()");
    const char* init = *ptr;
    for (int i = 0; i < m->ms->length; i++) {
        *ptr = init;
        struct matcher* mi = (struct matcher*)array_at(m->ms, i);
        if (mi->fn(ptr, mi) == MS_MATCHED) {
            if (m->type == MT_OR && !m->nocapture)
                m->captured = capture_new(init, *ptr - 1);
            return MS_MATCHED;
        }
    }
    return MS_FAILED;
}

/**
 * msの中に一致しない１文字にマッチする
 */
enum MATCH_STATE consume_nor(const char** ptr, struct matcher* m) {
    consumer_debug(m, "consumer_nor()");
    const char* init = *ptr;
    for (int i = 0; i < m->ms->length; i++) {
        *ptr = init;
        struct matcher* mi = (struct matcher*)array_at(m->ms, i);
        if (mi->fn(ptr, mi) == MS_MATCHED) {
            return MS_FAILED;
        }
    }
    (*ptr)++;
    return MS_MATCHED;
}

enum MATCH_STATE consume_head(const char** ptr, struct matcher* m) {
    consumer_debug(m, "consumer_head(%c)", **ptr);
    char c;
    if (*ptr == m->head || (m->multi_line && (c = *(*ptr - 1)) &&
                            (c == '\n' ||
                             c == '\r'))) {
        return MS_MATCHED;
    } else {
        return MS_FAILED;
    }
}

enum MATCH_STATE consume_tail(const char** ptr, struct matcher* m) {
    consumer_debug(m, "consumer_tail(%c)", **ptr);
    if (*ptr == m->tail || **ptr == '\n' || **ptr == '\r') {
        return MS_MATCHED;
    } else {
        return MS_FAILED;
    }
}

enum MATCH_STATE consume_any(const char** ptr, struct matcher* m) {
    consumer_debug(m, "consumer_any()");
    if (**ptr && (m->dot_all || **ptr != '\n')) {
        (*ptr)++;
        return MS_MATCHED;
    } else {
        return MS_FAILED;
    }
}

enum MATCH_STATE consume_seq(const char** ptr, struct matcher* m) {
    consumer_debug(m, "consumer_seq()");
    for (int i = 0; i < m->ms->length; i++) {
        struct matcher* mi = (struct matcher*)array_at(m->ms, i);
        if (mi->fn(ptr, mi) == MS_FAILED) {
            consumer_debug(m, "└(re = %d, char = %c)", MS_FAILED, **ptr);
            return MS_FAILED;
        } else {
            consumer_debug(m, "└(re = %d, char = %c)", MS_MATCHED, **ptr);
        }
    }
    consumer_debug(m, "└(re = %d)", MS_MATCHED);
    return MS_MATCHED;
}

enum MATCH_STATE root_executor(const char** ptr, struct matcher* m) {
    consumer_debug(m, "root_executor()");

    /* set constants */
    m->head = m->head ? m->head : *ptr;
    m->tail = m->head + strlen(m->head);

    /* copy constants */
    copy_constants(m);

    /* execute */
    return consume_seq(ptr, m);
}

enum MATCH_STATE consume_not(const char** ptr, struct matcher* m) {
    consumer_debug(m, "consumer_not()");
    return toggle_match_state(((struct matcher*)array_at(m->ms, 0))->fn(ptr, ((struct matcher*)array_at(m->ms, 0))));
}