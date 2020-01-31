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
        printf(                                                    \
            "ERROR(FAIL) : %s %s %s\n",                            \
            (PAT), (FLAG), (STR));                                 \
    } else {                                                       \
        _regex_test_count_ok++;                                    \
        printf(                                                    \
            "OK : %s %s\n",                                        \
            PAT, FLAG);                                            \
    }
#define REGEX_TEST(PAT, FLAG, STR)                                 \
    if (regex_match_simple((PAT), (FLAG), (STR)) == (RS_FAILED)) { \
        _regex_test_count_error++;                                 \
        printf(                                                    \
            "ERROR : %s %s %s\n",                                  \
            (PAT), (FLAG), (STR));                                 \
    } else {                                                       \
        _regex_test_count_ok++;                                    \
        printf(                                                    \
            "OK : %s %s\n",                                        \
            PAT, FLAG);                                            \
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

    REGEX_TEST("\\d+", "", "123")
    REGEX_TEST("(\\d+)", "", "123")
    REGEX_TEST("(\\d+)?", "", "")
    REGEX_TEST("(\\d+)?", "", "123")
    REGEX_TEST_FAIL("^(\\d+)?", "", "a123")
    REGEX_TEST_FAIL("(\\d+)?$", "", "123")
    REGEX_TEST_END
}

void dump_memory(void* __ptr, int __n) {
    int width = 16;
    printf("                ");
    for (int i = 0; i < width; i++) printf("%2d ", i);
    printf("\n---------------");
    for (int i = 0; i < width; i++) printf("---");
    printf("\n");
    char* string = (char*)calloc(sizeof(char), width);
    int count = 0;
    while (count < __n) {
        void* fp = __ptr;
        printf("%p  ", fp);
        for (int i = 0; i < width; i++) {
            printf("%02x ", *((unsigned char*)__ptr + i));
        }
        for (int i = 0; i < width; i++) {
            char c = *((unsigned char*)__ptr + i);
            sprintf(string + (count % width), "%c", (31 < c && c < 126) ? c : '.');
        }
        __ptr = (char*)__ptr + width;
        count += width;
        printf(string);
        memset(string, 0, width);
        printf("\n");
    }
}

// void dump_memory(void* __ptr, int __n) {
//     int width = 16;

//     printf("                ");
//     for (int i = 0; i < width; i++) printf("%2d ", i);
//     printf("\n---------------");
//     for (int i = 0; i < width; i++) printf("---");
//     printf("\n");

//     char* memory = (char*)calloc(sizeof(char), width * 3);
//     char* string = (char*)calloc(sizeof(char), width);
//     int count = 0;
//     char* fp = __ptr;
//     while (count++ < __n) {
//         int i = count % width;
//         sprintf(memory + i, "%02x", *((unsigned char*)__ptr + i));
//         char c = *((unsigned char*)__ptr + i);
//         sprintf(string + i, "%c", (31 < c && c < 126) ?: '.');
//         if (count && (i == 0)) {
//             printf("%p  %s %s\n", fp, memory, string);
//             memset(memory, 0, width * 3);
//             memset(string, 0, width);
//             fp += width;
//         }
//     }
//     if (count % width)
//         printf("%p  %s %s\n", fp, memory, string);

//     free(memory);
//     free(string);
// }

int main() {
    {
        // 実体
        array* _array = array_new(sizeof(int), false, 1);
        int a = 1;
        array_push(_array, &a);
        a = 2;
        array_push(_array, &a);
        a = 3;
        array_push(_array, &a);
        array_del(_array, 1);
        array_each_i(_array, printf("%d\n", *(int*)array_ei));
    }
    {
        // ポインタ
        array* _array = array_new(sizeof(int*), true, 1);
        int a = 1;
        array_push(_array, &a);
        int b = 2;
        array_push(_array, &b);
        int c = 3;
        array_push(_array, &c);
        array_del(_array, 1);
        array_each_i(_array, printf("%d\n", *(int*)array_ei));
    }
}