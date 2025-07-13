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

    // Test NULL inputs
    result = yaml_read_string(NULL, errbuf);
    assert(result == NULL);
    assert(strlen(errbuf) > 0);

    result = yaml_read(NULL, errbuf);
    assert(result == NULL);
    assert(strlen(errbuf) > 0);

    // Test yaml_write with NULL inputs
    int bytes = yaml_write(NULL, stdout);
    assert(bytes < 0);

    struct json *test_json = json_string("test");
    bytes = yaml_write(test_json, NULL);
    assert(bytes < 0);
    json_free(test_json);

    // Test empty input
    result = yaml_read_string("", errbuf);
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

    printf("YAML error handling test passed!\n");
    return 0;
}