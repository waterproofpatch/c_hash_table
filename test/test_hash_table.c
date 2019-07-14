#include <stddef.h>
#include <stdlib.h>

#include "unity.h"

// mocks
#include "mock_std_lib_wrapper.h"

// code under test
#include "hashtable.h"
#include "list.h"

typedef struct _test_key_t
{
    int num;
} test_key_t;

typedef struct _test_value_t
{
    int    a;
    int    b;
    double c;
} test_value_t;

int g_allocs = 0;
int g_frees  = 0;

void setUp()
{
    g_allocs = 0;
    g_frees  = 0;
}

void tearDown()
{
    TEST_ASSERT_EQUAL(0, g_allocs - g_frees);
}

void* allocator_stub(size_t size, int num_calls)
{
    g_allocs++;
    return malloc(size);
}

void deallocator_stub(void* ptr, int num_calls)
{
    g_frees++;
    free(ptr);
    return;
}

unsigned int hash_func(void* key)
{
    test_key_t* test_key = (test_key_t*)key;
    return (test_key->num ^ 123);
}

int key_comparator(void* k1, void* k2)
{
    test_key_t* test_key_1 = (test_key_t*)k1;
    test_key_t* test_key_2 = (test_key_t*)k2;
    return test_key_1->num == test_key_2->num;
}

void testHashTableInit()
{
    wrap_malloc_StubWithCallback(allocator_stub);
    wrap_free_StubWithCallback(deallocator_stub);
    hashtable_t* hash_table =
        hashtable_init(100, hash_func, key_comparator, wrap_malloc, wrap_free);
    TEST_ASSERT_NOT_NULL(hash_table);
    TEST_ASSERT_EQUAL(1, hashtable_destroy(hash_table));
}

void testHashTableInit0Slots()
{
    wrap_malloc_StubWithCallback(allocator_stub);
    wrap_free_StubWithCallback(deallocator_stub);
    hashtable_t* hash_table =
        hashtable_init(0, hash_func, key_comparator, wrap_malloc, wrap_free);
    TEST_ASSERT_NULL(hash_table);
}

void testHashTableAddAllocFail()
{
    void* alloc_1 = malloc(sizeof(hashtable_t));
    void* alloc_2 = malloc(sizeof(list_t*) * 5);
    void* alloc_3 = malloc(sizeof(list_t));
    void* alloc_4 = malloc(sizeof(list_t));
    void* alloc_5 = malloc(sizeof(list_t));
    void* alloc_6 = malloc(sizeof(list_t));
    void* alloc_7 = malloc(sizeof(list_t));
    wrap_malloc_ExpectAndReturn(sizeof(hashtable_t), alloc_1);
    wrap_malloc_ExpectAndReturn(sizeof(list_t*) * 5, alloc_2);
    wrap_malloc_ExpectAndReturn(sizeof(list_t), alloc_3);
    wrap_malloc_ExpectAndReturn(sizeof(list_t), alloc_4);
    wrap_malloc_ExpectAndReturn(sizeof(list_t), alloc_5);
    wrap_malloc_ExpectAndReturn(sizeof(list_t), alloc_6);
    wrap_malloc_ExpectAndReturn(sizeof(list_t), alloc_7);
    wrap_malloc_ExpectAndReturn(sizeof(kvp_t), NULL);
    hashtable_t* hash_table =
        hashtable_init(5, hash_func, key_comparator, wrap_malloc, wrap_free);
    TEST_ASSERT_NOT_NULL(hash_table);
    test_value_t v1 = {1, 2, 3.0};
    test_key_t   k1 = {1337};
    TEST_ASSERT_EQUAL(0, hashtable_add(hash_table, &k1, &v1));
    free(alloc_1);
    free(alloc_2);
    free(alloc_3);
    free(alloc_4);
    free(alloc_5);
    free(alloc_6);
    free(alloc_7);
}

void testHashTableInit5SlotsAndAdd1Item()
{
    wrap_malloc_StubWithCallback(allocator_stub);
    wrap_free_StubWithCallback(deallocator_stub);
    hashtable_t* hash_table =
        hashtable_init(5, hash_func, key_comparator, wrap_malloc, wrap_free);
    TEST_ASSERT_NOT_NULL(hash_table);
    test_value_t v1  = {1, 2, 3.0};
    test_key_t   k1  = {1337};
    int          res = hashtable_add(hash_table, &k1, &v1);
    TEST_ASSERT_EQUAL(1, hashtable_destroy(hash_table));
}

