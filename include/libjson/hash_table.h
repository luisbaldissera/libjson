/**
 * @file hash_table.h
 * @brief Hash table implementation
 */

#ifndef LIBJSON_HASH_TABLE_H
#define LIBJSON_HASH_TABLE_H

#include "libjson/closure.h"
#include "libjson/types.h"

/**
 * @brief Hash table entry structure
 */
struct hash_table_entry;

/**
 * @brief key of the hash table entry
 * @param entry The hash table entry
 * @return The key string of the entry
 */
const char *hash_table_entry_key(const struct hash_table_entry *entry);

/**
 * @brief value of the hash table entry
 * @param entry The hash table entry
 * @return The value pointer of the entry
 */
void *hash_table_entry_value(const struct hash_table_entry *entry);

/**
 * @brief Hash table structure
 *
 * A hash table implementation that maps string keys to void pointer values.
 * Implementation details are hidden in the source files.
 */
struct hash_table;

/**
 * @brief Creates a new hash table
 * @return Pointer to the new hash table, or NULL on allocation failure
 */
struct hash_table *hash_table_new();

/**
 * @brief Frees a hash table and optionally its values
 * @param table The hash table to free
 * @param free_value Function to free the value stored in each entry, or NULL if not needed
 */
void hash_table_free(struct hash_table *table, free_func free_value);

/**
 * @brief Sets a key-value pair in the hash table
 * @param table The hash table to modify
 * @param key The key string (will be copied)
 * @param value Pointer to the value to store
 */
void hash_table_set(struct hash_table *table, const char *key, void *value);

/**
 * @brief Gets the value associated with a key
 * @param table The hash table to query
 * @param key The key to look up
 * @return Pointer to the stored value, or NULL if key not found
 */
void *hash_table_get(const struct hash_table *table, const char *key);

/**
 * @brief Gets all keys in the hash table
 * @param table The hash table to query
 * @param keys Array to store the keys (must be pre-allocated). If NULL is passed,
 *            the function will return the number of keys without storing them.
 * @return Number of keys stored
 */
int hash_table_keys(const struct hash_table *table, char **keys);

/**
 * @brief Apply closure to each entry in the hash table
 * @param table The hash table to iterate
 * @param closure The closure to apply to each entry
 */
void hash_table_foreach(const struct hash_table *table, struct closure *closure);

struct hash_table_iter;

/**
 * @brief Creates a new iterator for the hash table
 * @param table The hash table to iterate
 * @return Pointer to the new iterator, or NULL on allocation failure
 */
struct hash_table_iter *hash_table_iter_new(const struct hash_table *table);

/**
 * @brief Frees the hash table iterator
 * @param iter The iterator to free
 */
void hash_table_iter_free(struct hash_table_iter *iter);

/**
 * @brief Gets the next entry in the hash table iterator
 * @param iter The iterator to use
 * @return Pointer to the next hash table entry, or NULL if there are no more entries
 */
struct hash_table_entry *hash_table_iter_next(struct hash_table_iter *iter);

/**
 * @brief Checks if the iterator has more entries
 * @param iter The iterator to check
 * @return Non-zero if there are more entries, 0 otherwise
 */
int hash_table_iter_has_next(struct hash_table_iter *iter);

#endif // LIBJSON_HASH_TABLE_H