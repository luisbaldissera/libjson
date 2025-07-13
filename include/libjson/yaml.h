#ifndef LIBJSON_YAML_H
#define LIBJSON_YAML_H

#include <stdio.h>
#include "json.h"

/**
 * @file yaml.h
 * @brief YAML support for the JSON manipulation library
 *
 * This header provides functions to read YAML data into JSON structures
 * and write JSON structures as YAML. Uses the same struct json* types
 * from json.h for full compatibility.
 */

/**
 * @brief Reads a YAML value from a file stream
 * @param in File stream to read from
 * @param errbuf Buffer to store error messages (optional). Use in
 * multi-threaded applications to avoid storing error messages in a static
 * buffer.
 * @return The parsed JSON value representing the YAML data, or NULL on parsing error
 * @see yaml_read_string()
 */
struct json *yaml_read(FILE *in, char *errbuf);

/**
 * @brief Reads a YAML value from a string
 * @param str The YAML string to parse
 * @param errbuf Buffer to store error messages (optional). Use in
 * multi-threaded applications to avoid storing error messages in a static
 * buffer.
 * @return The parsed JSON value representing the YAML data, or NULL on parsing error
 * @see yaml_read()
 */
struct json *yaml_read_string(const char *str, char *errbuf);

/**
 * @brief Writes a JSON value as YAML to a file stream
 * @param element JSON value to write as YAML
 * @param out File stream to write to
 * @return Number of bytes written, or negative value on error
 */
int yaml_write(struct json *element, FILE *out);

#endif // LIBJSON_YAML_H