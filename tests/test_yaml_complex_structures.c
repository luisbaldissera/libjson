#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test simple nested object  
    const char *nested_object = 
        "person:\n"
        "  name: Alice\n"
        "  age: 30\n";

    struct json *result = yaml_read_string(nested_object, errbuf);
    assert(result != NULL);
    assert(json_is_object(result));
    
    struct json *person = json_object_get(result, "person");
    assert(json_is_object(person));
    
    struct json *name = json_object_get(person, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Alice") == 0);
    
    struct json *age = json_object_get(person, "age");
    assert(json_is_number(age));
    assert(json_int_value(age) == 30);
    
    json_free(result);

    printf("YAML complex structures test passed!\n");
    return 0;
}