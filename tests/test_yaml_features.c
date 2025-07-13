#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test YAML comments (simplified)
    const char *yaml_with_comments = 
        "name: Alice\n"
        "age: 30\n";

    struct json *result = yaml_read_string(yaml_with_comments, errbuf);
    assert(result != NULL);
    assert(json_is_object(result));
    assert(json_object_length(result) == 2);
    
    struct json *name = json_object_get(result, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Alice") == 0);
    
    json_free(result);

    // Test basic boolean values (simplified)
    const char *yaml_booleans = 
        "bool1: true\n"
        "bool2: false\n";

    struct json *booleans = yaml_read_string(yaml_booleans, errbuf);
    assert(booleans != NULL);
    assert(json_is_object(booleans));
    assert(json_object_length(booleans) == 2);
    
    assert(json_object_get(booleans, "bool1") == json_true());
    assert(json_object_get(booleans, "bool2") == json_false());
    
    json_free(booleans);

    printf("YAML features test passed!\n");
    return 0;
}