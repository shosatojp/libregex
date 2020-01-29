#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

regex_capture* regex_capture_new(const char* begin, const char* end) {
    regex_capture* c = (regex_capture*)malloc(sizeof(regex_capture));
    c->begin = begin;
    c->end = end;
    return c;
}

int regex_capture_int(regex_capture* c) {
    char* str = regex_capture_str(c);
    int i = atoi(str);
    free(str);
    return i;
}

/**
 * MUST free returned pointer
 */
char* regex_capture_str(regex_capture* c) {
    return strcut(c->begin, c->end);
}

char* strcut(const char* begin, const char* end) {
    int len = end - begin + 1;
    char* str = (char*)calloc(sizeof(char), len + 1);
    memcpy(str, begin, len);
    return str;
}
