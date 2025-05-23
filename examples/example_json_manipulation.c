#include <stdio.h>
#include "libjson/json.h"

int main()
{
    // Create a JSON object
    struct json *jsonObject = json_object();

    // Add a string to the JSON object
    struct json *jsonString = json_string("Hello, World!");
    json_object_set(jsonObject, "greeting", jsonString);

    // Add a number to the JSON object
    struct json *jsonNumber = json_number(42);
    json_object_set(jsonObject, "answer", jsonNumber);

    // Add a boolean to the JSON object
    struct json *jsonBoolean = json_true();
    json_object_set(jsonObject, "is_answer", jsonBoolean);

    // Create a JSON array and add it to the object
    struct json *jsonArray = __json_array_macro((struct json *[]){
        json_number(1),
        json_number(2),
        json_number(3),
    });
    json_object_set(jsonObject, "numbers", jsonArray);

    // Write the JSON object to stdout
    json_write(jsonObject, stdout);
    printf("\n");

    // Free the JSON object
    json_free(jsonObject);

    return 0;
}