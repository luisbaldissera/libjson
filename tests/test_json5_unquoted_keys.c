#include "libjson/json5.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test unquoted keys in objects
    struct json *unquoted_keys = json5_read_string("{name: 'Alice', age: 30}", errbuf);
    assert(unquoted_keys != NULL);
    assert(json_is_object(unquoted_keys));
    assert(json_object_length(unquoted_keys) == 2);

    struct json *name = json_object_get(unquoted_keys, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Alice") == 0);

    struct json *age = json_object_get(unquoted_keys, "age");
    assert(json_is_number(age));
    assert(json_int_value(age) == 30);
    json_free(unquoted_keys);

    return 0;
}