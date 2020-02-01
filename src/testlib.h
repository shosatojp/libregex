#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

regex_state regex_match_simple(const char* pat, const char* flag, const char* str);
void regex_test_fail(const char* pat, const char* flags, const char* str);
void regex_test(const char* pat, const char* flags, const char* str);
#define REGEX_TEST(PAT, FLAGS, STR) \
    regex_test((PAT), (FLAGS), (STR));
#define REGEX_TEST_FAIL(PAT, FLAGS, STR) \
    regex_test_fail((PAT), (FLAGS), (STR));

bool regex_test_find_all(const char* pat, const char* flag, const char* str, int argc, const char** argv);
bool regex_test_capture(const char* pat, const char* flag, const char* str, int argc, const char** argv);

#define REGEX_TEST_MULTI_ARGS(FN, PAT, FLAGS, STR, ...)         \
    {                                                           \
        const char* strs[] = {__VA_ARGS__};                     \
        int len = sizeof(strs) / sizeof(void*);                 \
        bool result = (FN)((PAT), (FLAGS), (STR), len, strs);   \
        if (!result) {                                          \
            printf("ERROR: %s %s %s\n", (PAT), (FLAGS), (STR)); \
            exit(0);                                            \
        }                                                       \
    }

#define REGEX_TEST_FIND_ALL(PAT, FLAGS, STR, ...) \
    REGEX_TEST_MULTI_ARGS(regex_test_find_all, PAT, FLAGS, STR, __VA_ARGS__)

#define REGEX_TEST_CAPTURE(PAT, FLAGS, STR, ...) \
    REGEX_TEST_MULTI_ARGS(regex_test_capture, PAT, FLAGS, STR, __VA_ARGS__)
#define REGEX_TEST_END printf("OK\n");