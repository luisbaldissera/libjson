#include "libjson/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main()
{
    struct json *empty_obj = json_object();
    assert(empty_obj != NULL);
    assert(json_is_object(empty_obj));
    assert(json_object_length(empty_obj) == 0);

    struct json *copy = json_copy(empty_obj);
    assert(copy != NULL);
    assert(json_is_object(copy));
    assert(json_object_length(copy) == 0);
    assert(copy != empty_obj); // Ensure it's a different object

    json_free(empty_obj);
    json_free(copy);
    return 0;
}
