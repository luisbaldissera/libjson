#include "libjson/json5.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test basic JSON5 parsing - should work with standard JSON first
    struct json *null_json = json5_read_string("null", errbuf);
    assert(null_json != NULL);
    assert(json_is_null(null_json));
    json_free(null_json);

    struct json *true_json = json5_read_string("true", errbuf);
    assert(true_json != NULL);
    assert(json_is_boolean(true_json));
    assert(true_json == json_true());
    json_free(true_json);

    struct json *false_json = json5_read_string("false", errbuf);
    assert(false_json != NULL);
    assert(json_is_boolean(false_json));
    assert(false_json == json_false());
    json_free(false_json);

    struct json *number_json = json5_read_string("42.5", errbuf);
    assert(number_json != NULL);
    assert(json_is_number(number_json));
    assert(json_double_value(number_json) == 42.5);
    json_free(number_json);

    struct json *string_json = json5_read_string("\"hello world\"", errbuf);
    assert(string_json != NULL);
    assert(json_is_string(string_json));
    assert(strcmp(json_string_value(string_json), "hello world") == 0);
    json_free(string_json);

    return 0;
}