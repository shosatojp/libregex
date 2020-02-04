#include "hashmap.h"

int main() {
    /* create map with capacity */
    hashmap* fruits = hashmap_new(2);

    /* insert key-value pair */
    int a = 1;
    hashmap_add(fruits, "apple", &a);
    hashmap_add(fruits, "apple", &a);
    int b = 2;
    hashmap_add(fruits, "orange", &b);
    int c = 3;
    hashmap_add(fruits, "melon", &c);

    /* rehash */
    // hashmap_rehash(fruits, 10);
    // hashmap_rehash(fruits, 100);

    /* retry inserting after rehash */
    int d = 4;
    hashmap_add(fruits, "banana", &d);

    /* find */
    int* e = hashmap_find(fruits, "apple");
    if (e) {
        printf("find apple = %d\n", *e);
    } else {
        printf("not found\n");
    }

    printf("--------------\n");

    /* iterate map */
    hashmap_each_i(fruits, {
        hash_entry* _entry = hashmap_ei;
        printf("%s : %d\n", _entry->key, *(int*)_entry->ptr);
    });

    hashmap_destruct(fruits);
    free(fruits);
}