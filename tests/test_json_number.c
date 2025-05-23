#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main() {
    // Create a JSON number
    struct json *json_number_value = json_number(3.14);
    // Check if the value is indeed a number
    assert(json_isnumber(json_number_value));
    assert(json_todouble(json_number_value) == 3.14);
    assert(json_toint(json_number_value) == 3);
    // Free the JSON number value
    json_free(json_number_value);

    // Create a JSON integer
    struct json *json_integer_value = json_number(42);
    // Check if the value is indeed an integer
    assert(json_isnumber(json_integer_value));
    assert(json_toint(json_integer_value) == 42);
    assert(json_todouble(json_integer_value) == 42.0);
    // Free the JSON integer value
    json_free(json_integer_value);

    return 0;
}