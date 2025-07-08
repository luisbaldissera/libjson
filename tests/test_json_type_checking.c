#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    struct json *null_val = json_null();
    struct json *true_val = json_true();
    struct json *false_val = json_false();
    struct json *number_val = json_number(42.5);
    struct json *string_val = json_string("hello");
    struct json *array_val = json_array(json_number(1), json_number(2));
    struct json *object_val = json_object((struct json_key_value){"key", json_string("value")});

    // Test json_is_null
    assert(json_is_null(null_val) != 0);
    assert(json_is_null(true_val) == 0);
    assert(json_is_null(false_val) == 0);
    assert(json_is_null(number_val) == 0);
    assert(json_is_null(string_val) == 0);
    assert(json_is_null(array_val) == 0);
    assert(json_is_null(object_val) == 0);

    // Test json_is_boolean
    assert(json_is_boolean(null_val) == 0);
    assert(json_is_boolean(true_val) != 0);
    assert(json_is_boolean(false_val) != 0);
    assert(json_is_boolean(number_val) == 0);
    assert(json_is_boolean(string_val) == 0);
    assert(json_is_boolean(array_val) == 0);
    assert(json_is_boolean(object_val) == 0);

    // Test json_is_number
    assert(json_is_number(null_val) == 0);
    assert(json_is_number(true_val) == 0);
    assert(json_is_number(false_val) == 0);
    assert(json_is_number(number_val) != 0);
    assert(json_is_number(string_val) == 0);
    assert(json_is_number(array_val) == 0);
    assert(json_is_number(object_val) == 0);

    // Test json_is_string
    assert(json_is_string(null_val) == 0);
    assert(json_is_string(true_val) == 0);
    assert(json_is_string(false_val) == 0);
    assert(json_is_string(number_val) == 0);
    assert(json_is_string(string_val) != 0);
    assert(json_is_string(array_val) == 0);
    assert(json_is_string(object_val) == 0);

    // Test json_is_array
    assert(json_is_array(null_val) == 0);
    assert(json_is_array(true_val) == 0);
    assert(json_is_array(false_val) == 0);
    assert(json_is_array(number_val) == 0);
    assert(json_is_array(string_val) == 0);
    assert(json_is_array(array_val) != 0);
    assert(json_is_array(object_val) == 0);

    // Test json_is_object
    assert(json_is_object(null_val) == 0);
    assert(json_is_object(true_val) == 0);
    assert(json_is_object(false_val) == 0);
    assert(json_is_object(number_val) == 0);
    assert(json_is_object(string_val) == 0);
    assert(json_is_object(array_val) == 0);
    assert(json_is_object(object_val) != 0);

    // Test with NULL pointer (should handle gracefully)
    assert(json_is_null(NULL) == 0);
    assert(json_is_boolean(NULL) == 0);
    assert(json_is_number(NULL) == 0);
    assert(json_is_string(NULL) == 0);
    assert(json_is_array(NULL) == 0);
    assert(json_is_object(NULL) == 0);

    json_free(number_val);
    json_free(string_val);
    json_free(array_val);
    json_free(object_val);
    return 0;
}
