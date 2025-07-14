#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test null
    struct json *result = yaml_read_string("null", errbuf);
    assert(result != NULL);
    assert(json_is_null(result));
    json_free(result);

    // Test capitalized null
    result = yaml_read_string("Null", errbuf);
    assert(result != NULL);
    assert(json_is_null(result));
    json_free(result);

    // Test uppercase null
    result = yaml_read_string("NULL", errbuf);
    assert(result != NULL);
    assert(json_is_null(result));
    json_free(result);

    // Test tilde (YAML null variant)
    result = yaml_read_string("~", errbuf);
    assert(result != NULL);
    assert(json_is_null(result));
    json_free(result);

    printf("YAML null scalars test passed!\n");
    return 0;
}