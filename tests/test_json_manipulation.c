#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libjson/json.h"

void test_json_array_manipulation()
{
    struct json *array = json_array();

    // Create the values
    struct json *value1 = json_number(10);
    struct json *value2 = json_number(20);

    // Push to array (array takes ownership)
    json_array_push(array, value1);
    json_array_push(array, value2);


    // Test the array
    assert(json_array_length(array) == 2);
    assert(json_toint(json_array_get(array, 0)) == 10);
    assert(json_toint(json_array_get(array, 1)) == 20);

    // Free the array (and all its contents)
    json_free(array);
}

void test_json_object_manipulation()
{
    struct json *object = json_object();

    // Create the value
    struct json *value = json_string("test");

    // Set on object (object takes ownership)
    json_object_set(object, "key", value);


    // Test the object
    assert(json_object_get(object, "key") != NULL);
    assert(json_isstring(json_object_get(object, "key")));
    assert(json_tostring(json_object_get(object, "key")) != NULL);
    assert(strcmp(json_tostring(json_object_get(object, "key")), "test") == 0);

    // Free the object (and all its contents)
    json_free(object);
}

int main()
{
    test_json_array_manipulation();
    test_json_object_manipulation();

    printf("All tests passed!\n");
    return 0;
}