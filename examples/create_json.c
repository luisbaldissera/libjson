#include <stdio.h>
#include "libjson/json.h"

int main()
{
    json json_obj = json_object();

    json json_name = json_string("John Doe");
    json json_age = json_integer(30);
    json json_is_student = json_false();

    json_object_set(json_obj, "name", json_name);
    json_object_set(json_obj, "age", json_age);
    json_object_set(json_obj, "is_student", json_is_student);

    printf("Created JSON object:\n");
    json_fwrite(json_obj, stdout);
    printf("\n");

    json_free(json_obj);
    return 0;
}