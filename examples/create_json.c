#include <stdio.h>
#include "libjson/json.h"

int main() {
    JSON json_obj = JSON_object();
    
    JSON json_name = JSON_string("John Doe");
    JSON json_age = JSON_integer(30);
    JSON json_is_student = JSON_false();
    
    JSON_object_set(json_obj, "name", json_name);
    JSON_object_set(json_obj, "age", json_age);
    JSON_object_set(json_obj, "is_student", json_is_student);
    
    printf("Created JSON object:\n");
    JSON_fwrite(json_obj, stdout);
    printf("\n");
    
    JSON_free(json_obj);
    return 0;
}