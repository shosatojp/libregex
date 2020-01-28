#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

struct capture* capture_new(const char* begin, const char* end) {
    struct capture* c = (struct capture*)malloc(sizeof(struct capture));
    c->begin = begin;
    c->end = end;
    return c;
}

int capture_int(struct capture* c) {
    char* str = capture_str(c);
    int i = atoi(str);
    free(str);
    return i;
}

/**
 * MUST free returned pointer
 */
char* capture_str(struct capture* c) {
    return strcut(c->begin, c->end);
}

char* strcut(const char* begin, const char* end) {
    int len = end - begin + 1;
    char* str = (char*)calloc(sizeof(char), len + 1);
    memcpy(str, begin, len);
    return str;
}

/**
 * MUST free returned pointer
 */
struct Array* captured_all(struct matcher* root) {
    struct Array* captured = array_new(sizeof(struct capture*), true);
    _captured_all(root, captured);
    return captured;
}

int _captured_all(struct matcher* root, struct Array* cs) {
    if (root->ms) {
        for (int i = 0; i < root->ms->length; i++) {
            struct matcher* m = (struct matcher*)array_at(root->ms, i);
            if (m->captured) array_push(cs, m->captured);
            _captured_all(m, cs);
        }
    }
}
