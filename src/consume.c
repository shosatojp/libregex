#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

regex_state consume_char(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_char('%c')", m->c);
    if (**ptr == m->c ||
        (m->ignore_case && lower_case(**ptr) == lower_case(m->c))) {
        (*ptr)++;
        return RS_MATCHED;
    } else {
        return RS_FAILED;
    }
}

regex_state consume_span(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consume_span('%c', '%c')", m->c, m->c2);
    char lower;
    if (m->c <= **ptr && **ptr <= m->c2 ||
        (m->ignore_case && lower_case(m->c) <= (lower = lower_case(**ptr)) && lower <= lower_case(m->c2))) {
        (*ptr)++;
        return RS_MATCHED;
    } else {
        return RS_FAILED;
    }
}

regex_state consume_times(const char** ptr, regex* m, regex_options* op) {
    return m->non_greedy
               ? consume_plus_non_greedy(ptr, m, op)
               : consume_plus_greedy(ptr, m, op);
}

regex_state consume_times_non_greedy(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consume_times_greedy(%d, %d)", m->u, m->v);

    regex* next = next_sibling(m);

    const char* init = *ptr;
    regex* m0 = array_at(m->ms, 0);
}

regex_state consume_times_greedy(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consume_times_greedy(%d, %d)", m->u, m->v);
    int count = 0;
    while (count < m->v) {
        regex* m0 = array_at(m->ms, 0);
        if (m0->fn(ptr, m0, op) == RS_MATCHED) {
            count++;
        } else {
            break;
        }
    }
    if (m->u < 0) {
        if (count == m->v) {
            return RS_MATCHED;
        } else {
            return RS_FAILED;
        }
    } else {
        if ((count == 0 && m->u == 0) || (m->u <= count)) {
            return RS_MATCHED;
        } else {
            return RS_FAILED;
        }
    }
}

regex_state consume_plus(const char** ptr, regex* m, regex_options* op) {
    return m->non_greedy
               ? consume_plus_non_greedy(ptr, m, op)
               : consume_plus_greedy(ptr, m, op);
}

/* non greedy */
regex_state consume_plus_non_greedy(const char** ptr, regex* m, regex_options* op) {
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
        if (result == RS_MATCHED) {
            if (next && next->fn(&_ptr, next, op) == RS_MATCHED) {
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

    switch (m->type) {
        case RT_STAR:
            return RS_MATCHED;
            break;
        case RT_PLUS:
            return init != *ptr ? RS_MATCHED : RS_FAILED;
            break;
        case RT_TIMES:
            if (u < 0) {
                return count == v ? RS_MATCHED : RS_FAILED;
            } else {
                return (u <= count && count <= v) ? RS_MATCHED : RS_FAILED;
            }
            break;
        default:
            break;
    }
}

/* greedy */
regex_state consume_plus_greedy(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consume_plus_greedy()");

    regex* next = next_sibling(m);

    const char* init = *ptr;
    regex* m0 = array_at(m->ms, 0);

    int count = 0,
        u = m->u,
        v = m->v >= 0 ? m->v : (op->tail - op->head);
    consumer_debug(m, "u = %d,v = %d\n", u, v);

    bool _no_capture = op->no_capture;
    while (**ptr && count < v) {
        consumer_debug(m, "**ptr = %c, %d\n", **ptr, **ptr);
        regex_state result = m0->fn(ptr, m0, op);
        if (result == RS_MATCHED) {
            // op->no_capture = true;
            count++;
        } else {
            break;
        }
        printf("captured = %d\n", op->captured->length);
    }
    // op->no_capture = _no_capture;

    consumer_debug(m, "**ptr = %c, %d\n", **ptr, **ptr);
    consumer_debug(m, "count = %d\n", count);
    fflush(stdout);

    while (init < *ptr) {
        consumer_debug(m, "**ptr = %c, %d\n", **ptr, **ptr);

        const char* head = *ptr;
        const char* _ptr = *ptr;
        if (next && next->fn(&_ptr, next, op) == RS_MATCHED) {
            // 後ろから条件nextにマッチするまで
            *ptr = head;
            break;
        } else {
            *ptr = head - 1;
        }
    }

    switch (m->type) {
        case RT_STAR:
            return RS_MATCHED;
            break;
        case RT_PLUS:
            return init != *ptr ? RS_MATCHED : RS_FAILED;
            break;
        case RT_TIMES:
            if (u < 0) {
                return count == v ? RS_MATCHED : RS_FAILED;
            } else {
                return (u <= count && count <= v) ? RS_MATCHED : RS_FAILED;
            }
            break;
        default:
            break;
    }
}

regex_state consume_or(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_or(m->no_capture:%d, op->no_capture:%d)", m->no_capture, op->no_capture);
    const char* init = *ptr;
    for (int i = 0; i < m->ms->length; i++) {
        *ptr = init;
        regex* mi = array_at(m->ms, i);
        if (mi->fn(ptr, mi, op) == RS_MATCHED) {
            if (m->type == RT_OR && !m->no_capture && !op->no_capture) {
                array_push(op->captured, regex_capture_new(init, *ptr - 1));
            }
            return RS_MATCHED;
        }
    }
    return RS_FAILED;
}

/**
 * msの中に一致しない１文字にマッチする
 */
regex_state consume_nor(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_nor()");
    const char* init = *ptr;
    for (int i = 0; i < m->ms->length; i++) {
        *ptr = init;
        regex* mi = array_at(m->ms, i);
        if (mi->fn(ptr, mi, op) == RS_MATCHED) {
            return RS_FAILED;
        }
    }
    (*ptr)++;
    return RS_MATCHED;
}

regex_state consume_head(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_head(%c)", **ptr);
    char c;
    if (*ptr == op->head || (m->multi_line && (c = *(*ptr - 1)) &&
                             (c == '\n' ||
                              c == '\r'))) {
        return RS_MATCHED;
    } else {
        return RS_FAILED;
    }
}

regex_state consume_tail(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_tail(%c)", **ptr);
    if (*ptr == op->tail || **ptr == '\n' || **ptr == '\r') {
        return RS_MATCHED;
    } else {
        return RS_FAILED;
    }
}

regex_state consume_any(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_any()");
    if (**ptr && (m->dot_all || **ptr != '\n')) {
        (*ptr)++;
        return RS_MATCHED;
    } else {
        return RS_FAILED;
    }
}

regex_state consume_seq(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_seq()");
    for (int i = 0; i < m->ms->length; i++) {
        regex* mi = array_at(m->ms, i);
        if (mi->fn(ptr, mi, op) == RS_FAILED) {
            consumer_debug(m, "└(re = %d, char = %c)", RS_FAILED, **ptr);
            return RS_FAILED;
        } else {
            consumer_debug(m, "└(re = %d, char = %c)", RS_MATCHED, **ptr);
        }
    }
    consumer_debug(m, "└(re = %d)", RS_MATCHED);
    return RS_MATCHED;
}

regex_state regex_match(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "match_regex()");

    /* set constants */
    op->head = op->head ? op->head : *ptr;
    op->tail = op->head + strlen(op->head);

    /* execute */
    return consume_seq(ptr, m, op);
}

regex_state consume_not(const char** ptr, regex* m, regex_options* op) {
    consumer_debug(m, "consumer_not()");
    regex* m0 = array_at(m->ms, 0);
    return toggle_match_state(m0->fn(ptr, m0, op));
}