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

// Forward declarations of helper functions
// string scaping function
static int json_write_escaped_string(const char *str, FILE *out);
// json_write functions
static int json_write_array(struct json *array, FILE *out);
static int json_write_object(struct json *object, FILE *out);
static int json_write_string(struct json *node, FILE *out);
static int json_write_number(struct json *node, FILE *out);
static int json_write_boolean(struct json *node, FILE *out);
static int json_write_null(struct json *node, FILE *out);
// json_free function
static void json_free_string(struct json *json);
static void json_free_array(struct json *json);
static void json_free_object(struct json *json);

// Parser helper functions
static int json_parser_skip_whitespace(FILE *in);
static struct json *json_parse_value(FILE *in);
static struct json *json_parse_object(FILE *in);
static struct json *json_parse_array(FILE *in);
static struct json *json_parse_string(FILE *in);
static struct json *json_parse_number(FILE *in, int first_char);
static struct json *json_parse_literal(FILE *in, int first_char);
static char *json_parse_unescaped_string(FILE *in);
static char json_parse_escape_sequence(FILE *in);

// Error handling for parser
static char parse_error_buffer[256];
static int parse_line = 1;
static int parse_column = 0;

static void set_parse_error(const char *format, ...);

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

static void json_free_string(struct json *json)
{
  if (json->type == JSON_STRING)
  {
    // Free the string value
    free(json->value.string);
  }
}

static void json_free_array(struct json *json)
{
  if (json->type == JSON_ARRAY)
  {
    struct linked_list_iter *iter = linked_list_iter_new(json->value.array);
    struct json *element;
    while (element = linked_list_iter_next(iter))
    {
      json_free(element);
    }
    linked_list_iter_free(iter);
  }
}

static void json_free_object(struct json *json)
{
  if (json->type == JSON_OBJECT)
  {
    struct hash_table_iter *iter = hash_table_iter_new(json->value.object);
    struct hash_table_entry *entry;
    while (entry = hash_table_iter_next(iter))
    {
      struct json *value = (struct json *)hash_table_entry_value(entry);
      if (value)
        json_free(value);
    }
    hash_table_iter_free(iter);
  }
}

