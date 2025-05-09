#include <stdio.h>
#include <assert.h>
#include "json.h"

void test_json_creation()
{
    json json_null = json_null();
    assert(json_isnull(json_null));

    json json_true = json_true();
    assert(json_isboolean(json_true));

    json json_false = json_false();
    assert(json_isboolean(json_false));

    json json_decimal = json_decimal(3.14);
    assert(json_isnumber(json_decimal));
    assert(json_decimal->value.decimal == 3.14);

    json json_integer = json_integer(42);
    assert(json_isnumber(json_integer));
    assert(json_integer->value.integer == 42);

    json json_string = json_string("Hello, World!");
    assert(json_isstring(json_string));

    json json_array = json_array();
    assert(json_isarray(json_array));

    json json_object = json_object();
    assert(json_isobject(json_object));

    json_free(json_null);
    json_free(json_true);
    json_free(json_false);
    json_free(json_decimal);
    json_free(json_integer);
    json_free(json_string);
    json_free(json_array);
    json_free(json_object);
}

int main()
{
    test_json_creation();
    printf("All tests passed!\n");
    return 0;
}