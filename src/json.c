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

// Static JSON singleton values
static struct json json_null_value = {.type = JSON_NULL, .value = {0}};
static struct json json_true_value = {.type = JSON_BOOLEAN, .value = {.boolean = 1}};
static struct json json_false_value = {.type = JSON_BOOLEAN, .value = {.boolean = 0}};

/**
 * @section JSON write helper methods
 */
static int json_write_escaped_string(const char *str, FILE *out);
static int json_write_array(struct json *array, FILE *out);
static int json_write_object(struct json *object, FILE *out);
static int json_write_string(struct json *node, FILE *out);
static int json_write_number(struct json *node, FILE *out);
static int json_write_boolean(struct json *node, FILE *out);
static int json_write_null(struct json *node, FILE *out);

/**
 * @section JSON read helper methods
 */

// json read functions
// :: JSON := LITERAL | ARRAY | OBJECT
// :: LITERAL := NULL | TRUE | FALSE | NUMBER | STRING (lexical representation)
// :: ARRAY := "[" (ARRAY_LIST "]"
// :: ARRAY_LIST := JSON | JSON "," ARRAY_LIST
// :: OBJECT := "{" KEYVAL ("," KEYVAL)* "}"
// :: KEYVAL := STRING ":" JSON

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

// Helper for strings
static void strprep(char *dst, const char *src);

// Helper for error handling
struct error_context
{
  char *message;
  int line;
  int column;
};
static int update_error_context(struct error_context *errctx, const char c, int index);
static char __default_errbuf[LIBJSON_ERRBUF_SiZE];

// Helper function for parsing JSON
static struct json_token json_read_token(FILE *in, struct error_context *errctx);
static int json_parser_json(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx);
static int json_parser_literal(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx);
static int json_parser_array(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx);
static int json_parser_object(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx);
static int json_parser_key_value(FILE *in, struct json_token *token, struct json *object, struct error_context *errctx);

/**
 * @section JSON creation functions
 */

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
  if (!value)
    return &json_null_value;
  struct json *node = (struct json *)malloc(sizeof(struct json));
  if (!node)
    return NULL;

  node->type = JSON_STRING;
  node->value.string = strdup(value);
  if (!node->value.string)
  {
    free(node);
    return NULL;
  }
  return node;
}

struct json *__json_array_macro(struct json *elements[])
{
  struct json *node = (struct json *)malloc(sizeof(struct json));
  if (!node)
    return NULL;

  node->type = JSON_ARRAY;
  node->value.array = NULL; // Empty array initially
  while (elements && *elements)
  {
    json_array_push(node, *elements);
    elements++;
  }
  return node;
}

struct json *__json_object_macro(struct json_key_value elements[])
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
  while (elements && elements->key)
  {
    struct json *value = elements->value;
    hash_table_set(node->value.object, elements->key, value);
    elements++;
  }
  return node;
}

struct json *json_copy(struct json *json)
{
  // Null and singleton static values can be returned directly
  if (!json || json == &json_null_value || json == &json_true_value || json == &json_false_value)
    return json;

  struct json *copy = (struct json *)malloc(sizeof(struct json));
  if (!copy)
    return NULL;

  copy->type = json->type;
  switch (json->type)
  {
  case JSON_BOOLEAN:
    copy->value.boolean = json->value.boolean;
    break;
  case JSON_NUMBER:
    copy->value.number = json->value.number;
    break;
  case JSON_STRING:
    copy->value.string = strdup(json->value.string);
    if (!copy->value.string)
    {
      free(copy);
      return NULL;
    }
    break;
  case JSON_ARRAY:
    struct linked_list_iter *ll_iter = linked_list_iter_new(json->value.array);
    if (!ll_iter)
    {
      free(copy);
      return NULL;
    }
    copy->value.array = NULL;
    struct json *element;
    while ((element = linked_list_iter_next(ll_iter)))
    {
      struct json *element_copy = json_copy(element);
      if (!element_copy)
      {
        linked_list_iter_free(ll_iter);
        linked_list_free(copy->value.array, (free_func)json_free);
        free(copy);
        return NULL;
      }
      json_array_push(copy, element_copy);
    }
    break;
  case JSON_OBJECT:
    copy->value.object = hash_table_new();
    struct hash_table_iter *ht_iter = hash_table_iter_new(json->value.object);
    if (!copy->value.object || !ht_iter)
    {
      free(copy);
      return NULL;
    }
    struct hash_table_entry *entry;
    while ((entry = hash_table_iter_next(ht_iter)))
    {
      struct json *value_copy = json_copy((struct json *)hash_table_entry_value(entry));
      if (!value_copy)
      {
        hash_table_iter_free(ht_iter);
        hash_table_free(copy->value.object, (free_func)json_free);
        free(copy);
        return NULL;
      }
      hash_table_set(copy->value.object, hash_table_entry_key(entry), value_copy);
    }
    hash_table_iter_free(ht_iter);
    break;
  }
  return copy;
}

