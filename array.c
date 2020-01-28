#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Array* array_new(int elem_size, int isptr) {
    struct Array* array = (struct Array*)calloc(sizeof(struct Array), 1);
    array_init(array, elem_size, isptr);
    return array;
}

int array_init(struct Array* array, int elem_size, int isptr) {
    memset(array, 0, sizeof(struct Array));
    array->isptr = isptr;
    array->elem_size = isptr ? sizeof(void*) : elem_size;
}

int array_expand(struct Array* array, int capacity) {
    void** tmp = (void**)malloc(array->elem_size * capacity);
    if (tmp) {
        if (array->data) {
            printf("capacity %d\n", capacity);
            memmove(tmp, array->data, (array->length) * array->elem_size);
            free(array->data);
            array->data = NULL;
        }
        array->data = tmp;
        array->capacity = capacity;
        return 0;
    } else {
        // failed to allocate
        fprintf(stdout, "failed to allocate memory.");
        return -1;
    }
}

int array_push(struct Array* array, void* e) {
    return array_ins(array, e, array->length);
}

int array_ins(struct Array* array, void* e, int index) {
    if (index > array->length || index < 0) {
        fprintf(stdout, "index out of range.");
        return -1;
    } else if (array->capacity < array->length + 1 &&
               array_expand(array, (array->capacity ? array->capacity : DEFAULT_CAPACITY) * 2) < 0) {
        fprintf(stdout, "failed to expand.");
        return -1;
    } else {
        int bytes = array->isptr ? 1 : array->elem_size;
        memmove(array->data + (index + 1) * bytes,
                array->data + index * bytes,
                (array->length - index) * bytes);
        memcpy(array->data + (index * bytes),
               array->isptr ? &e : e, array->elem_size);
        array->length++;
        return 0;
    }
}

int array_del(struct Array* array, int index) {
    if (index >= array->length || index < 0) {
        fprintf(stdout, "index out of range.");
        return -1;
    } else {
        int bytes = array->isptr ? 1 : array->elem_size;
        memmove(array->data + (index * bytes),
                array->data + (index + 1) * bytes,
                (array->length - index - 1) * bytes);
        memset(array->data + (array->length - 1) * bytes, 0, 1);
        array->length--;
        return 0;
    }
}

int array_empty(struct Array* array) {
    array->length = 0;
    return 0;
}

int array_clear(struct Array* array) {
    if (array->data) {
        free(array->data);
        array->data = NULL;
    }
}

void* array_at(struct Array* array, int index) {
    int bytes = array->isptr ? 1 : array->elem_size;
    void** ptr = array->data + (index * bytes);
    return array->isptr ? *ptr : ptr;
}

int array_set(struct Array* array, Element e, int index) {
    if (-1 < index && index < array->length) {
        int bytes = array->isptr ? 1 : array->elem_size;
        memcpy(array->data + (index * bytes), array->isptr ? &e : e, bytes);
    } else {
        fprintf(stderr, "index out of range.\n");
    }
}