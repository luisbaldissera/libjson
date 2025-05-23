#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main() {
    // Create a JSON null value
    struct json *json_null_value = json_null();

    // Check if the value is indeed null
    assert(json_isnull(json_null_value));

    // Free the JSON null value
    json_free(json_null_value);

    return 0;
}