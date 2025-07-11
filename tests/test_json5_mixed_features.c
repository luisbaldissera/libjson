#include "libjson/json5.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

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