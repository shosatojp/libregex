#pragma once
#define DEFAULT_CAPACITY 10
struct point {
    double x;
    double y;
};

typedef void* Element;

struct Array {
    int length;
    int capacity;
    Element* data;
    int elem_size;  // bytes (ptr -> 4 or 8)
    int isptr;
};

typedef struct Array array;

struct Array* array_new(int elem_size, int isptr);
int array_init(struct Array* array, int elem_size, int isptr);
int array_expand(struct Array* array, int capacity);
int array_push(struct Array* array, Element e);
int array_ins(struct Array* array, Element e, int index);
int array_del(struct Array* array, int index);
int array_clear(struct Array* array);
void* array_at(struct Array* array, int index);
int array_set(struct Array* array, Element e, int index);
int array_empty(struct Array* array);