#include "json.h"
#include "ds.h"

#include <stdlib.h>
#include <string.h>

struct __JSON_struct {
  enum {
    JSON_DECIMAL,
    JSON_INTEGER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
  } type;
  union {
    double decimal;
    int integer;
    char *string;
    int boolean;
    struct {
      struct LL *first;
      struct LL *last;
    } array;
    struct HT *object; // of struct JSONNode
  } value;
};

struct __JSON_struct __restin_json_null;
struct __JSON_struct __restin_json_true;
struct __JSON_struct __restin_json_false;

JSON JSON_null() { return &__restin_json_null; }

JSON JSON_true() { return &__restin_json_true; }

JSON JSON_false() { return &__restin_json_false; }

JSON JSON_decimal(double value) {
  JSON json = malloc(sizeof(struct __JSON_struct));
  json->type = JSON_DECIMAL;
  json->value.decimal = value;
  return json;
}

JSON JSON_integer(int value) {
  JSON json = malloc(sizeof(struct __JSON_struct));
  json->type = JSON_DECIMAL;
  json->value.integer = value;
  return json;
}

JSON JSON_string(const char *value) {
  JSON json = malloc(sizeof(struct __JSON_struct));
  json->type = JSON_STRING;
  return json;
}

JSON JSON_array() {
  JSON json = malloc(sizeof(struct __JSON_struct));
  json->type = JSON_ARRAY;
  json->value.array.first = NULL;
  json->value.array.last = NULL;
  return json;
}

JSON JSON_object() {
  JSON json = malloc(sizeof(struct __JSON_struct));
  json->type = JSON_OBJECT;
  json->value.object = HT_new();
  return json;
}

int JSON_isnull(JSON node) { return node == &__restin_json_null; }

int JSON_isboolean(JSON node) {
  return node == &__restin_json_true || node == &__restin_json_false;
}

int JSON_isnumber(JSON node) {
  return node->type == JSON_DECIMAL || node->type == JSON_INTEGER;
}

int JSON_isstring(JSON node) { return node->type == JSON_STRING; }

int JSON_isarray(JSON node) { return node->type == JSON_ARRAY; }

int JSON_isobject(JSON node) { return node->type == JSON_OBJECT; }

void JSON_object_set(JSON object, const char *key, JSON value) {
  HT_set(object->value.object, key, value);
}

JSON JSON_object_get(const JSON object, const char *key) {
  return HT_get(object->value.object, key);
}

void JSON_array_push(JSON array, JSON value) {
  if (array->value.array.first == NULL) {
    struct LL *node = LL_new(value);
    array->value.array.first = node;
    array->value.array.last = node;
  } else {
    array->value.array.last = LL_insert(array->value.array.last, value);
  }
}

int JSON_fwrite(JSON node, FILE *out) {
  if (node->type == JSON_DECIMAL) {
    return fprintf(out, "%f", node->value.decimal);
  } else if (node->type == JSON_INTEGER) {
    return fprintf(out, "%d", node->value.integer);
  } else if (node->type == JSON_STRING) {
    return fprintf(out, "\"%s\"", node->value.string);
  } else if (node->type == JSON_ARRAY) {
    fprintf(out, "[");
    struct LL *current = node->value.array.first;
    while (current != NULL) {
      JSON_fwrite(LL_value(current), out);
      current = LL_next(current);
      if (current != NULL) {
        fprintf(out, ",");
      }
    }
    fprintf(out, "]");
  } else if (node->type == JSON_OBJECT) {
    fprintf(out, "{");
    char *keys[9999];
    int size = HT_keys(node->value.object, keys);
    for (int i = 0; i < size; i++) {
      fprintf(out, "\"%s\":", keys[i]);
      JSON_fwrite(HT_get(node->value.object, keys[i]), out);
      if (i < size - 1) {
        fprintf(out, ",");
      }
    }
    fprintf(out, "}");
  }
  return 0;
}

void __JSON_HT_free(char *key, void *value) { JSON_free(value); }
void __JSON_LL_free(void *value) { JSON_free(value); }
void JSON_free(JSON json) {
  if (json->type == JSON_STRING) {
    free(json->value.string);
  } else if (json->type == JSON_ARRAY) {
    LL_foreach(json->value.array.first, __JSON_LL_free);
    LL_free(json->value.array.first);
  } else if (json->type == JSON_OBJECT) {
    HT_foreach(json->value.object, __JSON_HT_free);
    HT_free(json->value.object, 1);
  }
  free(json);
}
