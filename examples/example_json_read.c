#include <stdio.h>
#include "libjson/json.h"

int main()
{

    struct json *input_json = json_read(stdin);
    if (input_json == NULL)
    {
        fprintf(stderr, "Failed to read JSON input\n");
        return 1;
    }
    // Print the JSON object to stdout
    json_write(input_json, stdout);
    printf("\n");
    // Free the JSON object
    json_free(input_json);

    return 0;
}
