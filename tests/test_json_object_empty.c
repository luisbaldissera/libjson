#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main()
{
    // Create an empty JSON object
    struct json *object = json_object();

    // Check if the object is created successfully
    assert(object != NULL);

    // Check if the object is indeed an object
    assert(json_isobject(object));

    // Check if the object is empty
    assert(json_object_length(object) == 0);

    // Free the JSON object
    json_free(object);
    return 0;
}
