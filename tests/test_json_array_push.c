#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // Create an empty array
    struct json *array = json_array();
    assert(json_is_array(array));
    assert(json_array_length(array) == 0);

    // Push elements one by one
    json_array_push(array, json_number(10));
    assert(json_array_length(array) == 1);
    assert(json_int_value(json_array_get(array, 0)) == 10);

    json_array_push(array, json_string("hello"));
    assert(json_array_length(array) == 2);
    assert(strcmp(json_string_value(json_array_get(array, 1)), "hello") == 0);

    json_array_push(array, json_true());
    assert(json_array_length(array) == 3);
    assert(json_array_get(array, 2) == json_true());

    json_array_push(array, json_null());
    assert(json_array_length(array) == 4);
    assert(json_array_get(array, 3) == json_null());

    // Push a nested array
    struct json *nested = json_array(json_number(1), json_number(2));
    json_array_push(array, nested);
    assert(json_array_length(array) == 5);
    assert(json_is_array(json_array_get(array, 4)));
    assert(json_array_length(json_array_get(array, 4)) == 2);

    // Push a nested object
    struct json *obj = json_object((struct json_key_value){"key", json_string("value")});
    json_array_push(array, obj);
    assert(json_array_length(array) == 6);
    assert(json_is_object(json_array_get(array, 5)));
    assert(json_object_length(json_array_get(array, 5)) == 1);

    json_free(array);
    return 0;
}
