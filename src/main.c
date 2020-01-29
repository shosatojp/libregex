#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

int main(int argc, char* argv[]) {
    /* 文字列 */
    const char *str = argc > 1 ? argv[1] : "123,456",
               *ptr = str;

    /* 正規表現 */
    regex* _regex = regex_compile(argc > 2 ? argv[2] : "(\\d+|,)", "ims");

    // find_all(&ptr, _regex);
    // exit(0);

    regex_options op;
    regex_options_init(&op);

    /* 実行 */
    regex_state result = regex_match(&ptr, _regex, &op);

    /* キャプチャ＆表示 */
    for (int i = 0; i < op.captured->length; i++) {
        char* str = regex_capture_str(array_at(op.captured, i));
        printf("$%d : %s\n", i + 1, str);
        free(str);
    }

    /* 終了処理 */
    regex_options_destruct(&op);
    regex_destruct(_regex);
    free(_regex);

    debug("current : %c (at %ld)\n", *ptr, ptr - str);
    printf("%s\n", result == RS_FAILED ? "FAILED" : "MATCHED");
}