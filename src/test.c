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

#define REGEX_TEST(PAT, FLAGS, STR) \
    regex_test((PAT), (FLAGS), (STR));
#define REGEX_TEST_FAIL(PAT, FLAGS, STR) \
    regex_test_fail((PAT), (FLAGS), (STR));

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

#define REGEX_TEST_MULTI_ARGS(FN, PAT, FLAGS, STR, ...)         \
    {                                                           \
        const char* strs[] = {__VA_ARGS__};                           \
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

void test() {
    REGEX_TEST("a", "", "a")
    REGEX_TEST("1", "", "1")
    REGEX_TEST_FAIL("1", "", "a")
    REGEX_TEST("\\d", "", "3")
    REGEX_TEST("\\\\", "", "\\")
    REGEX_TEST("\\w+", "", "hello")
    REGEX_TEST("\\w+", "", "hello!")
    REGEX_TEST("^\\w", "", "hoge10")
    REGEX_TEST("^\\w", "", "10hoge")
    REGEX_TEST("\\w+$", "", "10hoge")
    REGEX_TEST("\\w+$", "", "hoge10")

    REGEX_TEST("[0-9]", "", "3")
    REGEX_TEST("[^0-9]", "", "a")

    REGEX_TEST_FAIL("\\d{3,5}", "", "34")
    REGEX_TEST("\\d{3,5}", "", "345")

    REGEX_TEST("(\\d+)", "", "123")
    REGEX_TEST("(\\d+)?", "", "")
    REGEX_TEST("(\\d+)?", "", "123")

    REGEX_TEST("\\d+", "", "123")  // 5a
    REGEX_TEST("\\d*", "", "a")    // 5a

    REGEX_TEST("\\d*", "", "a")  // 5a

    REGEX_TEST("(\\d+)", "", "1")
    REGEX_TEST_FIND_ALL("\\d+", "", "1s2", "1", "2")
}

int main() {
    // test();
    REGEX_TEST_FIND_ALL("([\\w\\.\\-]+)@([\\w\\.\\-]+)", "", "<a href=\"mailto:hoge@example.com\">Send Mail</a>", "hoge@example.com")
    REGEX_TEST_FIND_ALL("0x[\\dA-F]{2}", "", "hoge0x340x12 hige0x5678", "0x34", "0x12", "0x56")
    REGEX_TEST_FIND_ALL("<.*>", "i", "<H1>hoge</h1>", "<H1>hoge</h1>")
    REGEX_TEST_FIND_ALL("<.*?>", "i", "<H1>hoge</h1>", "<H1>", "</h1>")

    REGEX_TEST_FIND_ALL("Apple?", "i", "apple Appl APPLEE", "apple", "Appl", "APPLE")

    REGEX_TEST_CAPTURE("(\\d+)", "", "222,234", "222")
    REGEX_TEST_CAPTURE("(.*)@(.*)", "", "hoge@example.com", "hoge", "example.com")
    REGEX_TEST_CAPTURE("(.*)@(?:.*)", "", "hoge@example.com", "hoge")
    REGEX_TEST_END
}