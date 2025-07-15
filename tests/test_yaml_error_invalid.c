#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test invalid YAML input
    struct json *result = yaml_read_string("\"unterminated string", errbuf);
    assert(result == NULL);
    assert(strlen(errbuf) > 0); // Should have error message

    printf("YAML invalid syntax error test passed!\n");
    return 0;
}