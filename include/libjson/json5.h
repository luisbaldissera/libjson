#ifndef LIBJSON_JSON5_H
#define LIBJSON_JSON5_H

#include "json.h"
#include <stdio.h>

/**
 * @file json5.h
 * @brief JSON5 parsing support for the JSON manipulation library
 *
 * This header provides functions to parse JSON5 format into the standard
 * JSON structures used by the library. JSON5 is a superset of JSON that
 * adds support for comments, trailing commas, unquoted keys, and other
 * conveniences.
 */

/**
 * @brief Reads a JSON5 value from a file stream
 * @param in File stream to read from
 * @param errbuf Buffer to store error messages (optional). Use in
 * multi-threaded applications to avoid storing error messages in a static
 * buffer.
 * @return The parsed JSON value, or NULL on parsing error
 * @see json5_read_string()
 */
struct json *json5_read(FILE *in, char *errbuf);

/**
 * @brief Reads a JSON5 value from a string
 * @param str The JSON5 string to parse
 * @param errbuf Buffer to store error messages (optional). Use in
 * multi-threaded applications to avoid storing error messages in a static
 * buffer.
 * @return The parsed JSON value, or NULL on parsing error
 * @see json5_read()
 */
struct json *json5_read_string(const char *str, char *errbuf);

#endif // LIBJSON_JSON5_H