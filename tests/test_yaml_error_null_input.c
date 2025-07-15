#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test NULL inputs
    struct json *result = yaml_read_string(NULL, errbuf);
    assert(result == NULL);
    assert(strlen(errbuf) > 0);

    result = yaml_read(NULL, errbuf);
    assert(result == NULL);
    assert(strlen(errbuf) > 0);

    printf("YAML NULL input error test passed!\n");
    return 0;
}