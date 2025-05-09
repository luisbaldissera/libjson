#include "libjson/json.h"
#include "libjson/closure.h"
#include "libjson/linked_list.h"
#include "libjson/hash_table.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

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

// Static JSON singleton values
static struct json json_null_value = { .type = JSON_NULL, .value = {0}};
static struct json json_true_value = { .type = JSON_BOOLEAN, .value = { .boolean = 1}};
static struct json json_false_value = { .type = JSON_BOOLEAN, .value = { .boolean = 0}};

// Forward declarations of helper functions
static void json_write_indent(FILE *out, int indent_level);
static int json_fwrite_internal(struct json *node, FILE *out, int indent, int indent_level);
static void *json_free_closure_func(void *value, void *ctx);

/************************************
 * JSON Creation Functions
 ************************************/

struct json *json_null()
{
  return &json_null_value;
}

struct json *json_true()
{
  return &json_true_value;
}

struct json *json_false()
{
  return &json_false_value;
}

struct json *json_number(double value)
{
  struct json *node = (struct json *)malloc(sizeof(struct json));
  if (!node)
    return NULL;

  node->type = JSON_NUMBER;
  node->value.number = value;
  return node;
}

struct json *json_string(const char *value)
{
  struct json *node = (struct json *)malloc(sizeof(struct json));
  if (!node)
    return NULL;

  node->type = JSON_STRING;
  node->value.string = value ? strdup(value) : NULL;
  if (!node->value.string && value)
  {
    free(node);
    return NULL;
  }
  return node;
}

struct json *json_array()
{
  struct json *node = (struct json *)malloc(sizeof(struct json));
  if (!node)
    return NULL;

  node->type = JSON_ARRAY;
  node->value.array = NULL; // Empty array initially
  return node;
}

struct json *json_object()
{
  struct json *node = (struct json *)malloc(sizeof(struct json));
  if (!node)
    return NULL;

  node->type = JSON_OBJECT;
  node->value.object = hash_table_new();
  if (!node->value.object)
  {
    free(node);
    return NULL;
  }
  return node;
}

static void *json_free_closure_func(void *value, void *ctx)
{
  if (value)
  {
    json_free((struct json *)value);
  }
  return NULL;
}

void json_free(struct json *json)
{
  if (!json || json == &json_null_value || json == &json_true_value || json == &json_false_value)
    return;

  switch (json->type)
  {
  case JSON_STRING:
    free(json->value.string);
    break;
  case JSON_ARRAY:
  {
    struct closure *free_closure = closure_new(json_free_closure_func, NULL);
    if (free_closure)
    {
      linked_list_free(json->value.array, free_closure);
      closure_free(free_closure);
    }
    break;
  }
  case JSON_OBJECT:
  {
    struct closure *free_closure = closure_new(json_free_closure_func, NULL);
    if (free_closure)
    {
      hash_table_free(json->value.object, free_closure);
      closure_free(free_closure);
    }
    break;
  }
  default:
    break; // Nothing to free for simple types
  }

  free(json);
}

/************************************
 * JSON Type Testing Functions
 ************************************/

int json_isnull(struct json *node)
{
  return node == &json_null_value;
}

int json_isboolean(struct json *node)
{
  return node && node->type == JSON_BOOLEAN;
}

int json_isnumber(struct json *node)
{
  return node && node->type == JSON_NUMBER;
}

int json_isstring(struct json *node)
{
  return node && node->type == JSON_STRING;
}

int json_isarray(struct json *node)
{
  return node && node->type == JSON_ARRAY;
}

int json_isobject(struct json *node)
{
  return node && node->type == JSON_OBJECT;
}

/************************************
 * JSON Access Functions
 ************************************/

double json_todouble(const struct json *node)
{
  if (!node || node->type != JSON_NUMBER)
    return 0.0;
  return node->value.number;
}

int json_toint(const struct json *node)
{
  if (!node || node->type != JSON_NUMBER)
    return 0;
  return (int)node->value.number;
}

const char *json_tostring(const struct json *node)
{
  if (!node || node->type != JSON_STRING)
    return NULL;
  return node->value.string;
}

/************************************
 * JSON Array Manipulation Functions
 ************************************/

void json_array_push(struct json *array, struct json *value)
{
  if (!array || !value || !json_isarray(array))
    return;

  if (!array->value.array)
  {
    // Create the first node
    array->value.array = linked_list_new(value);
  }
  else
  {
    // Find the last node and insert after it
    struct linked_list *node = array->value.array;
    while (linked_list_next(node))
    {
      node = linked_list_next(node);
    }
    linked_list_insert(node, value);
  }
}

/************************************
 * JSON Object Manipulation Functions
 ************************************/

void json_object_set(struct json *object, const char *key, struct json *value)
{
  if (!object || !key || !value || !json_isobject(object))
    return;

  hash_table_set(object->value.object, key, value);
}

struct json *json_object_get(const struct json *object, const char *key)
{
  if (!object || !key || !json_isobject((struct json *)object))
    return NULL;

  return (struct json *)hash_table_get(object->value.object, key);
}

struct json *json_object_remove(struct json *object, const char *key)
{
  if (!object || !key || !json_isobject(object))
    return NULL;

