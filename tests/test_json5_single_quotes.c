#include "libjson/json5.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test single quotes for strings
    struct json *single_quote = json5_read_string("'hello world'", errbuf);
    assert(single_quote != NULL);
    assert(json_is_string(single_quote));
    assert(strcmp(json_string_value(single_quote), "hello world") == 0);
    json_free(single_quote);

    return 0;
}