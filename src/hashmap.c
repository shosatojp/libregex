#include "hashmap.h"
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

static void debug(const char* format, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
#endif
}

char* strdup(const char* str) {
    int memlen = strlen(str) + 1;
    char* dest = (char*)malloc(sizeof(char) * memlen);
    memcpy(dest, str, memlen);
    return dest;
}

// http://www.cse.yorku.ca/~oz/hash.html#djb2
hashtype djb2(const char* str) {
    hashtype hash = 5381;
    int c;

    while (c = *(unsigned char*)str++)
        hash = ((hash << 5) + hash) + c;

    return hash;
}

// http://www.cse.yorku.ca/~oz/hash.html#sdbm
hashtype sdbm(const char* str) {
    hashtype hash = 0;
    int c;

    while (c = *(unsigned char*)str++)
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

hashmap* hashmap_new(int capacity) {
    hashmap* _m = (hashmap*)calloc(sizeof(hash_entry*), capacity);
    hashmap_init(_m, capacity);
    return _m;
}

int hashmap_init(hashmap* _m, int capacity) {
    _m->hash_entries = array_new(sizeof(hash_entry*), true, capacity);
    array_expand(_m->hash_entries, capacity);
    array_fill(_m->hash_entries, 0, _m->hash_entries->length - 1, 0);
    _m->capacity = capacity;
    _m->length = 0;
}

bool hashmap_add(hashmap* _m, const char* _key, void* _e) {
    if (hashmap_contains(_m, _key)) {
        debug("already contains '%s'\n", _key);
        return -2;
    } else {
        debug("add %s\n", _key);
        hash_entry* entry = hash_entry_new();
        entry->key = strdup(_key);
        entry->ptr = _e;

        if (((double)_m->length / (double)_m->capacity) > MAX_LOAD_FACTOR) {
            debug("load factor exceeded %3.g -> rehash\n", MAX_LOAD_FACTOR);
            _hashmap_rehash(_m, _m->capacity * _m->capacity);
        }

        while (_hashmap_add(_m, entry) < 0) {
            debug("no empty slot -> rehash\n");
            _hashmap_rehash(_m, _m->capacity * _m->capacity);
        }

        return true;
    }
}

int _hashmap_add(hashmap* _m, hash_entry* _e) {
    hashtype hash = djb2(_e->key) % _m->capacity;
    if (!array_at(_m->hash_entries, hash)) {
        _m->length++;
        array_set(_m->hash_entries, _e, hash);
    } else {
        hashtype hash2 = sdbm(_e->key) % _m->capacity,
                 count = 0;
        while (true) {
            hash = (hash + hash2) % _m->capacity;
            if (!array_at(_m->hash_entries, hash)) {
                _m->length++;
                array_set(_m->hash_entries, _e, hash);
                return 0;
            }
            if (count++ >= _m->capacity) return -1;
        }
    }
}

bool hashmap_contains(hashmap* _m, const char* key) {
    return !!hashmap_find(_m, key);
}

void* hashmap_find(hashmap* _m, const char* key) {
    hashtype index;
    hash_entry* entry = _hashmap_find(_m, key, &index);
    return entry ? entry->ptr : NULL;
}

hash_entry* _hashmap_find(hashmap* _m, const char* key, hashtype* index) {
    hashtype hash = djb2(key) % _m->capacity,
             hash2 = sdbm(key) % _m->capacity,
             count = 0;
    while (count < _m->length) {
        hash_entry* e = array_at(_m->hash_entries, hash);
        if (e) {
            if (!strcmp(e->key, key)) {
                *index = hash;
                return e;
            }
            count++;
            hash = (hash + hash2) % _m->capacity;
        } else
            break;
    }
    return NULL;
}

int hashmap_del(hashmap* _m, const char* key) {
    hashtype index = 0;
    hash_entry* entry = _hashmap_find(_m, key, &index);
    if (entry) {
        array_fill(_m->hash_entries, index, index, 0);
        hash_entry_destruct(entry);
        _m->length--;
    }
}

int _hashmap_rehash(hashmap* _m, int capacity) {
    debug("rehash %d\n", capacity);
    hashmap* _tmp = hashmap_new(capacity);
    for (hashtype i = 0; i < _m->capacity; ++i) {
        hash_entry* entry = array_at(_m->hash_entries, i);
        if (entry) _hashmap_add(_tmp, entry);
    }

    // copy _tmp to _m
    array_clear(_m->hash_entries);
    free(_m->hash_entries);
    _m->capacity = _tmp->capacity;
    _m->length = _tmp->length;
    _m->hash_entries = _tmp->hash_entries;
}

int hashmap_destruct(hashmap* _m) {
    if (_m) {
        array_each_i(_m->hash_entries,
                     hash_entry_destruct(array_ei));
        array_clear(_m->hash_entries);
    }
}

hash_entry* hash_entry_new() {
    hash_entry* _e = (hash_entry*)calloc(sizeof(hash_entry), 1);
    return _e;
}

int hash_entry_destruct(hash_entry* _e) {
    if (_e) {
        if (_e->key) free((char*)_e->key);
        free(_e);
    }
}
