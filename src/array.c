#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void _array_debug(const char* format, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
#endif
}

array* array_new(int elem_size, int isptr, int capacity) {
    array* _array = (array*)calloc(sizeof(array), 1);
    array_init(_array, elem_size, isptr);
    array_expand(_array, capacity);
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
        _array_debug("failed to allocate memory.\n");
        return -1;
    }
}

int array_push(array* _array, void* e) {
    return array_ins(_array, e, _array->length);
}

int array_ins(array* _array, void* e, int index) {
    if (index > _array->length || index < 0) {
        _array_debug("index out of range.\n");
        return -1;
    } else if (_array->capacity < _array->length + 1 &&
               array_expand(_array, (_array->capacity ? _array->capacity : DEFAULT_CAPACITY) * 2) < 0) {
        _array_debug("failed to expand.\n");
        return -1;
    } else {
        int bytes = _array->isptr ? 1 : _array->elem_size;
        memmove((void*)_array->data + (index + 1) * bytes,
                (void*)_array->data + index * bytes,
                (_array->length - index) * bytes);
        memcpy((void*)_array->data + (index * bytes),
               _array->isptr ? &e : e, _array->elem_size);
        _array->length++;
        return 0;
    }
}

/**
 * you need to **destruct** object pointed by deleted ptr
 */
array_element* array_del(array* _array, int index) {
    if (index >= _array->length || index < 0) {
        _array_debug("index out of range.\n");
        return NULL;
    } else {
        int bytes = _array->isptr ? 1 : _array->elem_size;
        void** head = (void*)_array->data + (index * bytes);
        array_element* ptr = _array->isptr ? *head : NULL;
        memmove(head, head + bytes,
                (_array->length - index - 1) * bytes);
        memset((void*)_array->data + (_array->length - 1) * bytes, 0, 1);
        _array->length--;
        return ptr;
    }
}

/**
 * you need to **destruct** object pointed by poped ptr
 */
array_element* array_pop(array* _array) {
    return array_del(_array, _array->length - 1);
}

/**
 * empty array
 * you need to **destruct** object pointed by ptrs
 */
int array_empty(array* _array) {
    _array->length = 0;
    return 0;
}

/**
 * destruct array data
 * you need to **destruct** object pointed by ptrs
 */
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
    void** ptr = (void*)_array->data + (index * bytes);
    return _array->isptr ? *ptr : ptr;
}

array_element* array_last(array* _array) {
    return array_at(_array, _array->length - 1);
}

array_element* array_first(array* _array) {
    return array_at(_array, 0);
}

/**
 * overwrite element
 * you need to **destruct** object pointed by overwritten ptr
 */
array_element* array_set(array* _array, array_element* e, int index) {
    if (-1 < index && index < _array->length) {
        int bytes = _array->isptr ? 1 : _array->elem_size;
        void** ptr = (void*)_array->data + (index * bytes);
        array_element* p = _array->isptr ? *ptr : NULL;
        memcpy((void*)_array->data + (index * bytes), _array->isptr ? &e : e, bytes);
        return p;
    } else {
        _array_debug("index out of range.\n");
        return NULL;
    }
}