#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // Test yaml_write with NULL inputs
    int bytes = yaml_write(NULL, stdout);
    assert(bytes < 0);

    struct json *test_json = json_string("test");
    bytes = yaml_write(test_json, NULL);
    assert(bytes < 0);
    json_free(test_json);

    printf("YAML write NULL error test passed!\n");
    return 0;
}