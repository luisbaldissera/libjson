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

    // Read the JSON from file
    struct json *input_json = json_read(memfile, NULL);
    // Print the JSON object to stdout
    json_write(input_json, stdout);
    printf("\n");
    // Free the JSON object
    json_free(input_json);
    fclose(memfile);

    // Read JSON from string directly
    struct json *json_from_string = json_read_string(json_string, NULL);
    if (json_from_string == NULL)
    {
        fprintf(stderr, "Failed to read JSON from string\n");
        return 1;
    }
    // Print the JSON object to stdout
    json_write(json_from_string, stdout);
    printf("\n");

    return 0;
}
