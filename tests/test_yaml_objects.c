#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test simple object
    const char *yaml_object = 
        "name: Alice\n"
        "age: 30\n"
        "active: true\n";

    struct json *object = yaml_read_string(yaml_object, errbuf);
    assert(object != NULL);
    assert(json_is_object(object));
    assert(json_object_length(object) == 3);
    
    struct json *name = json_object_get(object, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Alice") == 0);
    
    struct json *age = json_object_get(object, "age");
    assert(json_is_number(age));
    assert(json_int_value(age) == 30);
    
    struct json *active = json_object_get(object, "active");
    assert(json_is_boolean(active));
    assert(active == json_true());
    
    json_free(object);

    // Test object with quoted keys and values
    const char *quoted_object = 
        "\"first name\": \"John Doe\"\n"
        "\"last name\": \"Smith\"\n";

    struct json *quoted = yaml_read_string(quoted_object, errbuf);
    assert(quoted != NULL);
    assert(json_is_object(quoted));
    assert(json_object_length(quoted) == 2);
    
    struct json *first_name = json_object_get(quoted, "first name");
    assert(json_is_string(first_name));
    assert(strcmp(json_string_value(first_name), "John Doe") == 0);
    
    struct json *last_name = json_object_get(quoted, "last name");
    assert(json_is_string(last_name));
    assert(strcmp(json_string_value(last_name), "Smith") == 0);
    
    json_free(quoted);

    printf("YAML object test passed!\n");
    return 0;
}