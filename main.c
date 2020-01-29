#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "libregex.h"

int main(int argc, char* argv[]) {
    /* 文字列 */
    const char *str = argc > 1 ? argv[1] : "djdj33jjkkei11lkjl4jlkjdfsljk6666",
               *ptr = str;

    /* 正規表現 */
    regex* _regex = compile_regex(argc > 2 ? argv[2] : "(\\d+|jj|kk)", "ims");

    find_all(&ptr, _regex);
    exit(0);

    /* 実行 */
    match_state result = match_regex(&ptr, _regex);

    /* キャプチャ＆表示 */
    array* captured = captured_all(_regex);
    for (int i = 0; i < captured->length; i++) {
        char* str = capture_str((struct capture*)array_at(captured, i));
        printf("$%d : %s\n", i + 1, str);
        free(str);
    }
    free(captured);

    /* 終了処理 */
    clear_regex(_regex);
    destruct_regex(_regex);
    free(_regex);

    debug("current : %c (at %ld)\n", *ptr, ptr - str);
    printf("%s\n", result == MS_FAILED ? "FAILED" : "MATCHED");
}