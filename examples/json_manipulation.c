#include <stdio.h>
#include "libjson/json.h"

int main()
{
    // Create a JSON object
    json jsonObject = json_object();

    // Add a string to the JSON object
    json jsonString = json_string("Hello, World!");
    json_object_set(jsonObject, "greeting", jsonString);

    // Add a number to the JSON object
    json jsonNumber = json_integer(42);
    json_object_set(jsonObject, "answer", jsonNumber);

    // Add a boolean to the JSON object
    json jsonBoolean = json_true();
    json_object_set(jsonObject, "is_answer", jsonBoolean);

    // Create a JSON array and add it to the object
    json jsonArray = json_array();
    json_array_push(jsonArray, json_integer(1));
    json_array_push(jsonArray, json_integer(2));
    json_array_push(jsonArray, json_integer(3));
    json_object_set(jsonObject, "numbers", jsonArray);

    // Write the JSON object to stdout
    json_fwrite(jsonObject, stdout);
    printf("\n");

    // Free the JSON object
    json_free(jsonObject);

    return 0;
}