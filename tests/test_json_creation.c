#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "libjson/json.h"

void test_json_creation()
{
    struct json *json_null_value = json_null();
    assert(json_isnull(json_null_value));

    struct json *json_true_value = json_true();
    assert(json_isboolean(json_true_value));

    struct json *json_false_value = json_false();
    assert(json_isboolean(json_false_value));

    struct json *json_decimal_value = json_number(3.14);
    assert(json_isnumber(json_decimal_value));
    assert(json_todouble(json_decimal_value) == 3.14);

    struct json *json_integer_value = json_number(42);
    assert(json_isnumber(json_integer_value));
    assert(json_toint(json_integer_value) == 42);

    struct json *json_string_value = json_string("Hello, World!");
    assert(json_isstring(json_string_value));
    assert(strcmp(json_tostring(json_string_value), "Hello, World!") == 0);

    struct json *json_array_value = json_array();
    assert(json_isarray(json_array_value));

    struct json *json_object_value = json_object();
    assert(json_isobject(json_object_value));

    json_free(json_null_value);
    json_free(json_true_value);
    json_free(json_false_value);
    json_free(json_decimal_value);
    json_free(json_integer_value);
    json_free(json_string_value);
    json_free(json_array_value);
    json_free(json_object_value);
}

int main()
{
    test_json_creation();
    printf("All tests passed!\n");
    return 0;
}