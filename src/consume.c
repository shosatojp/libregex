#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

int consume_char(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_char('%c')", m->c);
    if (**ptr == m->c ||
        (m->ignore_case && lower_case(**ptr) == lower_case(m->c))) {
        (*ptr)++;
        return 1;
    } else {
        return RS_FAILED;
    }
}

int consume_span(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consume_span('%c', '%c')", m->c, m->c2);
    char lower;
    if (m->c <= **ptr && **ptr <= m->c2 ||
        (m->ignore_case && lower_case(m->c) <= (lower = lower_case(**ptr)) && lower <= lower_case(m->c2))) {
        (*ptr)++;
        return 1;
    } else {
        return RS_FAILED;
    }
}

int consume_times(const char** ptr, regex* m, regex_options* op) {
    return m->non_greedy
               ? consume_plus_non_greedy(ptr, m, op)
               : consume_plus_greedy(ptr, m, op);
}

int consume_plus(const char** ptr, regex* m, regex_options* op) {
    return m->non_greedy
               ? consume_plus_non_greedy(ptr, m, op)
               : consume_plus_greedy(ptr, m, op);
}

/* non greedy */
int consume_plus_non_greedy(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consume_plus_non_greedy()");

    regex* next = next_sibling(m);

    const char* init = *ptr;
    regex* m0 = array_at(m->ms, 0);
    int count = 0,
        u = m->u,
        v = m->v >= 0 ? m->v : (op->tail - op->head);
    consumer_debug(m, "u = %d,v = %d\n", u, v);
    while (**ptr && count < v) {
        const char* head = *ptr;
        const char* _ptr = *ptr;
        regex_state result = m0->fn(&_ptr, m0, op);
        if (result != RS_FAILED) {
            if (!next || next->fn(&_ptr, next, op) != RS_FAILED) {
                // 次の条件にマッチするまで
                (*ptr)++;
                break;
            } else {
                *ptr = head + 1;
                count++;
            }
        } else {
            break;
        }
    }

    int matched_len = (*ptr - init);
    switch (m->type) {
        case RT_STAR:
            return matched_len;
        case RT_PLUS:
            return matched_len > 0 ? matched_len : RS_FAILED;
        case RT_TIMES:
            if (u < 0) {
                return count == v ? matched_len : RS_FAILED;
            } else {
                return (u <= count && count <= v) ? matched_len : RS_FAILED;
            }
    }
}

/* greedy */
int consume_plus_greedy(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consume_plus_greedy()");

    regex* next = next_sibling(m);

    const char* init = *ptr;
    regex* m0 = array_at(m->ms, 0);

    int count = 0,
        u = m->u,
        v = m->v >= 0 ? m->v : ((op->tail - op->head) * 2 + 1);

    // forward match
    while (**ptr && count < v) {
        if (m0->fn(ptr, m0, op) != RS_FAILED) {
            count++;
        } else {
            break;
        }
    }

    // backward match
    bool _no_capture = op->no_capture;
    op->no_capture = true;
    while (init < *ptr) {
        const char *head = *ptr,
                   *_ptr = *ptr;
        if (!next || (next->fn(&_ptr, next, op) != RS_FAILED)) {
            *ptr = head;
            break;
        } else {
            *ptr = head - 1;
        }
    }
    op->no_capture = _no_capture;

    int matched_len = (*ptr - init);
    switch (m->type) {
        case RT_STAR:
            return matched_len;
        case RT_PLUS:
            return matched_len > 0 ? matched_len : RS_FAILED;
        case RT_TIMES:
            if (u < 0) {
                return count == v ? matched_len : RS_FAILED;
            } else {
                return (u <= count && count <= v) ? matched_len : RS_FAILED;
            }
    }
}

int consume_or(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_or()");
    const char* init = *ptr;
    for (int i = 0; i < m->ms->length; i++) {
        *ptr = init;
        regex* mi = array_at(m->ms, i);
        int matched_len;
        if ((matched_len = mi->fn(ptr, mi, op)) > -1) {
            if (m->type == RT_OR && !m->no_capture && !op->no_capture) {
                array_push(op->captured, regex_capture_new(init, *ptr - 1));
            }
            return matched_len;
        }
    }
    return RS_FAILED;
}

/**
 * msの中に一致しない１文字にマッチする
 */
int consume_nor(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_nor()");
    const char* init = *ptr;
    for (int i = 0; i < m->ms->length; i++) {
        *ptr = init;
        regex* mi = array_at(m->ms, i);
        int matched_len;
        if ((matched_len = mi->fn(ptr, mi, op)) > -1) {
            *ptr = init;
            return RS_FAILED;
        }
    }
    (*ptr)++;
    return 1;
}

int consume_head(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_head(%c)", **ptr);
    char c;
    if (*ptr == op->head) {
        return 0;
    } else if (m->multi_line && (c = *(*ptr - 1)) &&
               (c == '\n' ||
                c == '\r')) {
        return 0;
    } else {
        return RS_FAILED;
    }
}

int consume_tail(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_tail(%c)", **ptr);
    if (*ptr == op->tail) {
        return 0;
    } else if (**ptr == '\n' || **ptr == '\r') {
        return 0;
    } else {
        return RS_FAILED;
    }
}

int consume_any(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_any()");
    if (**ptr && (m->dot_all || **ptr != '\n')) {
        (*ptr)++;
        return 1;
    } else {
        return RS_FAILED;
    }
}

int consume_seq(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_seq()");
    int total_matched_len = 0;
    for (int i = 0; i < m->ms->length; i++) {
        regex* mi = array_at(m->ms, i);
        int matched_len;
        if ((matched_len = mi->fn(ptr, mi, op)) == RS_FAILED) {
            consumer_debug(m, "└(%d)", RS_FAILED);
            return RS_FAILED;
        } else {
            total_matched_len += matched_len;
            consumer_debug(m, "└(%d:'%c')", RS_MATCHED, **ptr);
        }
    }
    consumer_debug(m, "└(re = %d)", RS_MATCHED);
    return total_matched_len;
}

regex_state regex_match(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "match_regex()");

    /* set constants */
    op->head = op->head ? op->head : *ptr;
    op->tail = op->head + strlen(op->head);

    /* execute */
    return consume_seq(ptr, m, op);
}

// 使わない
// int consume_not(const char** ptr, regex* m, regex_options* op) {
//     consumer_debug(m, "consumer_not()");
//     regex* m0 = array_at(m->ms, 0);
//     return toggle_match_state(m0->fn(ptr, m0, op));
// }