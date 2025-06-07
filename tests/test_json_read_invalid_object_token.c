#include "libjson/json.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main()
{
    char errbuf[512];

    // Test an object with missing closing brace
    struct json *invalid_object = json_read_string("{\"key\": \"value\"", errbuf);

    assert(invalid_object == NULL);
    assert(strstr(errbuf, "Error parsing JSON") != NULL);

    // Test an object with invalid key-value format
    memset(errbuf, 0, sizeof(errbuf));
    invalid_object = json_read_string("{key: \"value\"}", errbuf);

    assert(invalid_object == NULL);
    assert(strstr(errbuf, "Error parsing JSON") != NULL);

    return 0;
}
