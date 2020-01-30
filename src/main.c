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
    if (regex_match_simple((PAT), (FLAG), (STR)) != (RS_FAILED)) { \
        _regex_test_count_error++;                                 \
        printf("ERROR(FAIL) : %s %s %s\n", (PAT), (FLAG), (STR));  \
    } else {                                                       \
        _regex_test_count_ok++;                                    \
        printf("OK          : %s %s\n", PAT, FLAG);                \
    }
#define REGEX_TEST(PAT, FLAG, STR)                                 \
    if (regex_match_simple((PAT), (FLAG), (STR)) == (RS_FAILED)) { \
        _regex_test_count_error++;                                 \
        printf("ERROR       : %s %s %s\n", (PAT), (FLAG), (STR));  \
    } else {                                                       \
        _regex_test_count_ok++;                                    \
        printf("OK          : %s %s\n", PAT, FLAG);                \
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
    REGEX_TEST("\\d+", "", "123")
    REGEX_TEST("(\\d+)", "", "123")
    REGEX_TEST("(\\d+)?", "", "")
    REGEX_TEST("(\\d+)?", "", "123")
    REGEX_TEST_FAIL("^(\\d+)?", "", "a123")
    REGEX_TEST_FAIL("(\\d+)?$", "", "123")
    REGEX_TEST_END
}

int main(int argc, char* argv[]) {
    test();
    exit(0);
    /* 文字列 */
    const char *str = argc > 1 ? argv[1] : "233",
               *ptr = str;

    /* 正規表現 */
    regex* _regex = regex_compile(argc > 2
                                      ? argv[2]
                                      : "(\\d{2,}?)",
                                  "im");

    /* findall */
    regex_found f;
    regex_found_init(&f);
    regex_find_all(&ptr, _regex, &f);
    array_each_i(f.results, printf("%s\n", (const char*)array_ei));
    // exit(0);

    /* init */
    ptr = str;

    regex_options op;
    regex_options_init(&op);

    /* 実行 */
    regex_state result = regex_match(&ptr, _regex, &op);

    /* キャプチャ＆表示 */
    array_each_i(op.captured,
                 char* str = regex_capture_str(array_ei);
                 printf("$%d : %s\n", array_i + 1, str);
                 free(str));

    /* 終了処理 */
    regex_options_destruct(&op);
    regex_destruct(_regex);
    free(_regex);

    debug("current : %c (at %ld)\n", *ptr, ptr - str);
    printf("%s\n", result == RS_FAILED ? "FAILED" : "MATCHED");
}
