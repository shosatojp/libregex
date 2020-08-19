#include "hashmap.h"
#include "time.h"

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
    {
        int n = 1000;
        for (int k = 3; k < 7; k++, n *= 10) {
            struct timespec f, t;
            timespec_get(&f, TIME_UTC);
            hashmap* fruits = hashmap_new(sizeof(int), false, 100000000);
            for (int i = 0; i < n; i++) {
                /* insert key-value pair */
                char str[20] = {0};
                sprintf(str, "%d", i);
                hashmap_add(fruits, str, i);

                int* e = hashmap_find(fruits, str);
                // if (e) printf("%d\n", *e);
            }

            timespec_get(&t, TIME_UTC);
            long ns;
            long long s;

            if (f.tv_nsec < t.tv_nsec) {
                ns = t.tv_nsec - f.tv_nsec;
                s = t.tv_sec - f.tv_sec;
            } else {
                ns = 1000000000 + t.tv_nsec - f.tv_nsec;
                s = t.tv_sec - f.tv_sec - 1;
            }
            printf("%lld.%09ld\n", s, ns);
        }
    }
}