#pragma once

#define DEFAULT_CAPACITY 10

typedef void Element;

struct Array {
    int length;
    int capacity;
    Element** data;
    int elem_size;
    int isptr;
};

typedef struct Array array;

array* array_new(int elem_size, int isptr);
int array_init(array* _array, int elem_size, int isptr);
int array_expand(array* _array, int capacity);
int array_push(array* _array, Element* e);
int array_ins(array* _array, Element* e, int index);
int array_del(array* _array, int index);
int array_clear(array* _array);
void* array_at(array* _array, int index);
int array_set(array* _array, Element* e, int index);
int array_empty(array* _array);