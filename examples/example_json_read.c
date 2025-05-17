#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libjson/json.h"

int main()
{

    const char *json_string = "{\"key\": \"value\", \"array\": [1, 2, 3], \"object\": {\"nested_key\": \"nested_value\"}}";
    FILE *memfile = fmemopen((void *)json_string, strlen(json_string), "r");
    if (memfile == NULL)
    {
        fprintf(stderr, "Failed to open memory file\n");
        return 1;
    }

    struct json *input_json = json_read(memfile);
    // Print the JSON object to stdout
    json_write(input_json, stdout);
    printf("\n");
    // Free the JSON object
    json_free(input_json);

    return 0;
}
