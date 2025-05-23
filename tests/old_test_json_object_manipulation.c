#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libjson/json.h"

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
    test_json_object_manipulation();
    printf("All object tests passed!\n");
    return 0;
}