#include "libjson/json.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main()
{
    char errbuf[512];

    // Test an array with missing closing bracket
    struct json *invalid_array = json_read_string("[1, 2, 3", errbuf);

    assert(invalid_array == NULL);
    assert(strstr(errbuf, "Error parsing JSON") != NULL);

    // Test an array with invalid separator
    memset(errbuf, 0, sizeof(errbuf));
    invalid_array = json_read_string("[1; 2; 3]", errbuf);

    assert(invalid_array == NULL);
    assert(strstr(errbuf, "Error parsing JSON") != NULL);

    return 0;
}
