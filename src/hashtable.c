#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashtable.h"
#include "list.h"

static void __hashtable_kvp_deallocator(void *kvp, void *context)
{
    hashtable_t *hashtable = (hashtable_t *)context;
    hashtable->deallocator(kvp);
}

hashtable_t *hashtable_init(unsigned int num_slots,
                            unsigned int (*hash_function)(void *),
                            int (*key_comparator)(void *, void *),
                            void *(*allocator)(size_t),
                            void (*deallocator)(void *))
{
    if (num_slots == 0)
    {
        return NULL;
    }

    hashtable_t *hashtable = (hashtable_t *)allocator(sizeof(hashtable_t));
    if (hashtable == NULL)
    {
        return NULL;
    }

    hashtable->lists = (list_t **)allocator(num_slots * sizeof(list_t *));
    if (NULL == hashtable->lists)
    {
        deallocator(hashtable);
        return NULL;
    }

    // init each list for each slot
    int i = 0;
    for (i = 0; i < num_slots; i++)
    {
        if ((hashtable->lists[i] = list_init(allocator, deallocator)) == NULL)
        {
            int j;
            for (j = 0; j < i; j++)
            {
                list_destroy(hashtable->lists[j]);
            }
            deallocator(hashtable->lists);
            deallocator(hashtable);
            return NULL;
        }
    }

    // store pointers to user-defined hash function pointers,
    // key comparators and deallocators
    hashtable->hash_function  = hash_function;
    hashtable->key_comparator = key_comparator;
    hashtable->allocator      = allocator;
    hashtable->deallocator    = deallocator;
    hashtable->num_slots      = num_slots;
    return hashtable;
}

int hashtable_add(hashtable_t *hashtable, void *key, void *value)
{
    int     index = hashtable->hash_function(key) % hashtable->num_slots;
    list_t *list_at_index = hashtable->lists[index];
    int     i;
    for (i = 0; i < list_count(list_at_index); i++)
    {
        kvp_t *kvp = list_get_at_index(list_at_index, i);
        if (hashtable->key_comparator(kvp->key, key))
        {
            return 2;   // already here
        }
    }
    kvp_t *new_kvp = hashtable->allocator(sizeof(kvp_t));
    if (!new_kvp)
    {
        return 0;
    }
    new_kvp->key   = key;
    new_kvp->value = value;
    return list_add(list_at_index, new_kvp) == 1;
}

int hashtable_remove(hashtable_t *hashtable, void *key)
{
    int     index = hashtable->hash_function(key) % hashtable->num_slots;
    list_t *list_at_index = hashtable->lists[index];
    int     i;
    for (i = 0; i < list_count(list_at_index); i++)
    {
        kvp_t *kvp = list_get_at_index(list_at_index, i);
        if (hashtable->key_comparator(kvp->key, key))
        {
            list_remove(list_at_index, kvp);
            hashtable->deallocator(kvp);
            return 1;
        }
    }
    return 0;
}

void *hashtable_get(hashtable_t *hashtable, void *key)
{
    int     index = hashtable->hash_function(key) % hashtable->num_slots;
    list_t *list_at_index = hashtable->lists[index];
    int     i;
    for (i = 0; i < list_count(list_at_index); i++)
    {
        kvp_t *kvp = list_get_at_index(list_at_index, i);
        if (hashtable->key_comparator(kvp->key, key))
        {
            return kvp->value;   // already here
        }
    }
    return NULL;
}

int hashtable_destroy(hashtable_t *hashtable)
{
    int32_t i;
    for (i = 0; i < hashtable->num_slots; i++)
    {
        // free up each kvp element within the list
        list_foreach(hashtable->lists[i], __hashtable_kvp_deallocator,
                     hashtable);
        // then destroy the actual list itself
        list_destroy(hashtable->lists[i]);
    }
    hashtable->deallocator(hashtable->lists);
    hashtable->deallocator(hashtable);
    return 1;
}
