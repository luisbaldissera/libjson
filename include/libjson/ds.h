#ifndef LIBJSON_DS_H
#define LIBJSON_DS_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @file ds.h
     * @brief Data structures used by the JSON library
     *
     * This header provides linked list and hash table implementations
     * that can be used directly by library users for their own purposes.
     * These data structures are also used internally by the JSON library.
     */

    /////////////////////////////////////////////////////////
    // Closure
    /////////////////////////////////////////////////////////

    /**
     * @brief Closure structure for context-aware callbacks
     *
     * Enables passing additional context to predicates and callbacks
     */
    struct closure;

    /**
     * @brief Function type for closures with context
     */
    typedef int (*closure_func)(void *value, void *ctx);

    /**
     * @brief Creates a new closure
     * @param func The function to call
     * @param context The context to pass to the function
     * @return A new closure, or NULL on allocation failure
     */
    struct closure *closure_new(closure_func func, void *context);

    /**
     * @brief Frees a closure
     * @param closure The closure to free
     */
    void closure_free(struct closure *closure);

    /**
     * @brief Invokes the closure function with the given value and internal context
     * @param closure The closure to invoke
     * @param value The value to pass to the closure function
     * @return The result of the closure function
     */
    int closure_invoke(struct closure *closure, void *value);

    ////////////////////////////////////////////////////////
    // Linked List
    ////////////////////////////////////////////////////////

    /**
     * @brief Linked list node structure
     *
     * A simple linked list implementation that can store any data type
     * using void pointers.
     */
    struct linked_list;

    /**
     * @brief Creates a new linked list node
     * @param value Pointer to the value to store in the node
     * @return Pointer to the new node, or NULL on allocation failure
     */
    struct linked_list *linked_list_new(void *value);

    /**
     * @brief Frees a linked list and optionally its values
     * @param list The head of the linked list to free
     */
    void linked_list_free(struct linked_list *list);

    /**
     * @brief Applies a function to each value in the linked list
     * @param list The head of the linked list
     * @param func Function to call on each value
     */
    void linked_list_foreach(struct linked_list *list, void (*func)(void *));

    /**
     * @brief Inserts a new node after the specified node
     * @param last Node after which to insert the new node
     * @param value Pointer to the value to store in the new node
     * @return Pointer to the newly inserted node, or NULL on allocation failure
     */
    struct linked_list *linked_list_insert(struct linked_list *last, void *value);

    /**
     * @brief Gets the value stored in a linked list node
     * @param node The node to get the value from
     * @return Pointer to the stored value
     */
    void *linked_list_value(struct linked_list *node);

    /**
     * @brief Gets the next node in the linked list
     * @param node The current node
     * @return Pointer to the next node, or NULL if there is no next node
     */
    struct linked_list *linked_list_next(struct linked_list *node);

    /**
     * @brief Finds a node in the linked list that matches a given condition
     * @param node The head of the linked list to search
     * @param closure The func-closure to use for matching
     * @param prev If provided, will be set to the previous node of the found node, otherwise use NULL.
     * @return Pointer to the found node, or NULL if not found
     */
    struct linked_list *linked_list_find(struct linked_list *node, struct closure *closure, struct linked_list **prev);

    ////////////////////////////////////////////////////////
    // Hash Table
    ////////////////////////////////////////////////////////

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
     * @param free_values Non-zero to also free the stored values, 0 to free only the table structure
     */
    void hash_table_free(struct hash_table *table, int free_values);

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
    void *HT_get(const struct hash_table *table, const char *key);

    /**
     * @brief Gets all keys in the hash table
     * @param table The hash table to query
     * @param keys Array to store the keys (must be pre-allocated). If NULL is passed,
     *            the function will return the number of keys without storing them.
     * @return Number of keys stored
     */
    int HT_keys(const struct hash_table *table, char **keys);

#ifdef __cplusplus
}
#endif

#endif // LIBJSON_DS_H