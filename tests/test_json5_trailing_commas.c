#include "libjson/json5.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

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

    return 0;
}