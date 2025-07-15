#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test standard true/false
    struct json *result = yaml_read_string("true", errbuf);
    assert(result != NULL);
    assert(json_is_boolean(result));
    assert(result == json_true());
    json_free(result);

    result = yaml_read_string("false", errbuf);
    assert(result != NULL);
    assert(json_is_boolean(result));
    assert(result == json_false());
    json_free(result);

    // Test YAML-specific boolean variants
    result = yaml_read_string("yes", errbuf);
    assert(result != NULL);
    assert(json_is_boolean(result));
    assert(result == json_true());
    json_free(result);

    result = yaml_read_string("no", errbuf);
    assert(result != NULL);
    assert(json_is_boolean(result));
    assert(result == json_false());
    json_free(result);

    result = yaml_read_string("on", errbuf);
    assert(result != NULL);
    assert(json_is_boolean(result));
    assert(result == json_true());
    json_free(result);

    result = yaml_read_string("off", errbuf);
    assert(result != NULL);
    assert(json_is_boolean(result));
    assert(result == json_false());
    json_free(result);

    // Test capitalized variants
    result = yaml_read_string("True", errbuf);
    assert(result != NULL);
    assert(json_is_boolean(result));
    assert(result == json_true());
    json_free(result);

    result = yaml_read_string("False", errbuf);
    assert(result != NULL);
    assert(json_is_boolean(result));
    assert(result == json_false());
    json_free(result);

    result = yaml_read_string("YES", errbuf);
    assert(result != NULL);
    assert(json_is_boolean(result));
    assert(result == json_true());
    json_free(result);

    result = yaml_read_string("NO", errbuf);
    assert(result != NULL);
    assert(json_is_boolean(result));
    assert(result == json_false());
    json_free(result);

    printf("YAML boolean scalars test passed!\n");
    return 0;
}