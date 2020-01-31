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
            array_each_i(m->ms,
                         if (array_ei == prev && array_i < m->ms->length - 1) {
                             sibling = array_at(m->ms, array_i + 1);
                             break;
                         });
            if (sibling) break;
        }
    }
    return sibling;
}

void put_regex_type(regex_type rt) {
    const char* labels[] = {"RT_SPAN",
                            "RT_CHAR",
                            "RT_PLUS",
                            "RT_STAR",
                            "RT_NOT",
                            "RT_OR",
                            "RT_ROOT",
                            "RT_ANY",
                            "RT_NONOF"
                            "RT_ANYOF"
                            "RT_HEAD",
                            "RT_TAIL",
                            "RT_SEQ",
                            "RT_TIMES"};
    printf("%s", labels[rt]);
}

void dump_memory(void* __ptr, int __n) {
    int width = 16;

    printf("                ");
    for (int i = 0; i < width; i++) printf("%2d ", i);
    printf("\n---------------");
    for (int i = 0; i < width; i++) printf("---");
    printf("\n");

    char* string = (char*)calloc(sizeof(char), width);
    int count = 0;
    while (count < __n) {
        void* fp = __ptr;
        printf("%p  ", fp);
        for (int i = 0; i < width; i++) {
            if (count++ < __n) {
                printf("%02x ", *((unsigned char*)__ptr + i));
                char c = *((unsigned char*)__ptr + i);
                sprintf((char*)string + (i % width), "%c", (31 < c && c < 126) ? c : '.');
            } else {
                printf("   ");
            }
        }
        __ptr = (char*)__ptr + width;
        printf("|%-16s|", string);
        memset(string, 0, width);
        printf("\n");
    }
    free(string);
}

void copy_constants(regex* m) {
    if (m->ms) {
        array_each_i(m->ms,
                     ((regex*)array_ei)->ignore_case = m->ignore_case;
                     ((regex*)array_ei)->multi_line = m->multi_line;
                     ((regex*)array_ei)->dot_all = m->dot_all;
                     copy_constants(array_ei);)
    }
}