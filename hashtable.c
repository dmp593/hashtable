#include "hashtable.h"
#include <string.h>

#define HT_INITIAL_SIZE 64

// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
#define FNV_OFFSET_BASIS 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

typedef struct
{
    char* key;
    void* value;
} Bucket;

struct HashTable
{
    Bucket* buckets;

    size_t size;
    size_t length;
};

typedef struct
{
    HashTableIterator it;
    
    const HashTable* ht;
    size_t index;
} HashTableIteratorImpl;

HashTable* ht_make()
{
    HashTable* ht = calloc(1, sizeof(HashTable));

    ht->buckets = calloc(HT_INITIAL_SIZE, sizeof(Bucket));
    ht->size = HT_INITIAL_SIZE;

    return ht;
}

void ht_free(HashTable* ht)
{
    for (size_t i = 0; i < ht->size; ++i)
    {
        free(ht->buckets[i].key);

        ht->buckets[i].key = NULL;
        ht->buckets[i].value = NULL;
    }

    free(ht->buckets);
    ht->length = 0;
    ht->size = 0;

    free(ht);
    ht = NULL;
}

// FNV-1a hash function
uint64_t ht_hash(const char* key) {
    uint64_t hash = FNV_OFFSET_BASIS;

    for (const char* ptr = key; *ptr; ++ptr)
    {
        hash ^= (uint64_t) (unsigned char) (*ptr);
        hash *= FNV_PRIME;
    }
    
    return hash;
}

size_t ht_index(const HashTable* ht, const char* key)
{
    size_t index = (size_t) (ht_hash(key) & (uint64_t) (ht->size - 1));

    while (ht->buckets[index].key != NULL) // collision... linear probing
    {
        if (strcmp(key, ht->buckets[index].key) == 0) return index; // already exists
        index = (index + 1) % ht->size;
    }

    return index;
}

void ht_expand(HashTable* ht)
{
    size_t old_size = ht->size;
    ht->size <<= 1;

    Bucket* buckets = ht->buckets;
    ht->buckets = calloc(ht->size, sizeof(Bucket));

    for (size_t i = 0; i < old_size; ++i)
    {
        if (buckets[i].key == NULL) continue;

        size_t index = ht_index(ht, buckets[i].key);

        ht->buckets[index].key = buckets[i].key;
        ht->buckets[index].value = buckets[i].value;
    }

    free(buckets);
}

void ht_put(HashTable* ht, const char *key, void* value)
{
    if (ht->length + 1 >= ht->size >> 1)
    {
        ht_expand(ht);
    }

    size_t index = ht_index(ht, key);

    if (ht->buckets[index].key == NULL)
    {
        ht->length++;
    }

    ht->buckets[index].key = strdup(key);
    ht->buckets[index].value = value;
}

bool ht_has(const HashTable* ht, const char* key)
{
    Bucket* bucket = ht->buckets + ht_index(ht, key);
    return bucket->key && strcmp(key, bucket->key) == 0;
}

void* ht_get(const HashTable* ht, const char* key)
{
    return ht->buckets[ht_index(ht, key)].value;
}

void* ht_del(HashTable* ht, const char* key)
{
    size_t index = ht_index(ht, key);
    Bucket* bucket = ht->buckets + index;

    if (!bucket->key || strcmp(key, bucket->key) != 0) return NULL;

    free(bucket->key);
    bucket->key = NULL;

    void* value = bucket->value;
    bucket->value = NULL;

    while (ht->buckets[++index].key)
    {
        size_t new_index = ht_index(ht, ht->buckets[index].key);
        if (new_index == index) continue;

        Bucket tmp = { ht->buckets[index].key, ht->buckets[index].value };

        ht->buckets[index].key = NULL;
        ht->buckets[index].value = NULL;

        ht->buckets[new_index].key = tmp.key;
        ht->buckets[new_index].value = tmp.value;
    }

    ht->length--;
    return value;
}

size_t ht_length(const HashTable* ht)
{
    return ht->length;
}

HashTableIterator* ht_itermake(const HashTable* ht)
{
    HashTableIteratorImpl* impl = calloc(1, sizeof(HashTableIteratorImpl));

    impl->it.key = 0;
    impl->it.value = 0;

    impl->ht = ht;
    impl->index = -1;

    return &impl->it;
}

void ht_iterfree(HashTableIterator *it)
{
    if (!it) return;

    HashTableIteratorImpl* pimpl = (HashTableIteratorImpl*) it;

    pimpl->it.key = 0;
    pimpl->it.value = 0;
    pimpl->ht = NULL;
    pimpl->index = -1;
    
    free(pimpl);
    pimpl = NULL;
}

bool ht_hasnext(HashTableIterator *it)
{
    HashTableIteratorImpl* pimpl = (HashTableIteratorImpl*) it;

    for (size_t i = pimpl->index + 1; i < pimpl->ht->size; ++i)
    {
        if (pimpl->ht->buckets[i].key != NULL) return true;
    }

    return false;
}

HashTableIterator* ht_next(HashTableIterator* it)
{
    HashTableIteratorImpl* pimpl = (HashTableIteratorImpl*) it;

    while(++pimpl->index < pimpl->ht->size)
    {
        if (pimpl->ht->buckets[pimpl->index].key != NULL)
        {
            pimpl->it.key = pimpl->ht->buckets[pimpl->index].key;
            pimpl->it.value = pimpl->ht->buckets[pimpl->index].value;

            return &pimpl->it;
        }
    }

    ht_iterfree(it);
    return NULL;
}