void testHashTableAddAndGetOneItem()
{
    wrap_malloc_StubWithCallback(allocator_stub);
    wrap_free_StubWithCallback(deallocator_stub);
    hashtable_t* hash_table =
        hashtable_init(5, hash_func, key_comparator, wrap_malloc, wrap_free);
    TEST_ASSERT_NOT_NULL(hash_table);
    test_value_t v1  = {1, 2, 3.0};
    test_key_t   k1  = {1337};
    int          res = hashtable_add(hash_table, &k1, &v1);
    TEST_ASSERT_EQUAL(1, res);
    test_value_t* res_val = (test_value_t*)hashtable_get(hash_table, &k1);
    TEST_ASSERT_NOT_NULL(res_val);
    TEST_ASSERT_EQUAL(v1.a, res_val->a);
    TEST_ASSERT_EQUAL(1, hashtable_destroy(hash_table));
}

void testHashTableAddElementTwice()
{
    wrap_malloc_StubWithCallback(allocator_stub);
    wrap_free_StubWithCallback(deallocator_stub);
    hashtable_t* hash_table =
        hashtable_init(5, hash_func, key_comparator, wrap_malloc, wrap_free);
    TEST_ASSERT_NOT_NULL(hash_table);
    test_value_t v1 = {1, 2, 3.0};
    test_key_t   k1 = {1337};
    TEST_ASSERT_EQUAL(1, hashtable_add(hash_table, &k1, &v1));
    TEST_ASSERT_EQUAL(2, hashtable_add(hash_table, &k1, &v1));
    TEST_ASSERT_EQUAL(1, hashtable_destroy(hash_table));
}

void testHashTableFailInitASecondList()
{
    void* alloc_1 = (hashtable_t*)malloc(sizeof(hashtable_t));
    void* alloc_2 = (list_t**)malloc(sizeof(list_t*) * 5);
    void* alloc_3 = (list_t*)malloc(sizeof(list_t));
    wrap_malloc_ExpectAndReturn(sizeof(hashtable_t), alloc_1);
    wrap_malloc_ExpectAndReturn(sizeof(list_t*) * 5, alloc_2);
    wrap_malloc_ExpectAndReturn(sizeof(list_t), alloc_3);
    wrap_malloc_ExpectAndReturn(sizeof(list_t), NULL);
    wrap_free_Expect(alloc_3);
    wrap_free_Expect(alloc_2);
    wrap_free_Expect(alloc_1);
    hashtable_t* hash_table =
        hashtable_init(5, hash_func, key_comparator, wrap_malloc, wrap_free);
    TEST_ASSERT_NULL(hash_table);
    free(alloc_1);
    free(alloc_2);
    free(alloc_3);
}
void testHashTableFailInitAList()
{
    void* alloc_1 = (hashtable_t*)malloc(sizeof(hashtable_t));
    void* alloc_2 = (list_t**)malloc(sizeof(list_t*) * 5);
    wrap_malloc_ExpectAndReturn(sizeof(hashtable_t), alloc_1);
    wrap_malloc_ExpectAndReturn(sizeof(list_t*) * 5, alloc_2);
    wrap_malloc_ExpectAndReturn(sizeof(list_t), NULL);
    wrap_free_Expect(alloc_1);
    wrap_free_Expect(alloc_2);
    hashtable_t* hash_table =
        hashtable_init(5, hash_func, key_comparator, wrap_malloc, wrap_free);
    TEST_ASSERT_NULL(hash_table);
    free(alloc_1);
    free(alloc_2);
}

void testHashTableFailInitLists()
{
    void* alloc_1 = (hashtable_t*)malloc(sizeof(hashtable_t));
    wrap_malloc_ExpectAndReturn(sizeof(hashtable_t), alloc_1);
    wrap_malloc_ExpectAndReturn(sizeof(list_t*) * 5, NULL);
    wrap_free_Expect(alloc_1);
    hashtable_t* hash_table =
        hashtable_init(5, hash_func, key_comparator, wrap_malloc, wrap_free);
    TEST_ASSERT_NULL(hash_table);
    free(alloc_1);
}

