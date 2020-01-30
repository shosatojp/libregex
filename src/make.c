#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

regex* regex_compile(const char* pat, const char* flags) {
    regex* m = make_pattern_matcher(NULL, pat);
    if (strchr(flags, 'i')) m->ignore_case = true;
    if (strchr(flags, 'm')) m->multi_line = true;
    if (strchr(flags, 's')) m->dot_all = true;
    return m;
}

regex* make_pattern_matcher(regex* mp, const char* pat) {
    regex* m = make_consume_seq_matcher(mp);
    m->mp = mp;
    tokenize(m, &pat);
    return m;
}

int matcher_count = 0;

regex* make_empty_matcher() {
    regex* m = (regex*)calloc(sizeof(regex), 1);
    m->id = matcher_count++;
    if (m->id == 78) {
        m->id;
    }
    return m;
}

regex* make_consume_char_matcher(regex* mp, char c) {
    regex* m = make_empty_matcher();
    m->type = RT_CHAR;
    m->fn = consume_char;
    m->c = c;
    m->mp = mp;
    return m;
}

regex* make_consume_head_matcher(regex* mp) {
    regex* m = make_empty_matcher();
    m->type = RT_HEAD;
    m->fn = consume_head;
    m->mp = mp;
    return m;
}

regex* make_consume_tail_matcher(regex* mp) {
    regex* m = make_empty_matcher();
    m->type = RT_TAIL;
    m->fn = consume_tail;
    m->mp = mp;
    return m;
}

regex* make_consume_times_matcher(regex* mp, int u, int v, regex* m0) {
    regex* m = make_empty_matcher();
    m->type = RT_TIMES;
    m->fn = consume_times;
    m->u = u;
    m->v = v;
    m->ms = array_new(sizeof(void*), true, 4);
    m->mp = mp;
    array_push(m->ms, m0);
    return m;
}

regex* make_consume_span_matcher(regex* mp, char c, char c2) {
    regex* m = make_empty_matcher();
    m->type = RT_SPAN;
    m->fn = consume_span;
    m->c = c;
    m->c2 = c2;
    m->mp = mp;
    return m;
}

// regex* make_consume_not_matcher(regex* mp, regex* m0) {
//     regex* m = make_empty_matcher();
//     m->type = RT_NOT;
//     m->fn = consume_not;
//     m->ms = array_new(sizeof(void*), true, 4);
//     m->mp = mp;
//     array_push(m->ms, m0);
//     return m;
// }

regex* make_consume_or_matcher(regex* mp) {
    regex* m = make_empty_matcher();
    m->type = RT_OR;
    m->fn = consume_or;
    m->ms = array_new(sizeof(void*), true, 4);
    m->mp = mp;
    return m;
}

regex* make_consume_anyof_matcher(regex* mp) {
    regex* m = make_empty_matcher();
    m->type = RT_ANYOF;
    m->fn = consume_or;
    m->ms = array_new(sizeof(void*), true, 4);
    m->mp = mp;
    return m;
}

regex* make_consume_nonof_matcher(regex* mp) {
    regex* m = make_empty_matcher();
    m->type = RT_NONOF;
    m->fn = consume_nor;
    m->ms = array_new(sizeof(void*), true, 4);
    m->mp = mp;
    return m;
}

regex* make_consume_seq_matcher(regex* mp) {
    regex* m = make_empty_matcher();
    m->type = RT_SEQ;
    m->fn = consume_seq;
    m->ms = array_new(sizeof(void*), true, 4);
    m->mp = mp;
    return m;
}

regex* make_consume_any_matcher(regex* mp) {
    regex* m = make_empty_matcher();
    m->type = RT_ANY;
    m->fn = consume_any;
    m->mp = mp;
    return m;
}

regex* make_consume_plus_matcher(regex* mp, char c, regex* m0) {
    regex* m = make_empty_matcher();
    m->type = RT_PLUS;
    m->fn = consume_plus;
    m->c = c;
    m->ms = array_new(sizeof(void*), true, 4);
    m->mp = mp;
    m->u = 1;
    m->v = -1;
    array_push(m->ms, m0);
    return m;
}

regex* make_consume_star_matcher(regex* mp, char c, regex* m0) {
    regex* m = make_empty_matcher();
    m->type = RT_STAR;
    m->fn = consume_plus;
    m->c = c;
    m->ms = array_new(sizeof(void*), true, 4);
    m->mp = mp;
    m->u = 0;
    m->v = -1;
    array_push(m->ms, m0);
    return m;
}