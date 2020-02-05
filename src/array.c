#include "array.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _array_debug(const char* format, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
#endif
}

array* array_new(int elem_size, bool isptr, indextype capacity) {
    array* _array = (array*)calloc(sizeof(array), 1);
    array_init(_array, elem_size, isptr);
    array_expand(_array, capacity);
    return _array;
}

int array_init(array* _array, int elem_size, bool isptr) {
    memset(_array, 0, sizeof(array));
    _array->isptr = isptr;
    _array->elem_size = isptr ? sizeof(void*) : elem_size;
    return 0;
}

int array_expand(array* _array, indextype capacity) {
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

int array_fill(array* _array, indextype _begin, indextype _end, char _c) {
    _array->length = _array->capacity;
    memset((char*)_array->data + (_begin * _array->elem_size), _c,
           (_end - _begin + 1) * _array->elem_size);
}

int _array_ins(array* _array, indextype index, ...) {
    if (index > _array->length || index < 0) {
        _array_debug("index out of range.\n");
        return -1;
    } else if (_array->capacity < _array->length + 1 &&
               array_expand(_array, (_array->capacity ? _array->capacity : DEFAULT_CAPACITY) * 2) < 0) {
        _array_debug("failed to expand.\n");
        return -1;
    } else {
        va_list args;
        va_start(args, index);
        char* elem_ptr = va_arg(args, char*);
        va_end(args);

        memmove((char*)_array->data + (index + 1) * _array->elem_size,
                (char*)_array->data + index * _array->elem_size,
                (_array->length - index) * _array->elem_size);
        char* ptr = (char*)_array->data + (index * _array->elem_size);
        for (int i = 0; i < _array->elem_size; ++i) {
            *(ptr + i) = *((char*)&elem_ptr + i);
        }
        _array->length++;
        return 0;
    }
}

/**
 * you need to **destruct** object pointed by deleted ptr
 */
array_element* array_del(array* _array, indextype index) {
    if (index >= _array->length || index < 0) {
        _array_debug("index out of range.\n");
        return NULL;
    } else {
        void** head = (void**)((char*)_array->data + (index * _array->elem_size));
        array_element* ptr = _array->isptr ? *head : NULL;
        memmove(head, (char*)head + _array->elem_size,
                (_array->length - index - 1) * _array->elem_size);
        memset((char*)_array->data + (_array->length - 1) * _array->elem_size, 0, _array->elem_size);
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

void* array_at(array* _array, indextype index) {
    void** ptr = (void*)((char*)_array->data + (index * _array->elem_size));
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
array_element* array_set(array* _array, array_element* e, indextype index) {
    if (0 <= index && index < _array->length) {
        void** ptr = (void*)((char*)_array->data + (index * _array->elem_size));
        array_element* p = _array->isptr ? *ptr : NULL;
        memcpy((char*)_array->data + (index * _array->elem_size),
               _array->isptr ? &e : e, _array->elem_size);
        return p;
    } else {
        _array_debug("index out of range.\n");
        return NULL;
    }
}