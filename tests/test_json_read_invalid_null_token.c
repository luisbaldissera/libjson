#include "libjson/json.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main()
{
    char errbuf[512];

    struct json *invalid_null = json_read_string("nulx", errbuf);

    assert(invalid_null == NULL);
    // For now, the column points to the unexpected character 'x' instead of the
    // start of the invalid token.
    assert(strcmp(errbuf, "Error parsing JSON (1:4): Invalid token: nulx") == 0);
}