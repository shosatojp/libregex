#include "hashmap.h"

typedef struct {
    int x;
    int y;
} point;

int main() {
    {
        // non pointer int
        /* create map with capacity */
        hashmap* fruits = hashmap_new(sizeof(int), false, 2);

        /* insert key-value pair */
        hashmap_add(fruits, "apple", 5);
        hashmap_add(fruits, "apple", 5);
        hashmap_add(fruits, "orange", 2);
        hashmap_add(fruits, "melon", 3);
        hashmap_add(fruits, "banana", 4);

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
    {
        // pointer
        hashmap* fruits = hashmap_new(sizeof(int*), true, 1);

        int a = 1, b = 2, c = 3;

        hashmap_add(fruits, "apple", &a);
        hashmap_add(fruits, "orange", &b);
        hashmap_add(fruits, "melon", &c);

        hashmap_del(fruits, "apple");

        hashmap_each_i(fruits, {
            hash_entry* _entry = hashmap_ei;
            printf("%10s : %d\n", _entry->key, *(int*)_entry->ptr);
        });

        hashmap_destruct(fruits);
        free(fruits);
    }
    {
        // non pointer, struct
        hashmap* fruits = hashmap_new(sizeof(point), false, 1);

        point a;
        a.x = 1;
        a.y = 2;
        point b;
        b.x = 2;
        b.y = 4;

        hashmap_add(fruits, "apple", a);
        hashmap_add(fruits, "orange", b);

        hashmap_del(fruits, "apple");

        hashmap_each_i(fruits, {
            hash_entry* _entry = hashmap_ei;
            printf("%10s : %d\n", _entry->key, ((point*)_entry->ptr)->y);
        });

        hashmap_destruct(fruits);
        free(fruits);
    }
}