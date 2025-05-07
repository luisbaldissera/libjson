#ifndef LIBJSON_DS_H
#define LIBJSON_DS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Hash table for storing key-value pairs.
 */
struct HT;

/**
 * Creates a new hash table.
 */
struct HT *HT_new();
/**
 * Hash function for the hash table.
 */
int HT_hash(const char *key);
/**
 * Initialize the hash table.
 */
void HT_init(struct HT *hash_table);
/**
 * Set a key in the hash table with the given data. Returns 1 if the key is new and 0 if it is updated.
 */
int HT_set(struct HT *hash_table, const char *key, void *data);
/**
 * Get all the keys from the hash table and store them in the given array.
 * Returns the number of keys.
 */
int HT_keys(struct HT *hash_table, char **keys);
/**
 * Get the data from the hash table with the given key.
 */
void *HT_get(struct HT *hash_table, const char *key);
/**
 * Free the hash table indexes. Note that the data is not freed. if self is
 * non-zero, the hash table itself is freed.
 */
void HT_free(struct HT *hash_table, int self);
/**
 * Iterate through the hash table and call the given function for each element.
 */
int HT_foreach(struct HT *hash_table, void (*func)(char *, void *));

/**
 * Linked list
 */
struct LL;
/**
 * Filter function for linked list find. Its implementation should return
 * non-zero value if the element should be matched.
 */
typedef int (*LL_filtfunc)(void *);
/**
 * Create a new linked list with the given data.
 */
struct LL *LL_new(void *data);
/**
 * Insert a new element after the linked list and returns the new created
 * element.
 */
struct LL *LL_insert(struct LL *linked_list, void *data);
/**
 * Find an element in the linked list that matches the filter function. If prev
 * is not NULL, it will store the previous element from the found element in
 * it. If not found, it will return NULL and prev will be the last element. If
 * the found element is the first element, prev will be NULL.
 */
struct LL *LL_find(struct LL *linked_list, LL_filtfunc filtfunc,
                   struct LL **prev);
/**
 * Free the linked list. Note that the data is not freed.
 */
void LL_free(struct LL *linked_list);
/**
 * Write the data from the linked list as an array to the given array. Returns
 * the number of elements written.
 */
int LL_array(struct LL *linked_list, void **array);
/**
 * Iterate through the linked list and call the given function for each
 * element and returns the total number of elements iterated.
 */
int LL_foreach(struct LL *ll, void (*func)(void *));
/** 
 * Get the next element in the linked list.
 */
struct LL * LL_next(struct LL *ll);
/**
 * Get the value of the linked list element.
 */
void *LL_value(struct LL *ll);

#ifdef __cplusplus
}
#endif

#endif // LIBJSON_DS_H