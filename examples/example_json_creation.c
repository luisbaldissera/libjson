#include <stdio.h>
#include "libjson/json.h"

int main()
{

    struct json *array = json_array();
    json_array_push(array, json_string("Hello"));
    json_array_push(array, json_number(42));
    json_array_push(array, json_true());
    json_array_push(array, json_false());
    json_array_push(array, json_null());
    struct json *nested_array = json_array();
    json_array_push(nested_array, json_string("Nested"));
    json_array_push(array, nested_array);

    printf("Created JSON array:\n");
    json_write(array, stdout);
    printf("\n");

    json_free(array);

    struct json *json_obj = json_object();

    struct json *json_name = json_string("John Doe");
    struct json *json_age = json_number(30);
    struct json *json_is_student = json_false();

    json_object_set(json_obj, "name", json_name);
    json_object_set(json_obj, "age", json_age);
    json_object_set(json_obj, "is_student", json_is_student);

    printf("Created JSON object:\n");
    json_write(json_obj, stdout);
    printf("\n");

    json_free(json_obj);
    return 0;
}