#include <stdio.h>
#include <stdlib.h>
#include "array.h"

typedef struct {
    int x;
    int y;
} point;

int main() {
    array* _array = array_new(sizeof(int), false, 1);
    array_ins(_array, 0, 3);
    int* z = array_at(_array, 0);
    printf("%%d = %d\n", *z);

    exit(0);

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
        array_ins(_array, &d, 1);
        point* z = array_at(_array, 1);
        if (!(z->x == 4 && z->y == 8)) exit(1);
    }
    {
        array* _array = array_new(sizeof(point), false, 1);
        point a;
        a.x = 1;
        a.y = 2;
        array_push(_array, &a);
        array_push(_array, &b);
        array_push(_array, &c);
        array_del(_array, 1);
        array_ins(_array, &d, 1);
        point* z = array_at(_array, 1);
        if (!(z->x == 4 && z->y == 8)) exit(1);
    }
}