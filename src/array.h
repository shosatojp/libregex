#pragma once
#include <stdbool.h>
#define DEFAULT_CAPACITY 10
#define DEBUG

typedef void array_element;
typedef unsigned long indextype;

struct _array {
    indextype length;
    indextype capacity;
    array_element** data;
    int elem_size;
    bool isptr;
};

typedef struct _array array;

array* array_new(int elem_size, bool isptr, indextype capacity);
int array_init(array* _array, int elem_size, bool isptr);
int array_expand(array* _array, indextype capacity);
int array_fill(array* _array, indextype _begin, indextype _end, char _c);
int _array_ins(array* _array, indextype index, ...);
array_element* array_del(array* _array, indextype index);
array_element* array_pop(array* _array);
array_element* array_at(array* _array, indextype index);
array_element* array_last(array* _array);
array_element* array_first(array* _array);
array_element* array_set(array* _array, array_element* e, indextype index);
int array_empty(array* _array);
int array_clear(array* _array);

#define array_push(_array, _e) _array_ins(_array, (_array)->length, _e)
#define array_ins(_array, index, e) _array_ins(_array, index, e)

#define _array_each(INDEX, ARRAY, STMT)                                             \
    for (int array_##INDEX = 0; array_##INDEX < (ARRAY)->length; array_##INDEX++) { \
        array_element* array_e##INDEX = array_at((ARRAY), array_##INDEX);           \
        STMT;                                                                       \
    }

#define array_i array_i
#define array_ei array_ei
#define array_each_i(ARRAY, STMT) _array_each(i, ARRAY, STMT)

#define array_j array_j
#define array_ej array_ej
#define array_each_j(ARRAY, STMT) _array_each(j, ARRAY, STMT)

#define array_k array_k
#define array_ek array_ek
#define array_each_k(ARRAY, STMT) _array_each(k, ARRAY, STMT)
