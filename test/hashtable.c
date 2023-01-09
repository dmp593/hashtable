#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../hashtable.h"


typedef struct
{
    int total;
    int fails;

    clock_t start;
    clock_t end;
} TestCounter;


#define TEST_INIT() \
{ \
    TestCounter _c = { .total = 0, .fails = 0, .start = clock(), .end = 0 };\
    printf("\n\n************* STARTING TESTS *************\n\n");

#define TEST(test_fn, name) { printf("%s:\t\t", name); if(!test_fn()) { printf("FAILED\n"); ++_c.fails; } else { printf("PASSED\n"); } ++_c.total; }

#define TEST_STOP() \
    _c.end = clock(); \
    printf("\n\n************** TESTS REPORT **************\n\n");\
    printf("\U00002705 PASSED:\t\t\t%4d\n", _c.total - _c.fails);\
    printf("\U0000274C FAILED:\t\t\t%4d\n", _c.fails);\
    printf("------------------------------------------\n");\
    printf("\U0001F9EA  TOTAL:\t\t\t%4d\n", _c.total);\
    printf("\U0001F551   TIME:\t\t\t%4.2f secs\n\n", ((double) (_c.end - _c.start)) / CLOCKS_PER_SEC);\
    printf("\n******************************************\n\n");\
} \


bool test_ht_make()
{
    HashTable* ht = ht_make();

    if (!ht) return false;

    free(ht);
    return true;
}

bool test_ht_put()
{
    HashTable* ht = ht_make();

    ht_put(ht, "foo", "bar");

    if (!ht_has(ht, "foo") || strcmp(ht_get(ht, "foo"), "bar") != 0)
    {
        free(ht);
        return false;
    }
    
    free(ht);
    return true;
}

bool test_ht_del()
{
    HashTable* ht = ht_make();
    
    if (ht_has(ht, "foo"))
    {
        free(ht);
        return false;
    }
    
    ht_put(ht, "foo", "bar");

    if (! ht_has(ht, "foo"))
    {
        free(ht);
        return false;
    }

    void* value = ht_del(ht, "foo");
    if(strcmp((char*) value, "bar") != 0)
    {
        free(ht);
        return false;
    }

    value = ht_get(ht, "foo");
    if (value != NULL)
    {
        free(ht);
        return false;
    }

    free(ht);
    return true;
}

bool test_ht_has()
{
    HashTable* ht = ht_make();

    if (ht_has(ht, "monkey"))
    {
        free(ht);
        return false;
    }

    ht_put(ht, "monkey", "patch");

    if (!ht_has(ht, "monkey"))
    {
        free(ht);
        return false;
    }

    free(ht);
    return true;
}

bool test_ht_replace()
{
    HashTable* ht = ht_make();

    ht_put(ht, "magic", "wizard");
    if (strcmp(ht_get(ht, "magic"), "wizard") != 0)
    {
        free(ht);
        return false;
    }

    ht_put(ht, "magic", "unicorn");
    if (strcmp(ht_get(ht, "magic"), "unicorn") != 0)
    {
        free(ht);
        return false;
    }

    free(ht);
    return true;
}

bool test_ht_length()
{
    HashTable* ht = ht_make();

    if (ht_length(ht) != 0)
    {
        free(ht);
        return false;
    }

    ht_put(ht, "foo", "bar");
    if (ht_length(ht) != 1)
    {
        free(ht);
        return false;
    }

    ht_put(ht, "dog", "border collie");
    if (ht_length(ht) != 2)
    {
        free(ht);
        return false;
    }

    ht_put(ht, "cat", "maine coon");
    if (ht_length(ht) != 3)
    {
        free(ht);
        return false;
    }

    ht_put(ht, "foo", "dummy");
    if (ht_length(ht) != 3)
    {
        free(ht);
        return false;
    }

    ht_del(ht, "foo");
    if (ht_length(ht) != 2)
    {
        free(ht);
        return false;
    }

    ht_del(ht, "foo");
    if (ht_length(ht) != 2)
    {
        free(ht);
        return false;
    }

    ht_del(ht, "dog");
    ht_del(ht, "cat");

    if (ht_length(ht) != 0)
    {
        free(ht);
        return false;
    }

    for (int i = 0; i < 50; ++i)
    {
        ht_del(ht, "foo");
        ht_del(ht, "dog");
        ht_del(ht, "cat");
    }

    if (ht_length(ht) != 0)
    {
        free(ht);
        return false;
    }

    free(ht);
    return true;
}

bool test_ht_expand()
{
    HashTable* ht = ht_make();

    for (int i = 0; i < 500; ++i)
    {
        char key[10] = {0};
        sprintf(key, "key-%d", i);
        ht_put(ht, key, "lorem");
    }

    if(ht_length(ht) != 500)
    {
        free(ht);
        return false;
    }

    free(ht);
    return true;
}

bool test_ht_stress()
{
    HashTable* ht = ht_make();

    char keys[50000][10] = {0};
    for (int i = 0; i < 50000; ++i)
    {
        sprintf(keys[i], "key-%d", i);
        ht_put(ht, keys[i], "lorem");
    }

    if(ht_length(ht) != 50000)
    {
        free(ht);
        return false;
    }

    free(ht);
    return true;
}


bool test_ht_consistency()
{
    HashTable* ht = ht_make();

    char key[10] = {0};

    for (int i = 0; i < 32; ++i)
    {
        sprintf(key, "key-%d", i);
        ht_put(ht, key, "lorem");
    }

    for (int i = 0; i < 32; ++i)
    {
        if (i % 2 == 0)
        {
            sprintf(key, "key-%d", i);
            ht_del(ht, key);
        }
    }

    for (int i = 0; i < 32; ++i)
    {
        if (i % 2 != 0)
        {
            sprintf(key, "key-%d", i);
            if (! ht_has(ht, key))
            {
                free(ht);
                return false;
            }
        }
    }

    free(ht);
    return true;
}


int main()
{
    TEST_INIT();

    TEST(test_ht_make, "Make an HashTable");
    TEST(test_ht_del, "Delete Value With Key");
    TEST(test_ht_has, "Has Value With Key");
    TEST(test_ht_put, "Put (Key, Value)");
    TEST(test_ht_replace, "Replace Existing Value");
    TEST(test_ht_length, "HashTable Length");
    TEST(test_ht_expand, "Test Expand HashTable");
    TEST(test_ht_stress, "Stress Test (50K)");
    TEST(test_ht_consistency, "Consistency deleting");

    TEST_STOP();
    return 0;
}
