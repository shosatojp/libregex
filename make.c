#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

void copy_constants(struct matcher* m) {
    if (m->ms) {
        for (int i = 0; i < m->ms->length; i++) {
            struct matcher* mi = (struct matcher*)array_at(m->ms, i);
            mi->head = m->head;
            mi->tail = m->tail;
            mi->ignore_case = m->ignore_case;
            mi->multi_line = m->multi_line;
            mi->dot_all = m->dot_all;
            copy_constants(mi);
        }
    }
}

struct matcher* compile_regex(const char* pat, const char* flags) {
    struct matcher* m = make_pattern_matcher(NULL, pat);
    if (strchr(flags, 'i')) m->ignore_case = true;
    if (strchr(flags, 'm')) m->multi_line = true;
    if (strchr(flags, 's')) m->dot_all = true;
    return m;
}

struct matcher* make_pattern_matcher(struct matcher* mp, const char* pat) {
    struct matcher* m = make_consume_seq_matcher(mp);
    m->mp = mp;
    tokenize(m, &pat);
    return m;
}

struct matcher* make_empty_matcher() {
    return (struct matcher*)calloc(sizeof(struct matcher), 1);
}

struct matcher* make_consume_char_matcher(struct matcher* mp, char c) {
    struct matcher* m = make_empty_matcher();
    m->type = MT_CHAR;
    m->fn = consume_char;
    m->c = c;
    m->mp = mp;
    return m;
}

struct matcher* make_consume_head_matcher(struct matcher* mp) {
    struct matcher* m = make_empty_matcher();
    m->type = MT_HEAD;
    m->fn = consume_head;
    m->mp = mp;
    return m;
}

struct matcher* make_consume_tail_matcher(struct matcher* mp) {
    struct matcher* m = make_empty_matcher();
    m->type = MT_TAIL;
    m->fn = consume_tail;
    m->mp = mp;
    return m;
}

struct matcher* make_consume_times_matcher(struct matcher* mp, int u, int v, struct matcher* m0) {
    struct matcher* m = make_empty_matcher();
    m->type = MT_TIMES;
    m->fn = consume_times;
    m->u = u;
    m->v = v;
    m->ms = array_new(sizeof(void*), true);
    m->mp = mp;
    array_push(m->ms, m0);
    return m;
}

struct matcher* make_consume_span_matcher(struct matcher* mp, char c, char c2) {
    struct matcher* m = make_empty_matcher();
    m->type = MT_SPAN;
    m->fn = consume_span;
    m->c = c;
    m->c2 = c2;
    m->mp = mp;
    return m;
}

struct matcher* make_consume_not_matcher(struct matcher* mp, struct matcher* m0) {
    struct matcher* m = make_empty_matcher();
    m->type = MT_NOT;
    m->fn = consume_not;
    m->ms = array_new(sizeof(void*), true);
    m->mp = mp;
    array_push(m->ms, m0);
    return m;
}

struct matcher* make_consume_or_matcher(struct matcher* mp) {
    struct matcher* m = make_empty_matcher();
    m->type = MT_OR;
    m->fn = consume_or;
    m->ms = array_new(sizeof(void*), true);
    m->mp = mp;
    return m;
}

struct matcher* make_consume_anyof_matcher(struct matcher* mp) {
    struct matcher* m = make_empty_matcher();
    m->type = MT_ANYOF;
    m->fn = consume_or;
    m->ms = array_new(sizeof(void*), true);
    m->mp = mp;
    return m;
}

struct matcher* make_consume_nonof_matcher(struct matcher* mp) {
    struct matcher* m = make_empty_matcher();
    m->type = MT_NONOF;
    m->fn = consume_nor;
    m->ms = array_new(sizeof(void*), true);
    m->mp = mp;
    return m;
}

struct matcher* make_consume_seq_matcher(struct matcher* mp) {
    struct matcher* m = make_empty_matcher();
    m->type = MT_SEQ;
    m->fn = consume_seq;
    m->ms = array_new(sizeof(void*), true);
    m->mp = mp;
    return m;
}

struct matcher* make_consume_any_matcher(struct matcher* mp) {
    struct matcher* m = make_empty_matcher();
    m->type = MT_ANY;
    m->fn = consume_any;
    m->mp = mp;
    return m;
}

struct matcher* make_consume_plus_matcher(struct matcher* mp, char c, struct matcher* m0) {
    struct matcher* m = make_empty_matcher();
    m->type = MT_PLUS;
    m->fn = consume_plus;
    m->c = c;
    m->ms = array_new(sizeof(void*), true);
    m->mp = mp;
    array_push(m->ms, m0);
    return m;
}

struct matcher* make_consume_star_matcher(struct matcher* mp, char c, struct matcher* m0) {
    struct matcher* m = make_empty_matcher();
    m->type = MT_STAR;
    m->fn = consume_plus;
    m->c = c;
    m->ms = array_new(sizeof(void*), true);
    m->mp = mp;
    array_push(m->ms, m0);
    return m;
}