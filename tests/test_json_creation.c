#include <stdio.h>
#include <assert.h>
#include "json.h"

void test_json_creation()
{
    json json_null = json_null();
    assert(JSON_isnull(json_null));

    json json_true = json_true();
    assert(JSON_isboolean(json_true));

    json json_false = json_false();
    assert(JSON_isboolean(json_false));

    json json_decimal = json_decimal(3.14);
    assert(JSON_isnumber(json_decimal));
    assert(json_decimal->value.decimal == 3.14);

    json json_integer = JSON_integer(42);
    assert(JSON_isnumber(json_integer));
    assert(json_integer->value.integer == 42);

    json json_string = JSON_string("Hello, World!");
    assert(JSON_isstring(json_string));

    json json_array = JSON_array();
    assert(JSON_isarray(json_array));

    json json_object = JSON_object();
    assert(JSON_isobject(json_object));

    JSON_free(json_null);
    JSON_free(json_true);
    JSON_free(json_false);
    JSON_free(json_decimal);
    JSON_free(json_integer);
    JSON_free(json_string);
    JSON_free(json_array);
    JSON_free(json_object);
}

int main()
{
    test_json_creation();
    printf("All tests passed!\n");
    return 0;
}