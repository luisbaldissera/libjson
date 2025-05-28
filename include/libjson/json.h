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
 * @brief Opaque JSON structure
 *
 * This type represents any JSON value (null, boolean, number, string, array, or object)
 * and hides implementation details from the user.
 */
struct json;

struct json_key_value
{
    char *key;          /**< Key string (for objects) */
    struct json *value; /**< JSON value associated with the key */
};

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
struct json *json_false();

/**
 * @brief Creates a JSON number value
 * @param value The numeric value to store
 * @return A new JSON number value
 */
struct json *json_number(double value);

/**
 * @brief Creates a JSON string value
 * @param value The string value to store (will be copied)
 * @return A new JSON string value
 */
struct json *json_string(const char *value);

struct json *__json_array_macro(struct json *elements[]);
#define json_array(...) __json_array_macro((struct json *[]){__VA_ARGS__ __VA_OPT__(, ) NULL})

struct json *__json_object_macro(struct json_key_value elements[]);
#define json_object(...) __json_object_macro((struct json_key_value[]){__VA_ARGS__ __VA_OPT__(, ){NULL, NULL}})

/**
 * @brief Copies a JSON value
 * @param json JSON value to copy
 * @return A new JSON value that is a copy of the original
 * @note The returned value is a deep copy, meaning all nested structures are also copied.
 *       The caller is responsible for freeing the returned value using json_free().
 * @see json_free()
 */
struct json *json_copy(struct json *json);

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
int json_is_null(struct json *node);

/**
 * @brief Tests if a JSON value is a boolean
 * @param node JSON value to test
 * @return Non-zero if node is a boolean, 0 otherwise
 */
int json_is_boolean(struct json *node);

/**
 * @brief Tests if a JSON value is a number
 * @param node JSON value to test
 * @return Non-zero if node is a number, 0 otherwise
 */
int json_is_number(struct json *node);

/**
 * @brief Tests if a JSON value is a string
 * @param node JSON value to test
 * @return Non-zero if node is a string, 0 otherwise
 */
int json_is_string(struct json *node);

/**
 * @brief Tests if a JSON value is an array
 * @param node JSON value to test
 * @return Non-zero if node is an array, 0 otherwise
 */
int json_is_array(struct json *node);

/**
 * @brief Tests if a JSON value is an object
 * @param node JSON value to test
 * @return Non-zero if node is an object, 0 otherwise
 */
int json_is_object(struct json *node);

////////////////////////////////////////
// JSON Access functions
////////////////////////////////////////

/**
 * @brief Gets the length of a JSON array
 * @param array JSON array to query
 * @return The number of elements in the array
 */
int json_array_length(struct json *array);

/**
 * @brief Gets the value at a specific index in a JSON array
 * @param array JSON array to query
 * @param index Index of the element to retrieve
 * @return The JSON value at the specified index, or NULL if index is out of bounds
 */
struct json *json_array_get(const struct json *array, int index);

/**
 * @brief Gets the value of a JSON number
 * @param node JSON value to query
 * @return The double value stored in the JSON number
 */
double json_double_value(const struct json *node);

/**
 * @brief Gets the value of a JSON integer
 * @param node JSON value to query
 * @return The int value stored in the JSON integer
 */
int json_int_value(const struct json *node);

/**
 * @brief Gets the value of a JSON string
 * @note The returned string is a copy of the original string stored in the JSON value.
 *      The caller is responsible for freeing the returned string.
 * @param node JSON value to query
 * @return The string value stored in the JSON string
 */
const char *json_string_value(const struct json *node);

////////////////////////////////////
// JSON Manipulation functions
////////////////////////////////////

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
 * @brief Gets the number of key-value pairs in a JSON object
 * @param object JSON object to query
 * @return The number of key-value pairs in the object
 */
int json_object_length(struct json *object);

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
int json_write(struct json *node, FILE *out);

/**
 * @brief Reads a JSON value from a file stream
 * @param in File stream to read from
 * @return The parsed JSON value, or NULL on parsing error
 * @see json_read_string()
 */
struct json *json_read(FILE *in);

/**
 * @brief Reads a JSON value from a string
 * @param json_string The JSON string to parse
 * @return The parsed JSON value, or NULL on parsing error
 * @see json_read()
 */
struct json *json_read_string(const char *json_string);

/**
 * @brief Returns the last error message from parsing
 * @return Error message string, or NULL if no error occurred
 */
const char *json_error();

#endif // LIBJSON_JSON_H