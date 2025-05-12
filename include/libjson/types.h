#ifndef LIBJSON_TYPES_H
#define LIBJSON_TYPES_H

/**
 * @file types.h
 * @brief Common types and definitions for the JSON library
 */

/**
 * @brief Function pointer type for freeing values
 *
 * This type is used for function pointers that free dynamically allocated
 * memory. It takes a single void pointer argument and returns nothing.
 */
typedef void (*free_func)(void *);

#endif // LIBJSON_TYPES_H