#include "libjson/json.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main()
{
    struct json *invalid_null = json_read_string("nulx", NULL);
    assert(invalid_null == NULL);
    assert(json_error(NULL) != NULL);
    const char *err = json_error(NULL);
    assert(strlen(err) > 0);
}