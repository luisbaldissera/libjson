#include <stdio.h>
#include "libjson/json.h"

int main() {
    // Create a JSON object
    JSON jsonObject = JSON_object();
    
    // Add a string to the JSON object
    JSON jsonString = JSON_string("Hello, World!");
    JSON_object_set(jsonObject, "greeting", jsonString);
    
    // Add a number to the JSON object
    JSON jsonNumber = JSON_integer(42);
    JSON_object_set(jsonObject, "answer", jsonNumber);
    
    // Add a boolean to the JSON object
    JSON jsonBoolean = JSON_true();
    JSON_object_set(jsonObject, "is_answer", jsonBoolean);
    
    // Create a JSON array and add it to the object
    JSON jsonArray = JSON_array();
    JSON_array_push(jsonArray, JSON_integer(1));
    JSON_array_push(jsonArray, JSON_integer(2));
    JSON_array_push(jsonArray, JSON_integer(3));
    JSON_object_set(jsonObject, "numbers", jsonArray);
    
    // Write the JSON object to stdout
    JSON_fwrite(jsonObject, stdout);
    printf("\n");
    
    // Free the JSON object
    JSON_free(jsonObject);
    
    return 0;
}