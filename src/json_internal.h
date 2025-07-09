#ifndef LIBJSON_JSON_INTERNAL_H
#define LIBJSON_JSON_INTERNAL_H

#include "libjson/json.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>

/**
 * @brief Function pointer type for freeing values
 */
typedef void (*free_func)(void *);

/**
 * @brief Function type for closures with function argument and context.
 */
typedef void *(*closure_func)(void *arg, void *ctx);

/**
 * @brief Function type for closures with function argument only.
 */
typedef void *(*pure_func)(void *arg);

/**
 * @brief Function type for closures with only args and no return value.
 */
typedef void (*call_func)(void *arg);

// Forward declarations for internal structures
struct closure;
struct linked_list;
struct linked_list_json;
struct hash_table;
struct hash_table_entry;
struct hash_table_iter;
struct linked_list_iter;
struct linked_list_json_iter;

// ===== CLOSURE API =====
struct closure *closure_pure(pure_func func);
struct closure *closure_call(call_func func);
struct closure *closure_new(closure_func func, void *ctx);
void closure_free(struct closure *closure);
void *closure_invoke(struct closure *closure, void *arg);

// ===== LINKED LIST API =====
struct linked_list *linked_list_new(void *value);
int linked_list_length(struct linked_list *list);
void linked_list_free(struct linked_list *list, free_func free_value);
void linked_list_foreach(struct linked_list *list, struct closure *closure);
struct linked_list *linked_list_insert(struct linked_list *last, void *value);
void *linked_list_value(struct linked_list *node);
struct linked_list *linked_list_next(struct linked_list *node);
struct linked_list *linked_list_find(struct linked_list *node, struct closure *closure, struct linked_list **prev);
void *linked_list_remove(struct linked_list *node, struct closure *closure, struct linked_list **prev);

// Linked list iterator API
struct linked_list_iter *linked_list_iter_new(struct linked_list *list);
void *linked_list_iter_next(struct linked_list_iter *iter);
int linked_list_iter_has_next(struct linked_list_iter *iter);
void linked_list_iter_free(struct linked_list_iter *iter);

// ===== JSON-SPECIFIC LINKED LIST API =====
struct linked_list_json *linked_list_json_new(struct json *value);
int linked_list_json_length(struct linked_list_json *list);
void linked_list_json_free(struct linked_list_json *list);
struct linked_list_json *linked_list_json_insert(struct linked_list_json *last, struct json *value);
struct json *linked_list_json_value(struct linked_list_json *node);
struct linked_list_json *linked_list_json_next(struct linked_list_json *node);
struct json *linked_list_json_get(struct linked_list_json *list, int index);

// JSON linked list iterator API
struct linked_list_json_iter *linked_list_json_iter_new(struct linked_list_json *list);
void linked_list_json_iter_free(struct linked_list_json_iter *iter);
struct json *linked_list_json_iter_next(struct linked_list_json_iter *iter);
int linked_list_json_iter_has_next(struct linked_list_json_iter *iter);

// ===== HASH TABLE API =====
const char *hash_table_entry_key(const struct hash_table_entry *entry);
void *hash_table_entry_value(const struct hash_table_entry *entry);
struct hash_table *hash_table_new();
void hash_table_free(struct hash_table *table, free_func free_value);
void hash_table_set(struct hash_table *table, const char *key, void *value);
void *hash_table_get(const struct hash_table *table, const char *key);
void *hash_table_remove(struct hash_table *table, const char *key);
int hash_table_has(const struct hash_table *table, const char *key);
int hash_table_keys(const struct hash_table *table, char **keys);
void hash_table_foreach(const struct hash_table *table, struct closure *closure);

// Hash table iterator API
struct hash_table_iter *hash_table_iter_new(const struct hash_table *table);
void hash_table_iter_free(struct hash_table_iter *iter);
struct hash_table_entry *hash_table_iter_next(struct hash_table_iter *iter);
int hash_table_iter_has_next(struct hash_table_iter *iter);

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
        struct linked_list_json *array;
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
