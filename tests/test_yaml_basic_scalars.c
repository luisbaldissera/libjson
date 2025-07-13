#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test basic scalar values
    struct json *null_yaml = yaml_read_string("null", errbuf);
    assert(null_yaml != NULL);
    assert(json_is_null(null_yaml));
    json_free(null_yaml);

    struct json *true_yaml = yaml_read_string("true", errbuf);
    assert(true_yaml != NULL);
    assert(json_is_boolean(true_yaml));
    assert(true_yaml == json_true());
    json_free(true_yaml);

    struct json *false_yaml = yaml_read_string("false", errbuf);
    assert(false_yaml != NULL);
    assert(json_is_boolean(false_yaml));
    assert(false_yaml == json_false());
    json_free(false_yaml);

    struct json *number_yaml = yaml_read_string("42.5", errbuf);
    assert(number_yaml != NULL);
    assert(json_is_number(number_yaml));
    assert(json_double_value(number_yaml) == 42.5);
    json_free(number_yaml);

    struct json *string_yaml = yaml_read_string("hello world", errbuf);
    assert(string_yaml != NULL);
    assert(json_is_string(string_yaml));
    assert(strcmp(json_string_value(string_yaml), "hello world") == 0);
    json_free(string_yaml);

    struct json *quoted_string = yaml_read_string("\"hello world\"", errbuf);
    assert(quoted_string != NULL);
    assert(json_is_string(quoted_string));
    assert(strcmp(json_string_value(quoted_string), "hello world") == 0);
    json_free(quoted_string);

    printf("YAML basic scalars test passed!\n");
    return 0;
}