void json_free(struct json *json)
{
  if (!json || json == &json_null_value || json == &json_true_value || json == &json_false_value)
    return;

  switch (json->type)
  {
  case JSON_STRING:
    json_free_string(json);
    break;
  case JSON_ARRAY:
    json_free_array(json);
    break;
  case JSON_OBJECT:
    json_free_object(json);
    break;
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
  return strdup(node->value.string);
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

/************************************
 * JSON Parser Functions
 ************************************/

/**
 * Record an error message during parsing
 */
static void set_parse_error(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vsnprintf(parse_error_buffer, sizeof(parse_error_buffer), format, args);
  va_end(args);
}

/**
 * Get the last error message from parsing
 */
const char *json_error()
{
  return parse_error_buffer[0] != '\0' ? parse_error_buffer : NULL;
}

/**
 * Skip whitespace characters in the input stream.
 * Track line and column numbers for error reporting.
 */
static int json_parser_skip_whitespace(FILE *in)
{
  int c;
  while ((c = fgetc(in)) != EOF)
  {
    if (c == ' ' || c == '\t' || c == '\r')
    {
      parse_column++;
      continue;
    }
    else if (c == '\n')
    {
      parse_line++;
      parse_column = 0;
      continue;
    }
    else
    {
      // Not a whitespace character, unget it and return
      ungetc(c, in);
      return 1;
    }
  }

  // End of file reached
  return 0;
}

/**
 * Parse a JSON value from the input stream
 */
static struct json *json_parse_value(FILE *in)
{
  if (!json_parser_skip_whitespace(in))
  {
    set_parse_error("Unexpected end of file");
    return NULL;
  }

  int c = fgetc(in);
  parse_column++;

  switch (c)
  {
  case '{':
    ungetc(c, in);
    parse_column--;
    return json_parse_object(in);

  case '[':
    ungetc(c, in);
    parse_column--;
    return json_parse_array(in);

  case '"':
    return json_parse_string(in);

  case 't':
  case 'f':
  case 'n':
    ungetc(c, in);
    parse_column--;
    return json_parse_literal(in, c);

  case '-':
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    return json_parse_number(in, c);

  default:
    set_parse_error("Unexpected character '%c' at line %d, column %d", c, parse_line, parse_column);
    return NULL;
  }
}

/**
 * Parse a JSON object from the input stream
 */
static struct json *json_parse_object(FILE *in)
{
  // Expect an opening brace
  int c = fgetc(in);
  parse_column++;

  if (c != '{')
  {
    set_parse_error("Expected '{' at line %d, column %d", parse_line, parse_column);
    return NULL;
  }

  struct json *obj = json_object();
  if (!obj)
  {
    set_parse_error("Failed to allocate memory for JSON object");
    return NULL;
  }

  // Skip whitespace after opening brace
  if (!json_parser_skip_whitespace(in))
  {
    set_parse_error("Unexpected end of file after '{'");
    json_free(obj);
    return NULL;
  }

  // Check for empty object
  c = fgetc(in);
  parse_column++;

  if (c == '}')
  {
    // Empty object
    return obj;
  }
  ungetc(c, in);
  parse_column--;

  // Parse key-value pairs
  int first = 1;
  while (1)
  {
    if (!first)
    {
      // Skip whitespace after comma
      if (!json_parser_skip_whitespace(in))
      {
        set_parse_error("Unexpected end of file in object");
        json_free(obj);
        return NULL;
      }
    }

    // Parse the key (must be a string)
    if (!json_parser_skip_whitespace(in))
    {
      set_parse_error("Unexpected end of file in object");
      json_free(obj);
      return NULL;
    }

    c = fgetc(in);
    parse_column++;

    if (c != '"')
    {
      set_parse_error("Expected string key in object at line %d, column %d", parse_line, parse_column);
      json_free(obj);
      return NULL;
    }

    char *key = json_parse_unescaped_string(in);
    if (!key)
    {
      json_free(obj);
      return NULL;
    }

    // Skip whitespace after key
    if (!json_parser_skip_whitespace(in))
    {
      set_parse_error("Unexpected end of file after key");
      free(key);
      json_free(obj);
      return NULL;
    }

    // Expect a colon
    c = fgetc(in);
    parse_column++;

    if (c != ':')
    {
      set_parse_error("Expected ':' after key at line %d, column %d", parse_line, parse_column);
      free(key);
      json_free(obj);
      return NULL;
    }

    // Parse the value
    struct json *value = json_parse_value(in);
    if (!value)
    {
      free(key);
      json_free(obj);
      return NULL;
    }

    // Add the key-value pair to the object
    json_object_set(obj, key, value);
    free(key);

    // Skip whitespace after value
    if (!json_parser_skip_whitespace(in))
    {
      set_parse_error("Unexpected end of file in object");
      json_free(obj);
      return NULL;
    }

    // Check for end of object or next key-value pair
    c = fgetc(in);
    parse_column++;

    if (c == '}')
    {
      // End of object
      return obj;
    }
    else if (c == ',')
    {
      // More key-value pairs to come
      first = 0;
      continue;
    }
    else
    {
      set_parse_error("Expected '}' or ',' at line %d, column %d", parse_line, parse_column);
      json_free(obj);
      return NULL;
    }
  }
}

/**
 * Parse a JSON array from the input stream
 */
static struct json *json_parse_array(FILE *in)
{
  // Expect an opening bracket
  int c = fgetc(in);
  parse_column++;

  if (c != '[')
  {
    set_parse_error("Expected '[' at line %d, column %d", parse_line, parse_column);
    return NULL;
  }

  struct json *arr = json_array();
  if (!arr)
  {
    set_parse_error("Failed to allocate memory for JSON array");
    return NULL;
  }

  // Skip whitespace after opening bracket
  if (!json_parser_skip_whitespace(in))
  {
    set_parse_error("Unexpected end of file after '['");
    json_free(arr);
    return NULL;
  }

  // Check for empty array
  c = fgetc(in);
  parse_column++;

  if (c == ']')
  {
    // Empty array
    return arr;
  }
  ungetc(c, in);
  parse_column--;

  // Parse array elements
  int first = 1;
  while (1)
  {
    if (!first)
    {
      // Skip whitespace after comma
      if (!json_parser_skip_whitespace(in))
      {
        set_parse_error("Unexpected end of file in array");
        json_free(arr);
        return NULL;
      }
    }

    // Parse value
    struct json *value = json_parse_value(in);
    if (!value)
    {
      json_free(arr);
      return NULL;
    }

    // Add the value to the array
    json_array_push(arr, value);

    // Skip whitespace after value
    if (!json_parser_skip_whitespace(in))
    {
      set_parse_error("Unexpected end of file in array");
      json_free(arr);
      return NULL;
    }

    // Check for end of array or next element
    c = fgetc(in);
    parse_column++;

    if (c == ']')
    {
      // End of array
      return arr;
    }
    else if (c == ',')
    {
      // More elements to come
      first = 0;
      continue;
    }
    else
    {
      set_parse_error("Expected ']' or ',' at line %d, column %d", parse_line, parse_column);
      json_free(arr);
      return NULL;
    }
  }
}

/**
 * Parse a single character in an escape sequence
 */
static char json_parse_escape_sequence(FILE *in)
{
  int c = fgetc(in);
  parse_column++;

  switch (c)
  {
  case '"':
    return '"';
  case '\\':
    return '\\';
  case '/':
    return '/';
  case 'b':
    return '\b';
  case 'f':
    return '\f';
  case 'n':
    return '\n';
  case 'r':
    return '\r';
  case 't':
    return '\t';
  case 'u':
  {
    // Unicode escape sequence, e.g., \u00A9 for ©
    char hex[5];
    for (int i = 0; i < 4; i++)
    {
      hex[i] = fgetc(in);
      parse_column++;

      if (!isxdigit(hex[i]))
      {
        set_parse_error("Invalid Unicode escape sequence at line %d, column %d", parse_line, parse_column);
        return '\0'; // Error indicator
      }
    }
    hex[4] = '\0';

    // Parse the 4-digit hex value
    unsigned int unicode_value;
    sscanf(hex, "%x", &unicode_value);

    // For simplicity, we only support BMP characters (U+0000 to U+FFFF)
    // and don't handle surrogate pairs or characters outside the BMP.
    // In practice, this would need more complex UTF-8 encoding.
    if (unicode_value <= 0x7F)
    {
      // ASCII range
      return (char)unicode_value;
    }
    else
    {
      // Non-ASCII Unicode, not fully supported in this simple parser
      set_parse_error("Non-ASCII Unicode characters not fully supported at line %d, column %d", parse_line, parse_column);
      return '?'; // Placeholder character
    }
  }
  default:
    set_parse_error("Invalid escape sequence '\\%c' at line %d, column %d", c, parse_line, parse_column);
    return '\0'; // Error indicator
  }
}

/**
 * Parse a string from the input stream, handling escape sequences
 * Assumes the opening quote has already been consumed
 */
static char *json_parse_unescaped_string(FILE *in)
{
  size_t buffer_size = 16; // Initial buffer size
  size_t buffer_pos = 0;
  char *buffer = (char *)malloc(buffer_size);

  if (!buffer)
  {
    set_parse_error("Failed to allocate memory for string");
    return NULL;
  }

  int c;

  while ((c = fgetc(in)) != EOF)
  {
    parse_column++;

    if (c == '"')
    {
      // End of string
      buffer[buffer_pos] = '\0';
      return buffer;
    }
    else if (c == '\\')
    {
      // Escape sequence
      char escaped_char = json_parse_escape_sequence(in);
      if (escaped_char == '\0')
      {
        // Error in escape sequence
        free(buffer);
        return NULL;
      }

      // Add escaped character to buffer
      if (buffer_pos + 1 >= buffer_size)
      {
        // Expand buffer if needed
        buffer_size *= 2;
        char *new_buffer = (char *)realloc(buffer, buffer_size);
        if (!new_buffer)
        {
          set_parse_error("Failed to allocate memory for string");
          free(buffer);
          return NULL;
        }
        buffer = new_buffer;
      }

      buffer[buffer_pos++] = escaped_char;
    }
    else if (c < 32)
    {
      // Control character not allowed in JSON strings
      set_parse_error("Control character in string at line %d, column %d", parse_line, parse_column);
      free(buffer);
      return NULL;
    }
    else
    {
      // Regular character
      if (buffer_pos + 1 >= buffer_size)
      {
        // Expand buffer if needed
        buffer_size *= 2;
        char *new_buffer = (char *)realloc(buffer, buffer_size);
        if (!new_buffer)
        {
          set_parse_error("Failed to allocate memory for string");
          free(buffer);
          return NULL;
        }
        buffer = new_buffer;
      }

      buffer[buffer_pos++] = (char)c;
    }
  }

  set_parse_error("Unexpected end of file in string");
  free(buffer);
  return NULL;
}

/**
 * Parse a JSON string from the input stream
 * Assumes the opening quote has already been consumed
 */
static struct json *json_parse_string(FILE *in)
{
  char *str = json_parse_unescaped_string(in);
  if (!str)
  {
    return NULL;
  }

  struct json *node = json_string(str);
  free(str);

  if (!node)
  {
    set_parse_error("Failed to allocate memory for JSON string");
    return NULL;
  }

  return node;
}

/**
 * Parse a JSON number from the input stream
 */
static struct json *json_parse_number(FILE *in, int first_char)
{
  size_t buffer_size = 32; // Should be enough for most numbers
  size_t buffer_pos = 0;
  char *buffer = (char *)malloc(buffer_size);

  if (!buffer)
  {
    set_parse_error("Failed to allocate memory for number");
    return NULL;
  }

  // Add first character to buffer
  buffer[buffer_pos++] = (char)first_char;

  // Parse the rest of the number
  int c;
  int has_decimal = 0;
  int has_exponent = 0;

  while ((c = fgetc(in)) != EOF)
  {
    if (isdigit(c))
    {
      // Digit
      if (buffer_pos + 1 >= buffer_size)
      {
        buffer_size *= 2;
        char *new_buffer = (char *)realloc(buffer, buffer_size);
        if (!new_buffer)
        {
          set_parse_error("Failed to allocate memory for number");
          free(buffer);
          return NULL;
        }
        buffer = new_buffer;
      }

      buffer[buffer_pos++] = (char)c;
      parse_column++;
    }
    else if (c == '.')
    {
      // Decimal point
      if (has_decimal || has_exponent)
      {
        set_parse_error("Invalid number format at line %d, column %d", parse_line, parse_column);
        free(buffer);
        return NULL;
      }

      has_decimal = 1;

      if (buffer_pos + 1 >= buffer_size)
      {
        buffer_size *= 2;
        char *new_buffer = (char *)realloc(buffer, buffer_size);
        if (!new_buffer)
        {
          set_parse_error("Failed to allocate memory for number");
          free(buffer);
          return NULL;
        }
        buffer = new_buffer;
      }

      buffer[buffer_pos++] = (char)c;
      parse_column++;
    }
    else if (c == 'e' || c == 'E')
    {
      // Exponent
      if (has_exponent)
      {
        set_parse_error("Invalid number format at line %d, column %d", parse_line, parse_column);
        free(buffer);
        return NULL;
      }

      has_exponent = 1;

      if (buffer_pos + 1 >= buffer_size)
      {
        buffer_size *= 2;
        char *new_buffer = (char *)realloc(buffer, buffer_size);
        if (!new_buffer)
        {
          set_parse_error("Failed to allocate memory for number");
          free(buffer);
          return NULL;
        }
        buffer = new_buffer;
      }

      buffer[buffer_pos++] = (char)c;
      parse_column++;

      // Check for sign after exponent
      c = fgetc(in);
      parse_column++;

      if (c == '+' || c == '-')
      {
        if (buffer_pos + 1 >= buffer_size)
        {
          buffer_size *= 2;
          char *new_buffer = (char *)realloc(buffer, buffer_size);
          if (!new_buffer)
          {
            set_parse_error("Failed to allocate memory for number");
            free(buffer);
            return NULL;
          }
          buffer = new_buffer;
        }

        buffer[buffer_pos++] = (char)c;
      }
      else
      {
        ungetc(c, in);
        parse_column--;
      }
    }
    else
    {
      // End of number
      ungetc(c, in);
      parse_column--;
      break;
    }
  }

  // Null-terminate the buffer
  if (buffer_pos + 1 >= buffer_size)
  {
    buffer_size += 1;
    char *new_buffer = (char *)realloc(buffer, buffer_size);
    if (!new_buffer)
    {
      set_parse_error("Failed to allocate memory for number");
      free(buffer);
      return NULL;
    }
    buffer = new_buffer;
  }

  buffer[buffer_pos] = '\0';

  // Convert the string to a double
  char *endptr;
  double value = strtod(buffer, &endptr);

  if (*endptr != '\0')
  {
    set_parse_error("Invalid number format: %s", buffer);
    free(buffer);
    return NULL;
  }

  free(buffer);

  struct json *node = json_number(value);
  if (!node)
  {
    set_parse_error("Failed to allocate memory for JSON number");
    return NULL;
  }

  return node;
}

/**
 * Parse a JSON literal (null, true, false) from the input stream
 */
static struct json *json_parse_literal(FILE *in, int first_char)
{
  if (first_char == 'n')
  {
    // null
    fgetc(in);
    int c;

    c = fgetc(in);
    parse_column++;
    if (c != 'u')
      goto literal_error;

    c = fgetc(in);
    parse_column++;
    if (c != 'l')
      goto literal_error;

    c = fgetc(in);
    parse_column++;
    if (c != 'l')
      goto literal_error;

    return json_null();
  }
  else if (first_char == 't')
  {
    // true
    fgetc(in);
    int c;

    c = fgetc(in);
    parse_column++;
    if (c != 'r')
      goto literal_error;

    c = fgetc(in);
    parse_column++;
    if (c != 'u')
      goto literal_error;

    c = fgetc(in);
    parse_column++;
    if (c != 'e')
      goto literal_error;

    return json_true();
  }
  else if (first_char == 'f')
  {
    // false
    fgetc(in);
    int c;

    c = fgetc(in);
    parse_column++;
    if (c != 'a')
      goto literal_error;

    c = fgetc(in);
    parse_column++;
    if (c != 'l')
      goto literal_error;

    c = fgetc(in);
    parse_column++;
    if (c != 's')
      goto literal_error;

    c = fgetc(in);
    parse_column++;
    if (c != 'e')
      goto literal_error;

    return json_false();
  }

literal_error:
  set_parse_error("Invalid literal at line %d, column %d", parse_line, parse_column);
  return NULL;
}

/**
 * Read a complete JSON value from a file stream
 */
struct json *json_read(FILE *in)
{
  if (!in)
  {
    set_parse_error("NULL file pointer");
    return NULL;
  }

  // Reset error message and position trackers
  parse_error_buffer[0] = '\0';
  parse_line = 1;
  parse_column = 0;

  // Skip initial whitespace
  if (!json_parser_skip_whitespace(in))
  {
    set_parse_error("Empty file");
    return NULL;
  }

  // Parse the root value
  struct json *root = json_parse_value(in);
  if (!root)
  {
    // Error message already set by the parsing function
    return NULL;
  }

  // Skip trailing whitespace
  if (json_parser_skip_whitespace(in))
  {
    int c = fgetc(in);
    if (c != EOF)
    {
      // Extra content after the JSON value
      set_parse_error("Extra content after JSON value at line %d, column %d", parse_line, parse_column);
      json_free(root);
      return NULL;
    }
  }

  return root;
}
