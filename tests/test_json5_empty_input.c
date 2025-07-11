#include "libjson/json5.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test empty file
    struct json *empty = json5_read_string("", errbuf);
    assert(empty == NULL);

    // Test whitespace only
    struct json *whitespace = json5_read_string("   \n\t  ", errbuf);
    assert(whitespace == NULL);

    return 0;
}