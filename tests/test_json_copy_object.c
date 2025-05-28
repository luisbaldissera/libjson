#include "libjson/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main()
{
    // Create a JSON object
    struct json *original = json_object(
        {"name", json_string("Alice")},
        {"age", json_number(30)},
        {"is_student", json_false()},
        {"courses", json_array(
                        json_string("Math"),
                        json_string("Science"),
                        json_string("History"))});
    assert(original != NULL);
    assert(json_is_object(original));

    // Copy the JSON object
    struct json *copy = json_copy(original);
    assert(copy != NULL);
    assert(json_is_object(copy));
    assert(json_object_length(copy) == 4);
    assert(strcmp(json_string_value(json_object_get(copy, "name")), "Alice") == 0);
    assert(json_int_value(json_object_get(copy, "age")) == 30);
    assert(json_object_get(copy, "is_student") == json_false());
    assert(json_is_array(json_object_get(copy, "courses")));
    struct json *courses = json_object_get(copy, "courses");
    assert(json_array_length(courses) == 3);
    assert(strcmp(json_string_value(json_array_get(courses, 0)), "Math") == 0);
    assert(strcmp(json_string_value(json_array_get(courses, 1)), "Science") == 0);
    assert(strcmp(json_string_value(json_array_get(courses, 2)), "History") == 0);
    // Verify the original object is unchanged
    assert(json_object_length(original) == 4);
    assert(strcmp(json_string_value(json_object_get(original, "name")), "Alice") == 0);
    assert(json_int_value(json_object_get(original, "age")) == 30);
    assert(json_object_get(original, "is_student") == json_false());
    assert(json_is_array(json_object_get(original, "courses")));
    struct json *original_courses = json_object_get(original, "courses");
    assert(json_array_length(original_courses) == 3);
    assert(strcmp(json_string_value(json_array_get(original_courses, 0)), "Math") == 0);
    assert(strcmp(json_string_value(json_array_get(original_courses, 1)), "Science") == 0);
    assert(strcmp(json_string_value(json_array_get(original_courses, 2)), "History") == 0);
    // Free the original and copied JSON objects
    json_free(original);
    json_free(copy);
    return 0;
}