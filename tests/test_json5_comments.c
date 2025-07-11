#include "libjson/json5.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

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

    return 0;
}