#include "hashmap.h"

int main() {
    /* create map with capacity */
    hashmap* fruits = hashmap_new(2);

    /* insert key-value pair */
    int a = 5;
    hashmap_add(fruits, "apple", &a);
    hashmap_add(fruits, "apple", &a);
    int b = 2;
    hashmap_add(fruits, "orange", &b);
    int c = 3;
    hashmap_add(fruits, "melon", &c);
    int d = 4;
    hashmap_add(fruits, "banana", &d);

    /* delete key */
    hashmap_del(fruits, "banana");
    hashmap_del(fruits, "hoge");

    /* check if contains */
    printf("orange : %d\n", hashmap_contains(fruits, "orange"));
    printf("grapes : %d\n", hashmap_contains(fruits, "grapes"));

    /* find */
    int* e = hashmap_find(fruits, "apple");
    if (e) {
        printf("find apple = %d\n", *e);
    } else {
        printf("not found\n");
    }

    /* iterate map */
    hashmap_each_i(fruits, {
        hash_entry* _entry = hashmap_ei;
        printf("%10s : %d\n", _entry->key, *(int*)_entry->ptr);
    });

    /* finalize */
    hashmap_destruct(fruits);
    free(fruits);
}