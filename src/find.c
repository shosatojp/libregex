#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

int regex_find_all(const char** ptr, regex* m, regex_found* f) {
    const char* head = *ptr;
    regex_options op;
    while (**ptr) {
        regex_options_init(&op);
        const char* init = *ptr;
        op.head = head;
        int matched_len = regex_match(ptr, m, &op);
        switch (matched_len) {
            case -1:
                *ptr = init + 1;
                break;
            case 0: {
                char* str = (char*)strcut(init, *ptr - 1);
                array_push(f->results, str);
                (*ptr)++;
                break;
            }
            default: {
                char* str = (char*)strcut(init, *ptr - 1);
                array_push(f->results, str);
                break;
            }
        }
        regex_options_destruct(&op);
    }
}

int regex_found_init(regex_found* f) {
    memset(f, 0, sizeof(regex_found));
    f->results = array_new(sizeof(regex_found*), true, 8);
}

int regex_found_destruct(regex_found* f) {
    for (int i = 0; i < f->results->length; i++) {
        free(array_at(f->results, i));
    }
    free(f->results);
}
