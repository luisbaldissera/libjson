#include "libjson/json.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main()
{
    char errbuf[512];

    // Test invalid boolean token
    struct json *invalid_bool = json_read_string("trux", errbuf);
    assert(invalid_bool == NULL);
    assert(strstr(errbuf, "Error parsing JSON") != NULL);

    // Test invalid token in nested structure
    memset(errbuf, 0, sizeof(errbuf));
    struct json *invalid_nested = json_read_string("{\"key\": [1, trux, 3]}", errbuf);
    assert(invalid_nested == NULL);
    assert(strstr(errbuf, "Error parsing JSON") != NULL);

    // Test incomplete object with missing value
    memset(errbuf, 0, sizeof(errbuf));
    struct json *incomplete_object = json_read_string("{\"key\": }", errbuf);
    assert(incomplete_object == NULL);
    assert(strstr(errbuf, "Error parsing JSON") != NULL);

    // Test incomplete array with missing value after comma
    memset(errbuf, 0, sizeof(errbuf));
    struct json *incomplete_array = json_read_string("[1, ]", errbuf);
    assert(incomplete_array == NULL);
    assert(strstr(errbuf, "Error parsing JSON") != NULL);

    return 0;
}
