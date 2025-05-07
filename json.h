#ifndef LIBJSON_JSON_H
#define LIBJSON_JSON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

struct __JSON_struct;
typedef struct __JSON_struct *JSON;

typedef int (*JSONArrayFilter)(int index, JSON node);
typedef JSON (*JSONArrayMap)(int index, JSON node);
typedef JSON (*JSONArrayReduce)(JSON acc, int index, JSON node);
typedef void (*JSONArrayForeach)(int index, JSON node);

typedef int (*JSONObjectFilter)(const char *key, JSON node);
typedef JSON (*JSONObjectMap)(const char *key, JSON node);
typedef JSON (*JSONObjectReduce)(JSON acc, const char *key, JSON node);
typedef void (*JSONObjectForeach)(const char *key, JSON node);

// Create JSON nodes
JSON JSON_null();
JSON JSON_true();
JSON JSON_false();
JSON JSON_decimal(double value);
JSON JSON_integer(int value);
JSON JSON_string(const char *value);
JSON JSON_array();
JSON JSON_object();
// Verify JSON node type
int JSON_isnull(JSON node);
int JSON_isboolean(JSON node);
int JSON_isnumber(JSON node);
int JSON_isstring(JSON node);
int JSON_isarray(JSON node);
int JSON_isobject(JSON node);
// Get JSON node values
double JSON_asdouble(JSON node);
float JSON_asfloat(JSON node);
int JSON_asint(JSON node);
long JSON_aslong(JSON node);
int JSON_asbool(JSON node);
const char *JSON_asstring(JSON node);
// JSON object functions
void JSON_object_set(JSON object, const char *key, JSON value);
void JSON_object_remove(JSON object, const char *key);
JSON JSON_object_get(JSON object, const char *key);
int JSON_object_keys(JSON object, const char **keys, int maxsize);
JSON JSON_object_filter(JSON object, JSONObjectFilter filter);
JSON JSON_object_map(JSON object, JSONObjectMap map);
JSON JSON_object_reduce(JSON object, JSONObjectReduce reduce, JSON initial);
void JSON_object_foreach(JSON object, JSONObjectForeach foreach);
// JSON array functions
void JSON_array_push(JSON array, JSON value);
void JSON_array_pop(JSON array);
void JSON_array_insert(JSON array, int index, JSON value);
void JSON_array_remove(JSON array, int index);
JSON JSON_array_get(JSON array, int index);
JSON JSON_array_set(JSON array, int index, JSON value);
int JSON_array_shift(JSON array);
JSON JSON_array_filter(JSON array, JSONArrayFilter filter);
JSON JSON_array_map(JSON array, JSONArrayMap map);
JSON JSON_array_reduce(JSON array, JSONArrayReduce reduce, JSON initial);
void JSON_array_foreach(JSON array, JSONArrayForeach foreach);
// IO Operations
JSON JSON_sread(const char *in);
JSON JSON_fread(FILE *in);
JSON JSON_read(int fd);
int JSON_fwrite(JSON node, FILE *out);
int JSON_swrite(JSON node, char *out, int maxsize);
int JSON_write(JSON node, int fd);
// Miscelaneous functions
JSON JSON_at(JSON node, const char *path);
JSON JSON_copy(JSON node);
JSON JSON_patch_diff(JSON a, JSON b);
JSON JSON_patch_apply(JSON a, JSON patch);
int JSON_cmp(JSON a, JSON b);
void JSON_free(JSON node);

#ifdef __cplusplus
}
#endif

#endif // LIBJSON_H