void testHashTableMallocFailInit()
{
    wrap_malloc_ExpectAndReturn(sizeof(hashtable_t), NULL);
    hashtable_t* hash_table =
        hashtable_init(5, hash_func, key_comparator, wrap_malloc, wrap_free);
    TEST_ASSERT_NULL(hash_table);
}

void testHashTableRemove1Element()
{
    wrap_malloc_StubWithCallback(allocator_stub);
    wrap_free_StubWithCallback(deallocator_stub);
    hashtable_t* hash_table =
        hashtable_init(5, hash_func, key_comparator, wrap_malloc, wrap_free);
    TEST_ASSERT_NOT_NULL(hash_table);
    test_value_t v1 = {1, 2, 3.0};
    test_key_t   k1 = {1337};
    TEST_ASSERT_EQUAL(0, hashtable_remove(hash_table, &k1));
    int res = hashtable_add(hash_table, &k1, &v1);
    TEST_ASSERT_EQUAL(1, res);
    TEST_ASSERT_EQUAL(&v1, hashtable_get(hash_table, &k1));
    TEST_ASSERT_EQUAL(1, hashtable_remove(hash_table, &k1));
    TEST_ASSERT_EQUAL(0, hashtable_remove(hash_table, &k1));
    TEST_ASSERT_EQUAL(NULL, hashtable_get(hash_table, &k1));
    TEST_ASSERT_EQUAL(1, hashtable_destroy(hash_table));
}

void testHashTableAddAndRemove200Items()
{
    const int   num_items = 200;
    test_key_t* keys[num_items];
    wrap_malloc_StubWithCallback(allocator_stub);
    wrap_free_StubWithCallback(deallocator_stub);
    hashtable_t* hash_table =
        hashtable_init(5, hash_func, key_comparator, wrap_malloc, wrap_free);
    TEST_ASSERT_NOT_NULL(hash_table);
    int i;
    for (i = 0; i < num_items; i++)
    {
        test_value_t* v1 = malloc(sizeof(test_value_t));
        v1->a            = 1 * i;
        v1->b            = 2 * i;
        v1->c            = 3.0 * i;
        test_key_t* k1   = malloc(sizeof(test_key_t));
        keys[i]          = k1;
        k1->num          = i;
        int res          = hashtable_add(hash_table, k1, v1);
        TEST_ASSERT_EQUAL(1, res);
    }
    for (i = 0; i < num_items; i++)
    {
        test_value_t* v = hashtable_get(hash_table, keys[i]);
        TEST_ASSERT_NOT_NULL(v);
        int res = hashtable_remove(hash_table, keys[i]);
        free(v);
        TEST_ASSERT_NULL(hashtable_get(hash_table, keys[i]));
        free(keys[i]);
    }
    TEST_ASSERT_EQUAL(1, hashtable_destroy(hash_table));
}

void testHashTableAddAndGet200Items()
{
    const int   num_items = 200;
    test_key_t* keys[num_items];
    wrap_malloc_StubWithCallback(allocator_stub);
    wrap_free_StubWithCallback(deallocator_stub);
    hashtable_t* hash_table =
        hashtable_init(5, hash_func, key_comparator, wrap_malloc, wrap_free);
    TEST_ASSERT_NOT_NULL(hash_table);
    int i;
    for (i = 0; i < num_items; i++)
    {
        test_value_t* v1 = malloc(sizeof(test_value_t));
        v1->a            = 1 * i;
        v1->b            = 2 * i;
        v1->c            = 3.0 * i;
        test_key_t* k1   = malloc(sizeof(test_key_t));
        keys[i]          = k1;
        k1->num          = i;
        int res          = hashtable_add(hash_table, k1, v1);
        TEST_ASSERT_EQUAL(1, res);
    }
    for (i = 0; i < num_items; i++)
    {
        test_value_t* v = hashtable_get(hash_table, keys[i]);
        TEST_ASSERT_NOT_NULL(v);
        TEST_ASSERT_EQUAL(1 * i, v->a);
        TEST_ASSERT_EQUAL(2 * i, v->b);
        TEST_ASSERT_EQUAL(3.0 * i, v->c);
        free(v);
        free(keys[i]);
    }
    TEST_ASSERT_EQUAL(1, hashtable_destroy(hash_table));
}
