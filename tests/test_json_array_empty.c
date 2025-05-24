#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main()
{
    // Create an empty JSON array
    struct json *array = json_array();

    // Check if the array is created successfully
    assert(array != NULL);
    // Check if the array is indeed an array
    assert(json_is_array(array));
    // Check if the array is empty
    assert(json_array_length(array) == 0);

    // Free the JSON array
    json_free(array);
    return 0;
}