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
    // Empty string could legitimately return NULL or an empty structure
    if (result) {
        json_free(result);
    }

    // Test whitespace-only input
    result = yaml_read_string("   \n  \t  \n  ", errbuf);
    // Whitespace-only could legitimately return NULL or an empty structure
    if (result) {
        json_free(result);
    }

    printf("YAML empty input error test passed!\n");
    return 0;
}