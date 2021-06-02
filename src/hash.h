#ifndef SLAB_HASH_H
#define SLAB_HASH_H

#include <string.h>

#include "slab.h"

/**
 * Basic hash table implementation.
 * It provides the mapping between buf -> bufctl for larger caches.
 */
#define NUM_BUCKETS 32

struct kmem_hash_node {
        void *bufaddr;                /* Address of the membuf */
        void *value;                  /* Address of the bufctl or slab */
        struct kmem_hash_node *next;  /* Next item in the list */
};

struct kmem_hash {
        struct kmem_hash_node *buckets[NUM_BUCKETS];
        struct kmem_cache *node_cache;
};

struct kmem_hash *
kmem_hash_init(struct kmem_cache *hash_cache, struct kmem_cache *node_cache);

void
kmem_hash_free(struct kmem_cache *hash_cache, struct kmem_hash *hash);

/**
 * Insert a bufctl into the hash table
 * ASSUMES it is not already present
 */
void
kmem_hash_insert(struct kmem_hash *hash, void *bufaddr, void *data);

/**
 * Get a bufctl from a given membuf address
 * Returns NULL if not found
 */
void *
kmem_hash_get(struct kmem_hash *hash, void *bufaddr);

/**
 * Remove the bufctl for the given address from the table
 */
void
kmem_hash_remove(struct kmem_hash *hash, void *bufaddr);

#endif