  // We need to use hash_table_get first to get the value before removal
  struct json *value = (struct json *)hash_table_get(object->value.object, key);

  // Custom closure to remove a key but not free its value
  struct closure *remove_closure = closure_new((closure_func)NULL, NULL);
  if (remove_closure)
  {
    hash_table_set(object->value.object, key, NULL);
    closure_free(remove_closure);
  }

  return value;
}

/************************************
 * JSON Helper Functions for Serialization
 ************************************/

static void json_write_indent(FILE *out, int indent_level)
{
  for (int i = 0; i < indent_level; i++)
  {
    fprintf(out, "  ");
  }
}

// Helper struct for array serialization
struct array_write_context
{
  FILE *out;
  int indent;
  int indent_level;
  int first;
};

// Closure function for array serialization
static void *array_write_closure_func(void *value, void *ctx)
{
  struct array_write_context *context = (struct array_write_context *)ctx;
  struct json *json_value = (struct json *)value;

  if (!context->first)
  {
    fprintf(context->out, ",");
  }
  else
  {
    context->first = 0;
  }

  if (context->indent)
  {
    fprintf(context->out, "\n");
    json_write_indent(context->out, context->indent_level + 1);
  }
  else
  {
    fprintf(context->out, " ");
  }

  json_fwrite_internal(json_value, context->out, context->indent, context->indent_level + 1);
  return NULL;
}

// Helper struct for object serialization
struct object_write_context
{
  FILE *out;
  int indent;
  int indent_level;
  int first;
};

// Closure function for object serialization
static void *object_write_closure_func(void *entry, void *ctx)
{
  struct object_write_context *context = (struct object_write_context *)ctx;
  struct hash_table_entry *hash_entry = (struct hash_table_entry *)entry;
  const char *key = hash_table_entry_key(hash_entry);
  struct json *value = (struct json *)hash_table_entry_value(hash_entry);

  if (!context->first)
  {
    fprintf(context->out, ",");
  }
  else
  {
    context->first = 0;
  }

  if (context->indent)
  {
    fprintf(context->out, "\n");
    json_write_indent(context->out, context->indent_level + 1);
  }
  else
  {
    fprintf(context->out, " ");
  }

  // Write key with quotes
  fprintf(context->out, "\"%s\":%s", key, context->indent ? " " : "");

  // Write value
  json_fwrite_internal(value, context->out, context->indent, context->indent_level + 1);
  return NULL;
}

static int json_fwrite_internal(struct json *node, FILE *out, int indent, int indent_level)
{
  if (!node || !out)
    return -1;

  int bytes_written = 0;

  switch (node->type)
  {
  case JSON_NULL:
    bytes_written = fprintf(out, "null");
    break;
  case JSON_BOOLEAN:
    bytes_written = fprintf(out, node->value.boolean ? "true" : "false");
    break;
  case JSON_NUMBER:
  {
    // Check if the number is an integer to avoid printing decimals unnecessarily
    double intpart;
    if (modf(node->value.number, &intpart) == 0.0)
    {
      bytes_written = fprintf(out, "%.0f", node->value.number);
    }
    else
    {
      bytes_written = fprintf(out, "%g", node->value.number);
    }
    break;
  }
  case JSON_STRING:
    // Simple string serialization (a more complete implementation would handle escaping)
    bytes_written = fprintf(out, "\"%s\"", node->value.string ? node->value.string : "");
    break;
  case JSON_ARRAY:
  {
    bytes_written = fprintf(out, "[");

    if (node->value.array)
    {
      struct array_write_context context = {
          .out = out,
          .indent = indent,
          .indent_level = indent_level,
          .first = 1};

      struct closure *write_closure = closure_new(array_write_closure_func, &context);
      if (write_closure)
      {
        linked_list_foreach(node->value.array, write_closure);
        closure_free(write_closure);
      }
    }

    if (indent && node->value.array)
    {
      fprintf(out, "\n");
      json_write_indent(out, indent_level);
    }
    else if (node->value.array)
    {
      fprintf(out, " ");
    }

    bytes_written += fprintf(out, "]");
    break;
  }
  case JSON_OBJECT:
  {
    bytes_written = fprintf(out, "{");

    if (node->value.object)
    {
      struct object_write_context context = {
          .out = out,
          .indent = indent,
          .indent_level = indent_level,
          .first = 1};

      struct closure *write_closure = closure_new(object_write_closure_func, &context);
      if (write_closure)
      {
        hash_table_foreach(node->value.object, write_closure);
        closure_free(write_closure);
      }
    }

    if (indent && node->value.object)
    {
      fprintf(out, "\n");
      json_write_indent(out, indent_level);
    }
    else if (node->value.object)
    {
      fprintf(out, " ");
    }

    bytes_written += fprintf(out, "}");
    break;
  }
  }

  return bytes_written;
}

/************************************
 * JSON Serialization Functions
 ************************************/

int json_fwrite(struct json *node, FILE *out)
{
  if (!node || !out)
    return -1;

  // Use pretty printing with indentation by default
  return json_fwrite_internal(node, out, 1, 0);
}
