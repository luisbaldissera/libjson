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

    // Test single quotes for strings
    struct json *single_quote = json5_read_string("'hello world'", errbuf);
    assert(single_quote != NULL);
    assert(json_is_string(single_quote));
    assert(strcmp(json_string_value(single_quote), "hello world") == 0);
    json_free(single_quote);

    // Test unquoted keys in objects
    struct json *unquoted_keys = json5_read_string("{name: 'Alice', age: 30}", errbuf);
    assert(unquoted_keys != NULL);
    assert(json_is_object(unquoted_keys));
    assert(json_object_length(unquoted_keys) == 2);

    name = json_object_get(unquoted_keys, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Alice") == 0);

    age = json_object_get(unquoted_keys, "age");
    assert(json_is_number(age));
    assert(json_int_value(age) == 30);
    json_free(unquoted_keys);

    // Test single-line comments
    struct json *with_comments = json5_read_string("{\n  // This is a comment\n  name: 'Bob'\n}", errbuf);
    assert(with_comments != NULL);
    assert(json_is_object(with_comments));
    assert(json_object_length(with_comments) == 1);

    struct json *bob_name = json_object_get(with_comments, "name");
    assert(json_is_string(bob_name));
    assert(strcmp(json_string_value(bob_name), "Bob") == 0);
    json_free(with_comments);

    // Test multi-line comments
    struct json *multiline_comments = json5_read_string("{\n  /* This is a\n     multi-line comment */\n  value: 42\n}", errbuf);
    assert(multiline_comments != NULL);
    assert(json_is_object(multiline_comments));
    assert(json_object_length(multiline_comments) == 1);

    struct json *value = json_object_get(multiline_comments, "value");
    assert(json_is_number(value));
    assert(json_int_value(value) == 42);
    json_free(multiline_comments);

    // Test mixed JSON5 features
    struct json *mixed = json5_read_string("{\n  // Configuration\n  name: 'Test',\n  items: [1, 2, 3,], // trailing comma\n  'quoted-key': true,\n}", errbuf);
    assert(mixed != NULL);
    assert(json_is_object(mixed));
    assert(json_object_length(mixed) == 3);

    struct json *test_name = json_object_get(mixed, "name");
    assert(json_is_string(test_name));
    assert(strcmp(json_string_value(test_name), "Test") == 0);

    struct json *items = json_object_get(mixed, "items");
    assert(json_is_array(items));
    assert(json_array_length(items) == 3);

    struct json *quoted_key = json_object_get(mixed, "quoted-key");
    assert(quoted_key == json_true());
    json_free(mixed);

    return 0;
}