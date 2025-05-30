/**
 * @file linked_list.h
 * @brief Linked list implementation
 */

#ifndef LIBJSON_LINKED_LIST_H
#define LIBJSON_LINKED_LIST_H

#include "libjson/closure.h"
#include "libjson/types.h"

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
 * @brief Gets the length of the linked list
 * @param list The head of the linked list
 * @return The number of nodes in the linked list
 */
int linked_list_length(struct linked_list *list);

/**
 * @brief Frees a linked list and optionally its values
 * @param list The head of the linked list to free
 * @param free_value Function to free the value stored in each node, or NULL if not needed
 */
void linked_list_free(struct linked_list *list, free_func free_value);

/**
 * @brief Applies a function to each value in the linked list
 * @param list The head of the linked list
 * @param closure The func-closure to apply to each value
 */
void linked_list_foreach(struct linked_list *list, struct closure *closure);

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

/**
 * @brief Removes a node from the linked list
 * @param node The first node of the linked list to search
 * @param closure The func-closure to use for matching
 * @param prev If provided, will be set to the previous node of the removed node, otherwise use NULL.
 * @return Pointer to the value stored in the removed node, or NULL if not found
 * @note The linked list node itself is freed.
 */
void *linked_list_remove(struct linked_list *node, struct closure *closure, struct linked_list **prev);

/**
 * @brief Opaque structure representing a linked list iterator
 */
struct linked_list_iter;

/**
 * @brief Creates a new iterator for the linked list
 * @param list The head of the linked list to iterate over
 * @return Pointer to the new iterator, or NULL on allocation failure
 * @note The iterator must be freed using linked_list_iter_free() when done
 */
struct linked_list_iter *linked_list_iter_new(struct linked_list *list);

/**
 * @brief Gets the next value from the iterator
 * @param iter The iterator to get the next value from
 * @return Pointer to the next value, or NULL if there are no more values
 */
void *linked_list_iter_next(struct linked_list_iter *iter);

/**
 * @brief Checks if there are more values in the iterator
 * @param iter The iterator to check
 * @return Non-zero if there are more values, 0 otherwise
 */
int linked_list_iter_has_next(struct linked_list_iter *iter);

/**
 * @brief Frees the iterator
 * @param iter The iterator to free
 * @note The iterator does not free the values it points to
 */
void linked_list_iter_free(struct linked_list_iter *iter);

#endif // LIBJSON_LINKED_LIST_H
