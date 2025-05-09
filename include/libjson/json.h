#ifndef LIBJSON_JSON_H
#define LIBJSON_JSON_H

#include <stdio.h>

/**
 * @file json.h
 * @brief Public API for the JSON manipulation library
 *
 * This header provides functions to create, manipulate, and serialize JSON data.
 * The library uses an opaque pointer approach to hide implementation details
 * from users while providing a clean, simple API.
 */

/**
 * @brief Opaque pointer to JSON structure
 *
 * This type represents any JSON value (null, boolean, number, string, array, or object)
 * and hides implementation details from the user.
 */
typedef struct json;

////////////////////////////////////
// JSON Creation functions
////////////////////////////////////

/**
 * @brief Creates a JSON null value
 * @return A new JSON null value
 */
struct json *json_null();

/**
 * @brief Creates a JSON true boolean value
 * @return A new JSON true value
 */
struct json *json_true();

/**
 * @brief Creates a JSON false boolean value
 * @return A new JSON false value
 */
struct json *JSON_false();

/**
 * @brief Creates a JSON decimal number value
 * @param value The double value to store
 * @return A new JSON number value
 */
struct json *json_decimal(double value);

/**
 * @brief Creates a JSON integer value
 * @param value The int value to store
 * @return A new JSON integer value
 */
struct json *json_integer(int value);

/**
 * @brief Creates a JSON string value
 * @param value The string value to store (will be copied)
 * @return A new JSON string value
 */
struct json *json_string(const char *value);

/**
 * @brief Creates an empty JSON array
 * @return A new empty JSON array
 */
struct json *json_array();

/**
 * @brief Creates an empty JSON object
 * @return A new empty JSON object
 */
struct json *json_object();

/**
 * @brief Frees a JSON value and all its children
 * @param json JSON value to free
 */
void json_free(struct json *json);

////////////////////////////////////
// JSON Type testing functions
////////////////////////////////////

/**
 * @brief Tests if a JSON value is null
 * @note Since null is static reference, this function is equivalent to comparing node == json_null()
 * @param node JSON value to test
 * @return Non-zero if node is null, 0 otherwise
 */
int json_isnull(struct json *node);

/**
 * @brief Tests if a JSON value is a boolean
 * @param node JSON value to test
 * @return Non-zero if node is a boolean, 0 otherwise
 */
int json_isboolean(struct json *node);

/**
 * @brief Tests if a JSON value is a number
 * @param node JSON value to test
 * @return Non-zero if node is a number, 0 otherwise
 */
int json_isnumber(struct json *node);

/**
 * @brief Tests if a JSON value is a string
 * @param node JSON value to test
 * @return Non-zero if node is a string, 0 otherwise
 */
int json_isstring(struct json *node);

/**
 * @brief Tests if a JSON value is an array
 * @param node JSON value to test
 * @return Non-zero if node is an array, 0 otherwise
 */
int json_isarray(struct json *node);

//////////////////////////////////////
// JSON Access functions
//////////////////////////////////////

/**
 * @brief Gets the value of a JSON number
 * @param node JSON value to query
 * @return The double value stored in the JSON number
 */
double json_todouble(const struct json *node);

/**
 * @brief Gets the value of a JSON integer
 * @param node JSON value to query
 * @return The int value stored in the JSON integer
 */
int json_toint(const struct json *node);

/**
 * @brief Gets the value of a JSON string
 * @note The returned string is a copy of the original string stored in the JSON value.
 *      The caller is responsible for freeing the returned string.
 * @param node JSON value to query
 * @return The string value stored in the JSON string
 */
const char *json_tostring(const struct json *node);

/**
 * @brief Gets the first element of a JSON array
 * @param array JSON array to query
 * @return The first JSON value in the array, or NULL if the array is empty
 */

////////////////////////////////////
// JSON Manipulation functions
////////////////////////////////////

/**
 * @brief Tests if a JSON value is an object
 * @param node JSON value to test
 * @return Non-zero if node is an object, 0 otherwise
 */
int json_isobject(struct json *node);

/**
 * @brief Sets a key-value pair in a JSON object
 * @param object JSON object to modify
 * @param key Key string (will be copied)
 * @param value JSON value to associate with the key
 */
void json_object_set(struct json *object, const char *key, struct json *value);

/**
 * @brief Gets a value by key from a JSON object
 * @param object JSON object to query
 * @param key Key string to look up
 * @return The associated JSON value, or NULL if key not found
 */
struct json *json_object_get(const struct json *object, const char *key);

/**
 * @brief Removes a key-value pair from a JSON object
 * @param object JSON object to modify
 * @param key Key string to remove
 * @return The removed JSON value, or NULL if key not found. Caller should free the returned value if necessary.
 */
struct json *json_object_remove(struct json *object, const char *key);

/**
 * @brief Adds a value to the end of a JSON array
 * @param array JSON array to modify
 * @param value JSON value to add to the array
 */
void json_array_push(struct json *array, struct json *value);

////////////////////////////////////
// JSON Serialization functions
////////////////////////////////////

/**
 * @brief Writes a JSON value to a file stream
 * @param node JSON value to write
 * @param out File stream to write to
 * @return Number of bytes written, or negative value on error
 */
int json_fwrite(struct json *node, FILE *out);

#endif // LIBJSON_JSON_H