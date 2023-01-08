#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>
#include <stdbool.h>



/* HashTable */

typedef struct HashTable HashTable;

HashTable* ht_make ();
void       ht_free (HashTable* hash);

void       ht_put (HashTable* ht, const char* key, void* value);
bool       ht_has (const HashTable* ht, const char* key);
void*      ht_get (const HashTable* ht, const char* key);
void*      ht_del (HashTable* ht, const char* key);

size_t     ht_length (const HashTable* ht);



/* HashTable Iterator */

typedef struct
{
    const char* key;
    void* value;
} HashTableIterator;

HashTableIterator* ht_itermake (const HashTable* it);
void               ht_iterfree (HashTableIterator* it);

bool               ht_hasnext  (HashTableIterator *it);
HashTableIterator* ht_next     (HashTableIterator *it);

#endif // HASHTABLE_H
