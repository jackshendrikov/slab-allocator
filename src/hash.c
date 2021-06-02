#include <stdint.h>
#include <stdio.h>

#include "slab.h"
#include "hash.h"

struct kmem_hash *
kmem_hash_init(struct kmem_cache *hash_cache, struct kmem_cache *node_cache)
{
        DEBUG_PRINT("Allocating hash from cache %s\n", hash_cache->name);
        struct kmem_hash *hash = kmem_cache_alloc(hash_cache, KM_NOSLEEP);
        if (!hash)
            DEBUG_PRINT("Unable to init hash...\n");

        hash->node_cache = node_cache;
        memset(hash->buckets, 0, sizeof(struct kmem_hash_node*) * NUM_BUCKETS);

        return hash;
}

void
kmem_hash_free(struct kmem_cache *hash_cache, struct kmem_hash *hash)
{
        struct kmem_hash_node *node;
        struct kmem_hash_node *temp;
        int i;

        for (i = 0; i < NUM_BUCKETS; i++) {
                node = hash->buckets[i];
                while(node) {
                        temp = node->next;
                        kmem_cache_free(hash->node_cache, node);
                        node = temp;
                }
        }

        kmem_cache_free(hash_cache, hash);
}

void
kmem_hash_insert(struct kmem_hash *hash, void *key, void *data)
{
        unsigned bucket;
        struct kmem_hash_node *node;
        struct kmem_hash_node *old_head;

        bucket = (uintptr_t) key % NUM_BUCKETS;

        node = kmem_cache_alloc(hash->node_cache, KM_SLEEP);
        node->bufaddr = key;
        node->value = data;

        old_head = hash->buckets[bucket];
        hash->buckets[bucket] = node;
        node->next = old_head;
}

void *
kmem_hash_get(struct kmem_hash *hash, void *key)
{
        unsigned bucket;
        struct kmem_hash_node *node;

        bucket = (uintptr_t)key % NUM_BUCKETS;
        node = hash->buckets[bucket];
        while (node) {
                if (node->bufaddr == key) return node->value;
                node = node->next;
        }

        return NULL;
}

void
kmem_hash_remove(struct kmem_hash *hash, void *bufaddr)
{
        uintptr_t bucket;
        struct kmem_hash_node *last;
        struct kmem_hash_node *node;

        bucket = (uintptr_t) bufaddr % NUM_BUCKETS;
        last = NULL;
        node = hash->buckets[bucket];
        while (node) {
                if (node->bufaddr == bufaddr) {
                        if (last)
                            last->next = node->next;
                        else
                            hash->buckets[bucket] = node->next;
                        kmem_cache_free(hash->node_cache, node);
                        return;
                }
                last = node;
                node = node->next;
        }
}
