#include "libjson/json5.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test complex unquoted identifiers
    struct json *complex_ids = json5_read_string("{$special: 1, _underscore: 2, mixedCase123: 3}", errbuf);
    assert(complex_ids != NULL);
    assert(json_is_object(complex_ids));
    assert(json_object_length(complex_ids) == 3);
    
    assert(json_int_value(json_object_get(complex_ids, "$special")) == 1);
    assert(json_int_value(json_object_get(complex_ids, "_underscore")) == 2);
    assert(json_int_value(json_object_get(complex_ids, "mixedCase123")) == 3);
    json_free(complex_ids);

    // Test that standard JSON still works through JSON5 parser
    struct json *standard_json = json5_read_string("{\"name\": \"Alice\", \"data\": [1, 2, 3]}", errbuf);
    assert(standard_json != NULL);
    assert(json_is_object(standard_json));
    assert(json_object_length(standard_json) == 2);
    
    struct json *name = json_object_get(standard_json, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Alice") == 0);
    
    struct json *data = json_object_get(standard_json, "data");
    assert(json_is_array(data));
    assert(json_array_length(data) == 3);
    json_free(standard_json);

    return 0;
}