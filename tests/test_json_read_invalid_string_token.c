#include "libjson/json.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main()
{
    char errbuf[512];

    // Test invalid string syntax (no quotes)
    struct json *invalid_string = json_read_string("this is not a valid json string", errbuf);

    assert(invalid_string == NULL);
    assert(strstr(errbuf, "Error parsing JSON") != NULL);

    return 0;
}
