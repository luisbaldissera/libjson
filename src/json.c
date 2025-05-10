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
static struct json json_null_value = {.type = JSON_NULL, .value = {0}};
static struct json json_true_value = {.type = JSON_BOOLEAN, .value = {.boolean = 1}};
static struct json json_false_value = {.type = JSON_BOOLEAN, .value = {.boolean = 0}};

// Forward declarations of helper functions
static int json_write_escaped_string(const char *str, FILE *out);

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
    struct closure *free_closure = closure_call((call_func)json_free);
    if (free_closure)
    {
      linked_list_foreach(json->value.array, free_closure);
      closure_free(free_closure);
    }
    break;
  }
  case JSON_OBJECT:
  {
    struct closure *free_closure = closure_call((call_func)json_free);
    if (free_closure)
    {
      hash_table_foreach(json->value.object, free_closure);
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

int json_array_length(struct json *array)
{
  if (!array || !json_isarray(array))
    return 0;

  return linked_list_length(array->value.array);
}

struct json *json_array_get(const struct json *array, int index)
{
  if (!array || !json_isarray((struct json *)array) || index < 0)
    return NULL;

  struct linked_list *node = array->value.array;
  for (int i = 0; i < index && node; i++)
  {
    node = linked_list_next(node);
  }
  return (struct json *)linked_list_value(node);
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
 * JSON Serialization Functions
 ************************************/

/**
 * Helper function to write escaped strings according to JSON specification
 * Handles escaping of control characters, double quotes, and backslashes
 */
static int json_write_escaped_string(const char *str, FILE *out)
{
  if (!str || !out)
    return -1;

  int bytes_written = 0;
  int ret;

  ret = fputc('"', out);
  if (ret == EOF)
    return -1;
  bytes_written++;

  for (const unsigned char *ptr = (const unsigned char *)str; *ptr; ptr++)
  {
    switch (*ptr)
    {
    case '"': // Double quote
      ret = fputs("\\\"", out);
      break;
    case '\\': // Backslash
      ret = fputs("\\\\", out);
      break;
    case '\b': // Backspace
      ret = fputs("\\b", out);
      break;
    case '\f': // Form feed
      ret = fputs("\\f", out);
      break;
    case '\n': // Newline
      ret = fputs("\\n", out);
      break;
    case '\r': // Carriage return
      ret = fputs("\\r", out);
      break;
    case '\t': // Tab
      ret = fputs("\\t", out);
      break;
    default:
      // Control characters (0-31) need special handling
      if (*ptr < 32)
      {
        // Use the \u escape sequence for control characters
        char escape_seq[8];
        sprintf(escape_seq, "\\u%04x", *ptr);
        ret = fputs(escape_seq, out);
      }
      else
      {
        // Normal character, output directly
        ret = fputc(*ptr, out);
        if (ret != EOF)
          ret = 1; // fputc returns the character written, not bytes written
      }
      break;
    }

    if (ret < 0)
      return -1;
    bytes_written += ret;
  }

  ret = fputc('"', out);
  if (ret == EOF)
    return -1;
  bytes_written++;

  return bytes_written;
}

int json_fwrite(struct json *node, FILE *out)
{
  if (!node || !out)
    return -1;

  switch (node->type)
  {
  case JSON_NULL:
  {
    return fprintf(out, "null");
  }
  case JSON_BOOLEAN:
  {
    return fprintf(out, node->value.boolean ? "true" : "false");
  }
  case JSON_NUMBER:
  {
    // Check if the number is an integer to avoid printing decimals unnecessarily
    double intpart;
    if (modf(node->value.number, &intpart) == 0.0)
    {
      return fprintf(out, "%.0f", node->value.number);
    }
    else
    {
      return fprintf(out, "%g", node->value.number);
    }
  }
  case JSON_STRING:
  {
    // Use our helper function to properly escape JSON strings
    return json_write_escaped_string(node->value.string ? node->value.string : "", out);
  }
  case JSON_ARRAY:
  {
    int ret, bytes_written = 0;
    struct linked_list_iter *iter = linked_list_iter_new(node->value.array);
    if (!iter)
    {
      return -1;
    }
    ret = fprintf(out, "[");
    if (ret < 0)
      return ret;
    bytes_written += ret;
    struct json *element;
    while (element = linked_list_iter_next(iter))
    {
      int ret = json_fwrite(element, out);
      if (ret < 0)
      {
        linked_list_iter_free(iter);
        return ret;
      }
      bytes_written += ret;
      if (linked_list_iter_has_next(iter))
      {
        ret = fprintf(out, ",");
        if (ret < 0)
        {
          linked_list_iter_free(iter);
          return ret;
        }
        bytes_written += ret;
      }
    }
    linked_list_iter_free(iter);
    bytes_written += fprintf(out, "]");
    return bytes_written;
  }
  case JSON_OBJECT:
  {
    int ret, bytes_written = 0;
    struct hash_table_iter *iter = hash_table_iter_new(node->value.object);
    if (!iter)
    {
      return -1;
    }
    ret = fprintf(out, "{");
    if (ret < 0)
      return ret;
    bytes_written += ret;
    struct hash_table_entry *entry;
    while (entry = hash_table_iter_next(iter))
    {
      // Properly escape the object key
      ret = json_write_escaped_string(hash_table_entry_key(entry), out);
      if (ret < 0)
      {
        hash_table_iter_free(iter);
        return ret;
      }
      bytes_written += ret;

      ret = fprintf(out, ":");
      if (ret < 0)
      {
        hash_table_iter_free(iter);
        return ret;
      }
      bytes_written += ret;

      ret = json_fwrite(hash_table_entry_value(entry), out);
      if (ret < 0)
      {
        hash_table_iter_free(iter);
        return ret;
      }
      bytes_written += ret;

      if (hash_table_iter_has_next(iter))
      {
        ret = fprintf(out, ",");
        if (ret < 0)
        {
          hash_table_iter_free(iter);
          return ret;
        }
        bytes_written += ret;
      }
    }
    hash_table_iter_free(iter);
    bytes_written += fprintf(out, "}");
    return bytes_written;
  }
  default:
    break;
  }
}
