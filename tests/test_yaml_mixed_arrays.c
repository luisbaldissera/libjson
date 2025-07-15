#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test mixed array with different types
    const char *mixed_array = 
        "- 42\n"
        "- \"string\"\n"
        "- true\n"
        "- null\n"
        "- 3.14\n";

    struct json *result = yaml_read_string(mixed_array, errbuf);
    assert(result != NULL);
    assert(json_is_array(result));
    assert(json_array_length(result) == 5);
    
    // Check number
    struct json *item = json_array_get(result, 0);
    assert(json_is_number(item));
    assert(json_int_value(item) == 42);
    
    // Check string
    item = json_array_get(result, 1);
    assert(json_is_string(item));
    assert(strcmp(json_string_value(item), "string") == 0);
    
    // Check boolean
    item = json_array_get(result, 2);
    assert(json_is_boolean(item));
    assert(item == json_true());
    
    // Check null
    item = json_array_get(result, 3);
    assert(json_is_null(item));
    
    // Check float
    item = json_array_get(result, 4);
    assert(json_is_number(item));
    assert(json_double_value(item) >= 3.13 && json_double_value(item) <= 3.15);
    
    json_free(result);

    printf("YAML mixed arrays test passed!\n");
    return 0;
}