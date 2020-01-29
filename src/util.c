#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

char upper_case(char c) {
    return is_lower_case(c) ? c - 32 : c;
}

char lower_case(char c) {
    return is_upper_case(c) ? c + 32 : c;
}

bool is_upper_case(char c) {
    return ('A' <= c && c <= 'Z');
}

bool is_lower_case(char c) {
    return ('a' <= c && c <= 'z');
}

regex_state toggle_match_state(regex_state state) {
    return (RS_MATCHED + RS_FAILED) - state;
}

regex* next_sibling(regex* m0) {
    regex* m = m0;
    regex* prev = NULL;
    regex* sibling = NULL;
    while (m->mp) {
        prev = m;
        m = m->mp;
        if (m->type == RT_SEQ && m->ms && m->ms->length > 1) {
            array_each(m->ms,
                       if (array_e == prev && array_i < m->ms->length - 1) {
                           sibling = array_at(m->ms, array_i + 1);
                           break;
                       });
            if (sibling) break;
        }
    }
    return sibling;
}