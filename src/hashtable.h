#ifndef HASHTABLE_H_
#define HASHTABLE_H_
#include "list.h"

typedef struct _kvp_t
{
    void *key;
    void *value;
} kvp_t;

/**
 * @brief hashtables maintain bookkeeping information about the hash functions,
 * key comparators and deallocators the user supplied to manage user supplied
 * keys and values.
 */
typedef struct _hashtable_t
{
    unsigned int (*hash_function)(void *);   //!< function used to hash keys.
    int (*key_comparator)(void *, void *);   //!< function used to compare keys.
    void *(*allocator)(size_t);    //!< function used to make allocations
    void (*deallocator)(void *);   //!< function used to deallocate memory
    list_t **    lists;            //!< an array of lists that implement the
    unsigned int num_slots;        //!< the number of slots for this hashtable.
} hashtable_t;

/**
 * @brief creates a hashtable based on user supplied values to hash values and
 * compare keys.
 * @param num_slots: the number of initial lists to use for storing values. Must
 * be greater than 0, or NULL will be returned
 * @param hash_function: the function to use to hash keys supplied on 'put' and
 * 'get' ops
 * @param key_comparator: the function to call to compare keys for equality
 * @param allocator: the user supplied function to make allocaitons
 * @param deallocator: the user supplied function to deallocate
 * @return hashtable_t*: hashtable object to used for rest of API
 * @return NULL: no memory available
 */
hashtable_t *hashtable_init(unsigned int num_slots,
                            unsigned int (*hash_function)(void *),
                            int (*key_comparator)(void *, void *),
                            void *(*allocator)(size_t),
                            void (*deallocator)(void *));
/**
 * @brief adds a key/value pair to the hashtable, if not present
 * @param hashtable: the hashtable to which the value should be added
 * @param key: the key for this data element.
 * @param value: the value (the data itself).
 * @return 1: success; value for key added
           2: no change, key was already present
 *         0: failure
 */
int hashtable_add(hashtable_t *hashtable, void *key, void *value);

/**
 * @brief gets a reference to the data element in the hashtable.
 * @param hashtable: the hashtable containing the data element.
 * @param key: the key indentifying the element to get.
 * @return void* value: the data, if present in the table, or NULL otherwise.
 */
void *hashtable_get(hashtable_t *hashtable, void *key);

/**
 * @brief removes an entry from the hashtable, if present.
 * @param hashtable: the hashtable from which the element should be removed.
 * @param key: the key identifying the element to remove.
 * @return 1: if the element was successfully removed.
 *         0: if the element was not found.
 */
int hashtable_remove(hashtable_t *hashtable, void *key);

/**
 * @brief destroys the hashtable, freeing all resources it allocated including
 * kv_pairs, and invokes the user supplied deallocators for key and values in
 * the table.
 * @param hashtable: the hashtable to destroy.
 * @return 1: on success
           0: failure
 */
int hashtable_destroy(hashtable_t *hashtable);

#endif   // HASHTABLE_H
