#include <stdio.h>
#include <assert.h>
#include "json.h"

void test_json_creation() {
    JSON json_null = JSON_null();
    assert(JSON_isnull(json_null));

    JSON json_true = JSON_true();
    assert(JSON_isboolean(json_true));
    
    JSON json_false = JSON_false();
    assert(JSON_isboolean(json_false));

    JSON json_decimal = JSON_decimal(3.14);
    assert(JSON_isnumber(json_decimal));
    assert(json_decimal->value.decimal == 3.14);

    JSON json_integer = JSON_integer(42);
    assert(JSON_isnumber(json_integer));
    assert(json_integer->value.integer == 42);

    JSON json_string = JSON_string("Hello, World!");
    assert(JSON_isstring(json_string));

    JSON json_array = JSON_array();
    assert(JSON_isarray(json_array));

    JSON json_object = JSON_object();
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

int main() {
    test_json_creation();
    printf("All tests passed!\n");
    return 0;
}