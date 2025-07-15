#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test comments at end of line
    const char *yaml_with_comments = 
        "name: John # This is a name\n"
        "age: 30   # This is an age\n";

    struct json *result = yaml_read_string(yaml_with_comments, errbuf);
    assert(result != NULL);
    assert(json_is_object(result));
    
    struct json *name = json_object_get(result, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "John") == 0);
    
    struct json *age = json_object_get(result, "age");
    assert(json_is_number(age));
    assert(json_int_value(age) == 30);
    
    json_free(result);

    // Test standalone comment lines
    const char *yaml_with_standalone_comments = 
        "# This is a standalone comment\n"
        "name: Alice\n"
        "# Another comment\n"
        "age: 25\n"
        "# Final comment\n";

    result = yaml_read_string(yaml_with_standalone_comments, errbuf);
    assert(result != NULL);
    assert(json_is_object(result));
    
    name = json_object_get(result, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Alice") == 0);
    
    age = json_object_get(result, "age");
    assert(json_is_number(age));
    assert(json_int_value(age) == 25);
    
    json_free(result);

    // Test comments in arrays
    const char *yaml_array_comments = 
        "- apple  # First fruit\n"
        "- banana # Second fruit\n"
        "# Comment between items\n"
        "- cherry # Third fruit\n";

    result = yaml_read_string(yaml_array_comments, errbuf);
    assert(result != NULL);
    assert(json_is_array(result));
    assert(json_array_length(result) == 3);
    
    struct json *item0 = json_array_get(result, 0);
    assert(json_is_string(item0));
    assert(strcmp(json_string_value(item0), "apple") == 0);
    
    struct json *item1 = json_array_get(result, 1);
    assert(json_is_string(item1));
    assert(strcmp(json_string_value(item1), "banana") == 0);
    
    struct json *item2 = json_array_get(result, 2);
    assert(json_is_string(item2));
    assert(strcmp(json_string_value(item2), "cherry") == 0);
    
    json_free(result);

    printf("YAML comments test passed!\n");
    return 0;
}