#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

array* array_new(int elem_size, int isptr) {
    array* _array = (array*)calloc(sizeof(array), 1);
    array_init(_array, elem_size, isptr);
    return _array;
}

int array_init(array* _array, int elem_size, int isptr) {
    memset(_array, 0, sizeof(array));
    _array->isptr = isptr;
    _array->elem_size = isptr ? sizeof(void*) : elem_size;
    return 0;
}

int array_expand(array* _array, int capacity) {
    void** tmp = (void**)malloc(_array->elem_size * capacity);
    if (tmp) {
        if (_array->data) {
            memmove(tmp, _array->data, (_array->length) * _array->elem_size);
            free(_array->data);
            _array->data = NULL;
        }
        _array->data = tmp;
        _array->capacity = capacity;
        return 0;
    } else {
        fprintf(stdout, "failed to allocate memory.");
        return -1;
    }
}

int array_push(array* _array, void* e) {
    return array_ins(_array, e, _array->length);
}

int array_ins(array* _array, void* e, int index) {
    if (index > _array->length || index < 0) {
        fprintf(stdout, "index out of range.");
        return -1;
    } else if (_array->capacity < _array->length + 1 &&
               array_expand(_array, (_array->capacity ? _array->capacity : DEFAULT_CAPACITY) * 2) < 0) {
        fprintf(stdout, "failed to expand.");
        return -1;
    } else {
        int bytes = _array->isptr ? 1 : _array->elem_size;
        memmove(_array->data + (index + 1) * bytes,
                _array->data + index * bytes,
                (_array->length - index) * bytes);
        memcpy(_array->data + (index * bytes),
               _array->isptr ? &e : e, _array->elem_size);
        _array->length++;
        return 0;
    }
}

int array_del(array* _array, int index) {
    if (index >= _array->length || index < 0) {
        fprintf(stdout, "index out of range.");
        return -1;
    } else {
        int bytes = _array->isptr ? 1 : _array->elem_size;
        memmove(_array->data + (index * bytes),
                _array->data + (index + 1) * bytes,
                (_array->length - index - 1) * bytes);
        memset(_array->data + (_array->length - 1) * bytes, 0, 1);
        _array->length--;
        return 0;
    }
}

int array_empty(array* _array) {
    _array->length = 0;
    return 0;
}

int array_clear(array* _array) {
    if (_array->data) {
        free(_array->data);
        _array->data = NULL;
        return 0;
    } else {
        return -1;
    }
}

void* array_at(array* _array, int index) {
    int bytes = _array->isptr ? 1 : _array->elem_size;
    void** ptr = _array->data + (index * bytes);
    return _array->isptr ? *ptr : ptr;
}

int array_set(array* _array, Element* e, int index) {
    if (-1 < index && index < _array->length) {
        int bytes = _array->isptr ? 1 : _array->elem_size;
        memcpy(_array->data + (index * bytes), _array->isptr ? &e : e, bytes);
        return 0;
    } else {
        fprintf(stderr, "index out of range.\n");
        return -1;
    }
}