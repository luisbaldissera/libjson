#include "libjson/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main()
{
    struct json *empty_arr = json_array();
    assert(empty_arr != NULL);
    assert(json_is_array(empty_arr));
    assert(json_array_length(empty_arr) == 0);
    struct json *copy = json_copy(empty_arr);
    assert(copy != NULL);
    assert(json_is_array(copy));
    assert(json_array_length(copy) == 0);
    assert(copy != empty_arr); // Ensure it's a different object
    json_free(empty_arr);
    json_free(copy);
    return 0;
}