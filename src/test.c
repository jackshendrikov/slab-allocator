#include <stdio.h>
#include <stdint.h>
#include "slab.h"
#include "hash.h"

struct big_foo {
        int nums[128];
};

struct foo {
        int a;
        int b;
        int c;
};

int
main()
{
        struct foo *datas[340];
        struct big_foo *big_datas[10];
        struct kmem_cache *cache = kmem_cache_create("test_cache", sizeof(struct foo), 0);
        printf("Cache address: %p\n\n", (void*)cache);
        struct foo *test_struct1 = kmem_cache_alloc(cache, KM_SLEEP);
        printf("Allocated item at %p\n\n", (void*)test_struct1);

        struct foo *test_struct2 = kmem_cache_alloc(cache, KM_SLEEP);
        printf("Allocated item at %p\n\n", (void*)test_struct2);

        test_struct1->a = 2;
        test_struct1->b = 4;
        test_struct1->c = 10;

        test_struct2->a = 1;
        test_struct2->b = 5;
        test_struct2->c = 11;

        printf("a + b + c = %d, expected = 16\n", (test_struct1->a + test_struct1->b + test_struct1->c));
        printf("a + b + c = %d, expected = 17\n", (test_struct2->a + test_struct2->b + test_struct2->c));
        printf("a + b + c = %d, expected = 16\n\n", (test_struct1->a + test_struct1->b + test_struct1->c));

        kmem_cache_free(cache, test_struct1);
        kmem_cache_free(cache, test_struct2);
        for (int i = 0; i < 340; i++) {
                datas[i] = kmem_cache_alloc(cache, KM_SLEEP);
                datas[i]->a = i;
                datas[i]->b = i*i;
                datas[i]->c = 3*i;
        }
        printf("Lots of small objects: %d, expected 19", (datas[3]->a + datas[4]->b));

        printf("\nfreeing the first slab\n");
        for (int i = 0; i < 338; i++) {
                kmem_cache_free(cache, datas[i]);
        }

        printf("Num slabs: %d\n", cache->slab_count);
        kmem_cache_destroy(cache);

        printf("\n----------\nTesting Hash Table\n----------\n\n");
        int test = 7;
        int test2 = 8;
        kmem_hash_insert(cache->hash, &test, &test2);
        int *res = kmem_hash_get(cache->hash, &test);
        printf("Result: %d", *res);

        printf("\n----------\nTesting Big Cache\n----------\n\n");
        struct kmem_cache *big_cache = kmem_cache_create("test_struct2", sizeof(struct big_foo), 0);
        for (int i = 0; i < 10; i++) {
                big_datas[i] = kmem_cache_alloc(big_cache, KM_SLEEP);
                big_datas[i]->nums[0] = i;
        }

        printf("Test value %d, expected 9\n", big_datas[2]->nums[0] + big_datas[7]->nums[0]);

        for (int i = 0; i < 10; i++) {
                kmem_cache_free(big_cache, big_datas[i]);
        }
        kmem_cache_destroy(big_cache);
}
