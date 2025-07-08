#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test successful parsing of various JSON types
    struct json *null_json = json_read_string("null", errbuf);
    assert(null_json != NULL);
    assert(json_is_null(null_json));
    json_free(null_json);

    struct json *true_json = json_read_string("true", errbuf);
    assert(true_json != NULL);
    assert(json_is_boolean(true_json));
    assert(true_json == json_true());
    json_free(true_json);

    struct json *false_json = json_read_string("false", errbuf);
    assert(false_json != NULL);
    assert(json_is_boolean(false_json));
    assert(false_json == json_false());
    json_free(false_json);

    struct json *number_json = json_read_string("42.5", errbuf);
    assert(number_json != NULL);
    assert(json_is_number(number_json));
    assert(json_double_value(number_json) == 42.5);
    json_free(number_json);

    struct json *negative_json = json_read_string("-123", errbuf);
    assert(negative_json != NULL);
    assert(json_is_number(negative_json));
    assert(json_int_value(negative_json) == -123);
    json_free(negative_json);

    struct json *string_json = json_read_string("\"hello world\"", errbuf);
    assert(string_json != NULL);
    assert(json_is_string(string_json));
    assert(strcmp(json_string_value(string_json), "hello world") == 0);
    json_free(string_json);

    struct json *empty_string_json = json_read_string("\"\"", errbuf);
    assert(empty_string_json != NULL);
    assert(json_is_string(empty_string_json));
    assert(strcmp(json_string_value(empty_string_json), "") == 0);
    json_free(empty_string_json);

    // Test string with escape sequences
    struct json *escaped_json = json_read_string("\"\\\"hello\\\"\\n\"", errbuf);
    assert(escaped_json != NULL);
    assert(json_is_string(escaped_json));
    assert(strcmp(json_string_value(escaped_json), "\"hello\"\n") == 0);
    json_free(escaped_json);

    // Test array parsing
    struct json *array_json = json_read_string("[1, 2, 3]", errbuf);
    assert(array_json != NULL);
    assert(json_is_array(array_json));
    assert(json_array_length(array_json) == 3);
    assert(json_int_value(json_array_get(array_json, 0)) == 1);
    assert(json_int_value(json_array_get(array_json, 1)) == 2);
    assert(json_int_value(json_array_get(array_json, 2)) == 3);
    json_free(array_json);

    // Test empty array
    struct json *empty_array_json = json_read_string("[]", errbuf);
    assert(empty_array_json != NULL);
    assert(json_is_array(empty_array_json));
    assert(json_array_length(empty_array_json) == 0);
    json_free(empty_array_json);

    // Test mixed array
    struct json *mixed_array = json_read_string("[null, true, 42, \"test\"]", errbuf);
    assert(mixed_array != NULL);
    assert(json_is_array(mixed_array));
    assert(json_array_length(mixed_array) == 4);
    assert(json_is_null(json_array_get(mixed_array, 0)));
    assert(json_array_get(mixed_array, 1) == json_true());
    assert(json_int_value(json_array_get(mixed_array, 2)) == 42);
    assert(strcmp(json_string_value(json_array_get(mixed_array, 3)), "test") == 0);
    json_free(mixed_array);

    // Test object parsing
    struct json *object_json = json_read_string("{\"name\": \"Alice\", \"age\": 30}", errbuf);
    assert(object_json != NULL);
    assert(json_is_object(object_json));
    assert(json_object_length(object_json) == 2);

    struct json *name = json_object_get(object_json, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Alice") == 0);

    struct json *age = json_object_get(object_json, "age");
    assert(json_is_number(age));
    assert(json_int_value(age) == 30);
    json_free(object_json);

    // Test empty object
    struct json *empty_object = json_read_string("{}", errbuf);
    assert(empty_object != NULL);
    assert(json_is_object(empty_object));
    assert(json_object_length(empty_object) == 0);
    json_free(empty_object);

    // Test nested structures
    struct json *nested = json_read_string("{\"data\": [1, {\"inner\": true}]}", errbuf);
    assert(nested != NULL);
    assert(json_is_object(nested));

    struct json *data = json_object_get(nested, "data");
    assert(json_is_array(data));
    assert(json_array_length(data) == 2);
    assert(json_int_value(json_array_get(data, 0)) == 1);

    struct json *inner_obj = json_array_get(data, 1);
    assert(json_is_object(inner_obj));
    struct json *inner_val = json_object_get(inner_obj, "inner");
    assert(inner_val == json_true());
    json_free(nested);

    // Test whitespace handling
    struct json *whitespace = json_read_string("  {  \"key\"  :  \"value\"  }  ", errbuf);
    assert(whitespace != NULL);
    assert(json_is_object(whitespace));
    assert(strcmp(json_string_value(json_object_get(whitespace, "key")), "value") == 0);
    json_free(whitespace);

    return 0;
}
