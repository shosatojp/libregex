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

#define REGEX_TEST_BEGIN          \
    int _regex_test_count_ok = 0, \
        _regex_test_count_error = 0;

#define REGEX_TEST_FAIL(PAT, FLAG, STR)                            \
    if (regex_match_simple((#PAT), (#FLAG), (#STR)) != (RS_FAILED)) { \
        _regex_test_count_error++;                                 \
        printf("ERROR(FAIL) : %s %s %s\n", (#PAT), (#FLAG), (#STR));  \
    } else {                                                       \
        _regex_test_count_ok++;                                    \
        printf("OK          : %s %s\n", #PAT, #FLAG);                \
    }
#define REGEX_TEST(PAT, FLAG, STR)                                 \
    if (regex_match_simple((#PAT), (#FLAG), (#STR)) == (RS_FAILED)) { \
        _regex_test_count_error++;                                 \
        printf("ERROR       : %s %s %s\n", (#PAT), (#FLAG), (#STR));  \
    } else {                                                       \
        _regex_test_count_ok++;                                    \
        printf("OK          : %s %s\n", #PAT, #FLAG);                \
    }
#define REGEX_TEST_END                                   \
    printf("---------------------\n");                   \
    if (_regex_test_count_error == 0) {                  \
        printf("OK\n");                                  \
    } else {                                             \
        printf("ERROR : %d\n", _regex_test_count_error); \
    }

void test() {
    REGEX_TEST_BEGIN
    REGEX_TEST(a, , a)
    REGEX_TEST(1, , 1)
    REGEX_TEST_FAIL(1, , a)
    REGEX_TEST(\\d, , 3)
    REGEX_TEST(\\\\, , \\)
    REGEX_TEST(\\w+, , hello)
    REGEX_TEST(\\w+, , hello!)
    REGEX_TEST(^\\w, , hoge10)
    REGEX_TEST(^\\w, , 10hoge)
    REGEX_TEST(\\w+$, , 10hoge)
    REGEX_TEST(\\w+$, , hoge10)

    REGEX_TEST(\\d+, , 123)
    REGEX_TEST((\\d+), , 123)
    REGEX_TEST((\\d+)?, , )
    REGEX_TEST((\\d+)?, , 123)
    REGEX_TEST_FAIL(^(\\d+)?, , a123)
    REGEX_TEST_FAIL((\\d+)?$, , 123)
    REGEX_TEST_END
}

int main() {
    test();
}