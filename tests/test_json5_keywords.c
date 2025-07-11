#include "libjson/json5.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test JSON5 keywords cannot be used as unquoted identifiers
    struct json *keyword_id1 = json5_read_string("{null: 'value'}", errbuf);
    assert(keyword_id1 == NULL); // null should be parsed as JSON_TOKEN_NULL, not as an identifier

    struct json *keyword_id2 = json5_read_string("{true: 'value'}", errbuf);
    assert(keyword_id2 == NULL); // true should be parsed as JSON_TOKEN_TRUE, not as an identifier

    struct json *keyword_id3 = json5_read_string("{false: 'value'}", errbuf);
    assert(keyword_id3 == NULL); // false should be parsed as JSON_TOKEN_FALSE, not as an identifier

    return 0;
}