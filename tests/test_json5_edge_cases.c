#include "libjson/json5.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test edge cases that should work

    // Test JSON5 keywords cannot be used as unquoted identifiers
    struct json *keyword_id1 = json5_read_string("{null: 'value'}", errbuf);
    assert(keyword_id1 == NULL); // null should be parsed as JSON_TOKEN_NULL, not as an identifier

    struct json *keyword_id2 = json5_read_string("{true: 'value'}", errbuf);
    assert(keyword_id2 == NULL); // true should be parsed as JSON_TOKEN_TRUE, not as an identifier

    struct json *keyword_id3 = json5_read_string("{false: 'value'}", errbuf);
    assert(keyword_id3 == NULL); // false should be parsed as JSON_TOKEN_FALSE, not as an identifier

    // Test mixed quote types
    struct json *mixed_quotes = json5_read_string("{\"double\": 'single', 'single': \"double\"}", errbuf);
    assert(mixed_quotes != NULL);
    assert(json_is_object(mixed_quotes));
    assert(json_object_length(mixed_quotes) == 2);
    
    struct json *double_val = json_object_get(mixed_quotes, "double");
    assert(json_is_string(double_val));
    assert(strcmp(json_string_value(double_val), "single") == 0);
    
    struct json *single_val = json_object_get(mixed_quotes, "single");
    assert(json_is_string(single_val));
    assert(strcmp(json_string_value(single_val), "double") == 0);
    json_free(mixed_quotes);

    // Test empty file
    struct json *empty = json5_read_string("", errbuf);
    assert(empty == NULL);

    // Test whitespace only
    struct json *whitespace = json5_read_string("   \n\t  ", errbuf);
    assert(whitespace == NULL);

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