#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // Create a JSON object with multiple key-value pairs
    struct json *object = json_object(
        (struct json_key_value){"name", json_string("John")},
        (struct json_key_value){"age", json_number(30)},
        (struct json_key_value){"active", json_true()}
    );

    // Verify initial state
    assert(json_is_object(object));
    assert(json_object_length(object) == 3);
    assert(json_object_get(object, "name") != NULL);
    assert(json_object_get(object, "age") != NULL);
    assert(json_object_get(object, "active") != NULL);

    // Remove existing key
    struct json *removed = json_object_remove(object, "age");
    assert(removed != NULL);
    assert(json_is_number(removed));
    assert(json_int_value(removed) == 30);
    assert(json_object_length(object) == 2);
    assert(json_object_get(object, "age") == NULL);
    json_free(removed);

    // Try to remove non-existing key
    struct json *not_found = json_object_remove(object, "nonexistent");
    assert(not_found == NULL);
    assert(json_object_length(object) == 2);

    // Remove another key
    removed = json_object_remove(object, "name");
    assert(removed != NULL);
    assert(json_is_string(removed));
    assert(strcmp(json_string_value(removed), "John") == 0);
    assert(json_object_length(object) == 1);
    json_free(removed);

    // Remove last key
    removed = json_object_remove(object, "active");
    assert(removed != NULL);
    assert(json_is_boolean(removed));
    assert(removed == json_true());
    assert(json_object_length(object) == 0);
    json_free(removed);

    json_free(object);
    return 0;
}
