#include <stdio.h>
#include "libjson/json.h"

int main()
{

    struct json *array = json_array(
        json_number(1),
        json_number(2),
        json_number(3),
        json_string("Hello"),
        json_true(),
        json_false(),
        json_null(),
        json_array(
            json_number(4),
            json_string("World")));

    printf("Created JSON array:\n");
    json_write(array, stdout);
    printf("\n");

    json_free(array);

    struct json *json_obj = json_object(
        {"name", json_string("Doe John")},
        {"age", json_number(30)},
        {"is_student", json_false()});

    printf("Created JSON object:\n");
    json_write(json_obj, stdout);
    printf("\n");

    json_free(json_obj);
    return 0;
}