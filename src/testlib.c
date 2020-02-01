#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

regex_state regex_match_simple(const char* pat, const char* flag, const char* str) {
    regex* _regex = regex_compile(pat, flag);
    regex_options op;
    regex_options_init(&op);
    regex_state result = regex_match(&str, _regex, &op);
    regex_options_destruct(&op);
    regex_destruct(_regex);
    free(_regex);
    return result;
}

void regex_test_fail(const char* pat, const char* flags, const char* str) {
    if (regex_match_simple(pat, flags, str) != RS_FAILED) {
        printf("ERROR: %s %s %s\n", pat, flags, str);
        exit(0);
    }
}
void regex_test(const char* pat, const char* flags, const char* str) {
    if (regex_match_simple(pat, flags, str) == RS_FAILED) {
        printf("ERROR: %s %s %s\n", pat, flags, str);
        exit(0);
    }
}

bool regex_test_find_all(const char* pat, const char* flag, const char* str, int argc, const char** argv) {
    regex* _regex = regex_compile(pat, flag);
    regex_found f;
    regex_found_init(&f);
    regex_find_all(&str, _regex, &f);
    bool result;
    if (argc == f.results->length) {
        array_each_i(f.results,
                     if (!strcmp(array_ei, argv[array_i])) { result = true; } else {
                         result = false;
                         break;
                     });
    } else {
        result = false;
    }
    regex_destruct(_regex);
    free(_regex);
    return result;
}

bool regex_test_capture(const char* pat, const char* flag, const char* str, int argc, const char** argv) {
    regex* _regex = regex_compile(pat, flag);
    regex_options op;
    regex_options_init(&op);
    regex_state re = regex_match(&str, _regex, &op);

    bool result;
    if (re != RS_FAILED && argc == op.captured->length) {
        array_each_i(op.captured,
                     char* str = regex_capture_str(array_ei);
                     if (!strcmp(str, argv[array_i])) { result = true; free(str); } else {
                         result = false;
                         free(str);
                         break;
                     });
    } else {
        result = false;
    }

    regex_options_destruct(&op);
    regex_destruct(_regex);
    free(_regex);
    return result;
}
