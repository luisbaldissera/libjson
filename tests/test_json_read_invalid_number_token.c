#include "libjson/json.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main()
{
    char errbuf[512];

    // Test an invalid number format (starts with a decimal)
    struct json *invalid_number = json_read_string(".123", errbuf);

    assert(invalid_number == NULL);
    assert(strstr(errbuf, "Error parsing JSON") != NULL);

    return 0;
}
