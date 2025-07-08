#ifndef LIBJSON_JSON_INTERNAL_H
#define LIBJSON_JSON_INTERNAL_H

#include "libjson/json.h"
#include "libjson/closure.h"
#include "libjson/linked_list.h"
#include "libjson/hash_table.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>

/**
 * JSON value types
 */
typedef enum json_type
{
    JSON_NULL,
    JSON_BOOLEAN,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} json_type;

/**
 * JSON structure implementation
 */
struct json
{
    json_type type;
    union
    {
        int boolean;
        double number;
        char *string;
        struct linked_list *array;
        struct hash_table *object;
    } value;
};

/**
 * JSON token types for parsing
 */
struct json_token
{
    enum
    {
        JSON_TOKEN_INVALID,
        JSON_TOKEN_EOF,
        JSON_TOKEN_NULL,
        JSON_TOKEN_TRUE,
        JSON_TOKEN_FALSE,
        JSON_TOKEN_NUMBER,
        JSON_TOKEN_STRING,
        JSON_TOKEN_COMMA,
        JSON_TOKEN_COLON,
        JSON_TOKEN_ARRAY_START,
        JSON_TOKEN_ARRAY_END,
        JSON_TOKEN_OBJECT_START,
        JSON_TOKEN_OBJECT_END
    } type;
    // only used for JSON_TOKEN_STRING and JSON_TOKEN_NUMBER
    char *value;
};

/**
 * Error handling context
 */
struct error_context
{
    char *message;
    int line;
    int column;
};

// Static JSON singleton values (externally defined)
extern struct json json_null_value;
extern struct json json_true_value;
extern struct json json_false_value;

// Default error buffer (externally defined)
extern char __default_errbuf[LIBJSON_ERRBUF_SiZE];

// Internal helper functions
void strprep(char *dst, const char *src);
int update_error_context(struct error_context *errctx, const char c, int index);

// JSON write helper functions
int json_write_escaped_string(const char *str, FILE *out);
int json_write_array(struct json *array, FILE *out);
int json_write_object(struct json *object, FILE *out);
int json_write_string(struct json *node, FILE *out);
int json_write_number(struct json *node, FILE *out);
int json_write_boolean(struct json *node, FILE *out);
int json_write_null(struct json *node, FILE *out);

// JSON read helper functions
struct json_token json_read_token(FILE *in, struct error_context *errctx);
int json_parser_json(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx);
int json_parser_literal(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx);
int json_parser_array(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx);
int json_parser_object(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx);
int json_parser_key_value(FILE *in, struct json_token *token, struct json *object, struct error_context *errctx);

#endif // LIBJSON_JSON_INTERNAL_H
