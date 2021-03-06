#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

int main(int argc, char* argv[]) {
    /* 文字列 */
    const char *str = argc > 1 ? argv[1] : "https://news.yahoo.co.jp:443/pickup/6350009?hoge=234",
               *ptr = str;

    /* 正規表現 */
    regex* _regex = regex_compile(argc > 2
                                      ? argv[2]
                                      : "(https?)://([\\w.-]+)(?::(\\d+))?(/[^?]+)(\\?.*)?",
                                  argc > 3
                                      ? argv[3]
                                      : "im");

    /* findall */
    printf("----------- find_all --------\n");
    regex_found f;
    regex_found_init(&f);
    regex_find_all(&ptr, _regex, &f);
    array_each_i(f.results, printf("'%s'\n", (const char*)array_ei));

    /* init */
    ptr = str;

    printf("----------- full match --------\n");
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

    printf("%s\n", result == RS_FAILED ? "FAILED" : "MATCHED");
}
