#include "libjson/json.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main()
{
    struct json *valid_null = json_read_string("null", NULL);
    assert(valid_null != NULL);
    assert(!json_error(NULL));
}