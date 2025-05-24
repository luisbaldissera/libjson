#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main()
{
    // Create a JSON true value
    struct json *json_true_value = json_true();

    // Check if the value is indeed true
    assert(json_is_boolean(json_true_value));
    assert(json_true_value == json_true());

    // Free the JSON true value
    json_free(json_true_value);
    return 0;
}