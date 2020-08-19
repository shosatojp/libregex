#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#define MAX_LOAD_FACTOR 0.4

typedef unsigned long hashtype;

typedef struct {
    const char* key;
    void* ptr;
} hash_entry;

typedef struct {
    int elem_size;
    array* hash_entries;
    hashtype capacity;
    hashtype length;
    bool isptr;
} hashmap;

/* hashmap */
hashmap* hashmap_new(int elem_size, bool isptr, int capacity);
int hashmap_init(hashmap* _m, int elem_size, bool isptr, int capacity);
bool _hashmap_add(hashmap* _m, const char* _key, ...);
int basic_hashmap_add(hashmap* _m, hash_entry* _e);
bool hashmap_contains(hashmap* _m, const char* key);
void* hashmap_find(hashmap* _m, const char* key);
hash_entry* _hashmap_find(hashmap* _m, const char* key, hashtype* index);
void* hashmap_del(hashmap* _m, const char* key);
int _hashmap_rehash(hashmap* _m, int capacity);
int hashmap_destruct(hashmap* _m);

/* hash_entry */
hash_entry* hash_entry_new();
int hash_entry_destruct(hash_entry* _e, bool isptr);

/* macros */
#define _hashmap_each(INDEX, M, STMT)                                                               \
    for (int hashmap_##INDEX = 0; hashmap_##INDEX < (M)->hash_entries->length; hashmap_##INDEX++) { \
        hash_entry* hashmap_e##INDEX = array_at((M)->hash_entries, hashmap_##INDEX);                \
        if (hashmap_e##INDEX) {                                                                     \
            STMT;                                                                                   \
        }                                                                                           \
    }

#define hashmap_i hashmap_i
#define hashmap_ei hashmap_ei
#define hashmap_each_i(M, STMT) _hashmap_each(i, M, STMT)

#define hashmap_j hashmap_j
#define hashmap_ej hashmap_ej
#define hashmap_each_j(M, STMT) _hashmap_each(j, M, STMT)

#define hashmap_k hashmap_k
#define hashmap_ek hashmap_ek
#define hashmap_each_k(M, STMT) _hashmap_each(k, M, STMT)

#define hashmap_add(_m, _key, _e) _hashmap_add(_m, _key, _e)