void json_free(struct json *json)
{
  if (!json || json == &json_null_value || json == &json_true_value || json == &json_false_value)
    return;

  switch (json->type)
  {
  case JSON_ARRAY:
    linked_list_free(json->value.array, (free_func)json_free);
    break;
  case JSON_OBJECT:
    hash_table_free(json->value.object, (free_func)json_free);
    break;
  case JSON_STRING:
    free(json->value.string);
    break;
  default:
    break;
  }
  free(json);
}

/**
 * @section JSON type checking functions
 */

int json_is_null(struct json *node)
{
  return node == &json_null_value;
}

int json_is_boolean(struct json *node)
{
  return node && node->type == JSON_BOOLEAN;
}

int json_is_number(struct json *node)
{
  return node && node->type == JSON_NUMBER;
}

int json_is_string(struct json *node)
{
  return node && node->type == JSON_STRING;
}

int json_is_array(struct json *node)
{
  return node && node->type == JSON_ARRAY;
}

int json_is_object(struct json *node)
{
  return node && node->type == JSON_OBJECT;
}

/**
 * @section JSON manipulation functions
 */

/**
 * @subsection JSON array manipulation functions
 */

void json_array_push(struct json *array, struct json *value)
{
  if (!array || !value || !json_is_array(array))
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

int json_array_length(struct json *array)
{
  if (!array || !json_is_array(array))
    return 0;

  return linked_list_length(array->value.array);
}

struct json *json_array_get(const struct json *array, int index)
{
  if (!array || !json_is_array((struct json *)array) || index < 0)
    return NULL;

  struct linked_list *node = array->value.array;
  for (int i = 0; i < index && node; i++)
  {
    node = linked_list_next(node);
  }
  return (struct json *)linked_list_value(node);
}

/**
 * @subsection JSON object manipulation functions
 */

void json_object_set(struct json *object, const char *key, struct json *value)
{
  if (!object || !key || !value || !json_is_object(object))
    return;

  hash_table_set(object->value.object, key, value);
}

struct json *json_object_get(const struct json *object, const char *key)
{
  if (!object || !key || !json_is_object((struct json *)object))
    return NULL;

  return (struct json *)hash_table_get(object->value.object, key);
}

int json_object_length(struct json *object)
{
  if (!object || !json_is_object(object))
    return 0;

  return hash_table_keys(object->value.object, NULL);
}

struct json *json_object_remove(struct json *object, const char *key)
{
  if (!object || !key || !json_is_object(object))
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

const char *json_error(char *errbuf) {
  if (!errbuf)
    errbuf = __default_errbuf;
  if (!errbuf || strlen(errbuf) == 0)
    return NULL;

  return strdup(errbuf);
}

/**
 * @section JSON access functions
 */

double json_double_value(const struct json *node)
{
  if (!node || node->type != JSON_NUMBER)
    return 0.0;
  return node->value.number;
}

int json_int_value(const struct json *node)
{
  if (!node || node->type != JSON_NUMBER)
    return 0;
  return (int)node->value.number;
}

const char *json_string_value(const struct json *node)
{
  if (!node || node->type != JSON_STRING)
    return NULL;
  return strdup(node->value.string);
}

/**
 * @section JSON serialization/deserialization functions
 */

/**
 * @subsection JSON write function
 */

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

static int json_write_null(struct json *node, FILE *out)
{
  if (!node || !out)
    return -1;

  return fprintf(out, "null");
}

static int json_write_boolean(struct json *node, FILE *out)
{
  if (!node || !out)
    return -1;

  return fprintf(out, node->value.boolean ? "true" : "false");
}

static int json_write_number(struct json *node, FILE *out)
{
  if (!node || !out)
    return -1;

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

static int json_write_string(struct json *node, FILE *out)
{
  if (!node || !out)
    return -1;

  return json_write_escaped_string(node->value.string, out);
}

static int json_write_array(struct json *node, FILE *out)
{
  if (!node || !out)
    return -1;

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
    int ret = json_write(element, out);
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

int json_write_object(struct json *node, FILE *out)
{
  if (!node || !out)
    return -1;

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

    ret = json_write(hash_table_entry_value(entry), out);
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

int json_write(struct json *node, FILE *out)
{
  if (!node || !out)
    return -1;

  switch (node->type)
  {
  case JSON_NULL:
    return json_write_null(node, out);
  case JSON_BOOLEAN:
    return json_write_boolean(node, out);
  case JSON_NUMBER:
    return json_write_number(node, out);
  case JSON_STRING:
    return json_write_string(node, out);
  case JSON_ARRAY:
    return json_write_array(node, out);
  case JSON_OBJECT:
    return json_write_object(node, out);
  }
}

/**
 * @subsection JSON read Functions
 */

struct json *json_read(FILE *in, char *errbuf)
{
  struct json_token token;
  char linecol[64];
  if (!in)
    return NULL;
  if (!errbuf)
    errbuf = __default_errbuf;
  struct error_context errctx = {
      .message = errbuf,
      .line = 0,
      .column = 0};
  token = json_read_token(in, &errctx);
  struct json *result = NULL;
  if (token.type != JSON_TOKEN_INVALID && json_parser_json(in, &token, &result, &errctx))
  {
    // Sucessfully parsed JSON
    errbuf[0] = '\0';
    return result;
  }
  int initial_message_length = strlen(errctx.message);
  sprintf(linecol, "(%d:%d): ", errctx.line + 1, errctx.column);
  strprep(errctx.message, linecol);
  strprep(errctx.message, "Error parsing JSON ");
  if (result)
  {
    json_free(result);
    result = NULL;
  }
  return result;
}

struct json *json_read_string(const char *json_string, char *errbuf)
{
  if (!json_string)
    return NULL;

  FILE *memfile = fmemopen((void *)json_string, strlen(json_string), "r");
  if (!memfile)
    return NULL;

  struct json *result = json_read(memfile, errbuf);
  fclose(memfile);
  return result;
}

static void strprep(char *dst, const char *src)
{
  int len_src = strlen(src);
  int len_dst = strlen(dst);
  // Shift existing string to the right
  memmove(dst + len_src, dst, len_dst + 1);
  // Copy new string to the beginning
  memcpy(dst, src, len_src);
}

static int update_error_context(struct error_context *errctx, const char c, int index)
{
  if (errctx)
  {
    if (c == '\n' || c == '\r')
    {
      errctx->line++;
      errctx->column = 0;
    }
    else if (c == ' ')
    {
      errctx->column = 0;
    }
    else
    {
      errctx->column++;
    }
    errctx->message[index] = c;
  }
  return (int)c;
}

static int json_parser_json(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx)
{
  return json_parser_literal(in, token, dest, errctx) || json_parser_array(in, token, dest, errctx) || json_parser_object(in, token, dest, errctx);
}

static int json_parser_array(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx)
{
  if (token->type == JSON_TOKEN_ARRAY_START)
  {
    *dest = json_array();
    *token = json_read_token(in, errctx);
    struct json *element = NULL;
    if (json_parser_json(in, token, &element, errctx))
    {
      json_array_push(*dest, element);
      *token = json_read_token(in, errctx);
      while (token->type == JSON_TOKEN_COMMA)
      {
        *token = json_read_token(in, errctx);
        if (json_parser_json(in, token, &element, errctx))
        {
          json_array_push(*dest, element);
          *token = json_read_token(in, errctx);
        }
        else // Error: Unexpected inner JSON
        {
          json_free(*dest);
          *dest = NULL;
          return 0;
        }
      }
    }
    if (token->type == JSON_TOKEN_ARRAY_END)
    {
      return 1;
    }
    else // Error: Unexpected token
    {
      json_free(*dest);
      *dest = NULL;
      return 0;
    }
  }
  // Not an array
  return 0;
}

static int json_parser_object(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx)
{
  if (token->type == JSON_TOKEN_OBJECT_START)
  {
    *dest = json_object();
    *token = json_read_token(in, errctx);
    if (json_parser_key_value(in, token, *dest, errctx))
    {
      *token = json_read_token(in, errctx);
      while (token->type == JSON_TOKEN_COMMA)
      {
        *token = json_read_token(in, errctx);
        if (json_parser_key_value(in, token, *dest, errctx))
        {
          *token = json_read_token(in, errctx);
        }
        else // Error: Unexpected inner JSON
        {
          json_free(*dest);
          *dest = NULL;
          return 0;
        }
      }
    }
    if (token->type == JSON_TOKEN_OBJECT_END)
    {
      return 1;
    }
    else // Error: Unexpected token.
    {
      json_free(*dest);
      *dest = NULL;
      return 0;
    }
  }
  // Not an object
  return 0;
}

static int json_parser_key_value(FILE *in, struct json_token *token, struct json *object, struct error_context *errctx)
{
  if (token->type == JSON_TOKEN_STRING)
  {
    char *key = strdup(token->value);
    *token = json_read_token(in, errctx);
    if (token->type == JSON_TOKEN_COLON)
    {
      *token = json_read_token(in, errctx);
      struct json *value = NULL;
      if (json_parser_json(in, token, &value, errctx))
      {
        json_object_set(object, key, value);
        free(key);
        return 1;
      }
      else
      {
        free(key);
        return 0;
      }
    }
    else
    {
      free(key);
      return 0;
    }
  }
  else
  {
    return 0;
  }
}

static int json_parser_literal(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx)
{
  if (token->type == JSON_TOKEN_NULL)
  {
    *dest = json_null();
    return 1;
  }
  else if (token->type == JSON_TOKEN_TRUE)
  {
    *dest = json_true();
    return 1;
  }
  else if (token->type == JSON_TOKEN_FALSE)
  {
    *dest = json_false();
    return 1;
  }
  else if (token->type == JSON_TOKEN_NUMBER)
  {
    *dest = json_number(atof(token->value));
    return 1;
  }
  else if (token->type == JSON_TOKEN_STRING)
  {
    *dest = json_string(token->value);
    return 1;
  }
  return 0;
}

static struct json_token json_read_token(FILE *in, struct error_context *errctx)
{
  struct json_token token = {0};
  int c, i = 0;
  const char *error = "Invalid token: ";
  int error_length = strlen(error);

  // Skip whitespace
  while (isspace(c = update_error_context(errctx, fgetc(in), i)))
    ;

  if (c == EOF)
  {
    token.type = JSON_TOKEN_EOF;
    return token;
  }

  switch (c)
  {
  case 'n':
  {
    i = 1;
    if (
        update_error_context(errctx, fgetc(in), i++) == 'u' &&
        update_error_context(errctx, fgetc(in), i++) == 'l' &&
        update_error_context(errctx, fgetc(in), i++) == 'l')
    {
      token.type = JSON_TOKEN_NULL;
    }
    else
    {
      token.type = JSON_TOKEN_INVALID;
    }
    break;
  }
  case 't':
  {
    i = 1;
    if (
        update_error_context(errctx, fgetc(in), i++) == 'r' &&
        update_error_context(errctx, fgetc(in), i++) == 'u' &&
        update_error_context(errctx, fgetc(in), i++) == 'e')
    {
      token.type = JSON_TOKEN_TRUE;
    }
    else
    {
      token.type = JSON_TOKEN_INVALID;
    }
    break;
  }
  case 'f':
  {
    i = 1;
    if (
        update_error_context(errctx, fgetc(in), i++) == 'a' &&
        update_error_context(errctx, fgetc(in), i++) == 'l' &&
        update_error_context(errctx, fgetc(in), i++) == 's' &&
        update_error_context(errctx, fgetc(in), i++) == 'e')
    {
      token.type = JSON_TOKEN_FALSE;
    }
    else
    {

      token.type = JSON_TOKEN_INVALID;
    }
    break;
  }
  case '[':
  {
    token.type = JSON_TOKEN_ARRAY_START;
    break;
  }
  case ']':
  {
    token.type = JSON_TOKEN_ARRAY_END;
    break;
  }
  case '{':
  {
    token.type = JSON_TOKEN_OBJECT_START;
    break;
  }
  case '}':
  {
    token.type = JSON_TOKEN_OBJECT_END;
    break;
  }
  case ',':
  {
    token.type = JSON_TOKEN_COMMA;
    break;
  }
  case ':':
  {
    token.type = JSON_TOKEN_COLON;
    break;
  }
  case '"':
  {
    char *str = NULL;
    struct linked_list *char_list = NULL, *char_list_ptr = NULL;
    i = 1;
    while ((c = update_error_context(errctx, fgetc(in), i++)) != '"')
    {
      if (c == '\\')
      {
        c = update_error_context(errctx, fgetc(in), i++);
        switch (c)
        {
        case 'b':
          c = '\b';
          break;
        case 'f':
          c = '\f';
          break;
        case 'n':
          c = '\n';
          break;
        case 'r':
          c = '\r';
          break;
        case 't':
          c = '\t';
          break;
        case '\\':
          c = '\\';
          break;
        case 'u':
        {
          char hex[5] = {0, 0, 0, 0, 0};
          for (int i = 0; i < 4 && isxdigit(c = update_error_context(errctx, fgetc(in), i++)); i++)
          {
            hex[i] = c;
          }
          if (c == EOF)
          {
            token.type = JSON_TOKEN_INVALID;
            break;
          }
          c = (char)strtol(hex, NULL, 16);
        }
        break;
        default: // Error: Invalid escape sequence
          token.type = JSON_TOKEN_INVALID;
          break;
        }
      }
      if (char_list)
      {
        char_list_ptr = linked_list_insert(char_list_ptr, (void *)c);
      }
      else
      {
        char_list = linked_list_new((void *)c);
        char_list_ptr = char_list;
      }
    }
    if (char_list)
    {
      int length = linked_list_length(char_list);
      str = (char *)malloc(length + 1);
      if (str)
      {
        struct linked_list *node = char_list;
        for (int j = 0; j < length; j++)
        {
          str[j] = (char)linked_list_value(node);
          node = linked_list_next(node);
        }
        str[length] = '\0';
        token.value = str;
        token.type = JSON_TOKEN_STRING;
        linked_list_free(char_list, NULL);
      }
    }
    else
    {
      token.type = JSON_TOKEN_INVALID;
    }
    break;
  }
  default: // Number or INVALID
  {
    ungetc(c, in);
    char buffer[32];
    int j = 0;
    i = 0; // Because of ungetc, we need to reset i
    while (isdigit(c = update_error_context(errctx, fgetc(in), i++)) || c == '.' || c == '-' || c == '+' || c == 'e' || c == 'E')
    {
      if (j < sizeof(buffer) - 1)
      {
        buffer[j++] = c;
      }
      else
      {
        token.type = JSON_TOKEN_INVALID;
        break;
      }
    }
    if (c != EOF)
    {
      ungetc(c, in);
      i--; // Adjust index for ungetc
    }
    buffer[j] = '\0';
    if (j > 0)
    {
      token.value = strdup(buffer);
      token.type = JSON_TOKEN_NUMBER;
    }
    else
    {
      token.type = JSON_TOKEN_INVALID;
    }
    break;
  }
  }
  if (token.type == JSON_TOKEN_INVALID)
  {
    errctx->message[i] = '\0';
    strprep(errctx->message, error);
  }
  return token;
}