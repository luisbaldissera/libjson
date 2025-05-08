#include <stdio.h>
#include "libjson/json.h"

int main()
{
    json json_obj = JSON_object();

    json json_name = JSON_string("John Doe");
    json json_age = JSON_integer(30);
    json json_is_student = json_false();

    json_object_set(json_obj, "name", json_name);
    json_object_set(json_obj, "age", json_age);
    json_object_set(json_obj, "is_student", json_is_student);

    printf("Created JSON object:\n");
    JSON_fwrite(json_obj, stdout);
    printf("\n");

    JSON_free(json_obj);
    return 0;
}