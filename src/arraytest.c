#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"

typedef struct {
    int x;
    int y;
} point;

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
            if (count++ < __n) {
                printf("%02x ", *((unsigned char*)__ptr + i));
                char c = *((unsigned char*)__ptr + i);
                sprintf((char*)string + (i % width), "%c", (31 < c && c < 126) ? c : '.');
            } else {
                printf("   ");
            }
        }
        __ptr = (char*)__ptr + width;
        printf("|%-16s|", string);
        memset(string, 0, width);
        printf("\n");
    }
    free(string);
}

void insert_test(array* _array) {
    char a[] = {'a', 'b', 'c', 0};
    // char* a = "hoge";
    dump_memory(a - 16, 32);
    array_push(_array, a);
}

int main() {
    point a;
    a.x = 1;
    a.y = 2;
    point b;
    b.x = 2;
    b.y = 4;
    point c;
    c.x = 3;
    c.y = 6;
    point d;
    d.x = 4;
    d.y = 8;

    {
        array* _array = array_new(sizeof(point*), true, 1);
        array_push(_array, &a);
        array_push(_array, &b);
        array_push(_array, &c);
        array_del(_array, 1);
        array_ins(_array, 1, &d);
        point* z = array_at(_array, 1);
        if (!(z->x == 4 && z->y == 8)) exit(1);
        printf("%d, %d\n", z->x, z->y);
    }
    {
        array* _array = array_new(sizeof(point), false, 1);
        point a;
        a.x = 1;
        a.y = 2;
        array_push(_array, a);
        array_push(_array, b);
        array_push(_array, c);
        array_del(_array, 1);
        array_ins(_array, 1, d);
        point* z = array_at(_array, 1);
        if (!(z->x == 4 && z->y == 8)) exit(1);
        printf("%d, %d\n", z->x, z->y);
    }
    {
        array* _array_1 = array_new(sizeof(int), false, 10);
        array* _array_2 = array_new(sizeof(point), false, 10);
        array* _array_3 = array_new(sizeof(const char*), true, 10);

        point a;
        a.x = 1;
        a.y = 2;

        array_push(_array_1, 123);
        array_push(_array_2, a);
        // array_push(_array_3, strdup("Hello"));
        insert_test(_array_3);

        printf("%d\n", *(int*)array_at(_array_1, 0));
        printf("%d\n", ((point*)array_at(_array_2, 0))->x);
        printf("%s\n", (const char*)array_at(_array_3, 0));

        dump_memory((char*)array_at(_array_3, 0) - 16, 32);

        // 123
        // 1
        // Hello
    }
}