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

    // Test array with trailing comma (JSON5 feature)
    struct json *array_trailing = json5_read_string("[1, 2, 3,]", errbuf);
    assert(array_trailing != NULL);
    assert(json_is_array(array_trailing));
    assert(json_array_length(array_trailing) == 3);
    assert(json_int_value(json_array_get(array_trailing, 0)) == 1);
    assert(json_int_value(json_array_get(array_trailing, 1)) == 2);
    assert(json_int_value(json_array_get(array_trailing, 2)) == 3);
    json_free(array_trailing);

    // Test object with trailing comma (JSON5 feature)
    struct json *object_trailing = json5_read_string("{\"name\": \"Alice\", \"age\": 30,}", errbuf);
    assert(object_trailing != NULL);
    assert(json_is_object(object_trailing));
    assert(json_object_length(object_trailing) == 2);

    struct json *name = json_object_get(object_trailing, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Alice") == 0);

    struct json *age = json_object_get(object_trailing, "age");
    assert(json_is_number(age));
    assert(json_int_value(age) == 30);
    json_free(object_trailing);

    // Test debug: simple trailing comma first
    printf("Testing '[1,]'...\n");
    struct json *simple_trailing = json5_read_string("[1,]", errbuf);
    if (simple_trailing == NULL) {
        printf("Error: %s\n", errbuf);
        return 1;
    } else {
        printf("Success: parsed array with length %d\n", json_array_length(simple_trailing));
        json_free(simple_trailing);
    }

    // Test empty array (no trailing comma needed)
    printf("Testing '[]'...\n");
    struct json *empty_array = json5_read_string("[]", errbuf);
    if (empty_array == NULL) {
        printf("Error: %s\n", errbuf);
        return 1;
    } else {
        printf("Success: parsed array with length %d\n", json_array_length(empty_array));
        json_free(empty_array);
    }

    // Test empty object (no trailing comma needed)
    printf("Testing '{}'...\n");
    struct json *empty_object = json5_read_string("{}", errbuf);
    if (empty_object == NULL) {
        printf("Error: %s\n", errbuf);
        return 1;
    } else {
        printf("Success: parsed object with length %d\n", json_object_length(empty_object));
        json_free(empty_object);
    }

    return 0;
}