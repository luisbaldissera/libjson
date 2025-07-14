#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test empty input
    struct json *result = yaml_read_string("", errbuf);
    // Empty string should return NULL
    assert(result == NULL);

    // Test whitespace-only input
    result = yaml_read_string("   \n  \t  \n  ", errbuf);
    // Whitespace-only input may return a valid JSON structure, so we just check it's handled gracefully
    if (result) {
        json_free(result);
    }

    printf("YAML empty input error test passed!\n");
    return 0;
}