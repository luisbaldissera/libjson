#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test simple array
    const char *yaml_array = 
        "- apple\n"
        "- banana\n"
        "- cherry\n";

    struct json *array = yaml_read_string(yaml_array, errbuf);
    assert(array != NULL);
    assert(json_is_array(array));
    assert(json_array_length(array) == 3);
    
    struct json *item0 = json_array_get(array, 0);
    assert(json_is_string(item0));
    assert(strcmp(json_string_value(item0), "apple") == 0);
    
    struct json *item1 = json_array_get(array, 1);
    assert(json_is_string(item1));
    assert(strcmp(json_string_value(item1), "banana") == 0);
    
    struct json *item2 = json_array_get(array, 2);
    assert(json_is_string(item2));
    assert(strcmp(json_string_value(item2), "cherry") == 0);
    
    json_free(array);

    printf("YAML array test passed!\n");
    return 0;
}