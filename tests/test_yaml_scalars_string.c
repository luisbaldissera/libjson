#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test unquoted string
    struct json *result = yaml_read_string("hello", errbuf);
    assert(result != NULL);
    assert(json_is_string(result));
    assert(strcmp(json_string_value(result), "hello") == 0);
    json_free(result);

    // Test quoted string
    result = yaml_read_string("\"quoted string\"", errbuf);
    assert(result != NULL);
    assert(json_is_string(result));
    assert(strcmp(json_string_value(result), "quoted string") == 0);
    json_free(result);

    // Test single-quoted string
    result = yaml_read_string("'single quoted'", errbuf);
    assert(result != NULL);
    assert(json_is_string(result));
    assert(strcmp(json_string_value(result), "single quoted") == 0);
    json_free(result);

    // Test string with spaces
    result = yaml_read_string("\"string with spaces\"", errbuf);
    assert(result != NULL);
    assert(json_is_string(result));
    assert(strcmp(json_string_value(result), "string with spaces") == 0);
    json_free(result);

    // Test empty string
    result = yaml_read_string("\"\"", errbuf);
    assert(result != NULL);
    assert(json_is_string(result));
    assert(strcmp(json_string_value(result), "") == 0);
    json_free(result);

    printf("YAML string scalars test passed!\n");
    return 0;
}