#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main()
{
    // Create a JSON false value
    struct json *json_false_value = json_false();

    // Check if the value is indeed false
    assert(json_is_boolean(json_false_value));
    assert(json_false_value == json_false());

    // Free the JSON false value
    json_free(json_false_value);
    return 0;
}