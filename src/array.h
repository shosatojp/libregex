#pragma once
#include <stdbool.h>

#define DEFAULT_CAPACITY 10

typedef void array_element;

struct _array {
    int length;
    int capacity;
    array_element** data;
    int elem_size;
    int isptr;
};

typedef struct _array array;

array* array_new(int elem_size, int isptr, int capacity);
int array_init(array* _array, int elem_size, int isptr);
int array_expand(array* _array, int capacity);
int array_push(array* _array, array_element* e);
int array_ins(array* _array, array_element* e, int index);
array_element* array_del(array* _array, int index);
array_element* array_at(array* _array, int index);
array_element* array_set(array* _array, array_element* e, int index);
int array_empty(array* _array);
int array_clear(array* _array);

#define array_i array_i
#define array_e array_e
#define array_each(ARRAY, STMT)                                   \
    for (int array_i = 0; array_i < (ARRAY)->length; array_i++) { \
        array_element* array_e = array_at((ARRAY), array_i);      \
        STMT;                                                     \
    }
