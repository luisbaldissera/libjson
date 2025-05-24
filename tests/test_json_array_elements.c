#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{

    // Create a JSON array
    struct json *array = json_array(
        json_number(10),
        json_string("test"),
        json_true(),
        json_false(),
        json_null(),
        json_number(3.14));

    // Check if the array is created successfully
    assert(array != NULL);
    // Check if the array is indeed an array
    assert(json_is_array(array));
    // Check if the array has the correct number of elements
    assert(json_array_length(array) == 6);
    // Check the values of the elements in the array
    assert(json_int_value(json_array_get(array, 0)) == 10);
    assert(strcmp(json_string_value(json_array_get(array, 1)), "test") == 0);
    assert(json_is_boolean(json_array_get(array, 2)));
    assert(json_array_get(array, 2) == json_true());
    assert(json_is_boolean(json_array_get(array, 3)));
    assert(json_array_get(array, 3) == json_false());
    assert(json_is_null(json_array_get(array, 4)));
    assert(json_is_number(json_array_get(array, 5)));
    assert(json_double_value(json_array_get(array, 5)) == 3.14);

    // Free the JSON array
    json_free(array);
    return 0;
}