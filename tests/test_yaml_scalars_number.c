#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test positive integer
    struct json *result = yaml_read_string("42", errbuf);
    assert(result != NULL);
    assert(json_is_number(result));
    assert(json_int_value(result) == 42);
    json_free(result);

    // Test negative integer
    result = yaml_read_string("-17", errbuf);
    assert(result != NULL);
    assert(json_is_number(result));
    assert(json_int_value(result) == -17);
    json_free(result);

    // Test zero
    result = yaml_read_string("0", errbuf);
    assert(result != NULL);
    assert(json_is_number(result));
    assert(json_int_value(result) == 0);
    json_free(result);

    // Test floating point
    result = yaml_read_string("3.14", errbuf);
    assert(result != NULL);
    assert(json_is_number(result));
    assert(json_double_value(result) >= 3.13 && json_double_value(result) <= 3.15);
    json_free(result);

    // Test negative floating point
    result = yaml_read_string("-2.5", errbuf);
    assert(result != NULL);
    assert(json_is_number(result));
    assert(json_double_value(result) >= -2.51 && json_double_value(result) <= -2.49);
    json_free(result);

    // Test scientific notation
    result = yaml_read_string("1.23e4", errbuf);
    assert(result != NULL);
    assert(json_is_number(result));
    assert(json_double_value(result) >= 12299 && json_double_value(result) <= 12301);
    json_free(result);

    printf("YAML number scalars test passed!\n");
    return 0;
}