#include "libjson/json5.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

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

    return 0;
}