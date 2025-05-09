#include <stdio.h>
#include <libjson/json.h>

int main()
{
    const char *json_string = "{\"name\": \"John\", \"age\": 30, \"is_student\": false}";
    json json = JSON_parse(json_string);

    if (json_isnull(json))
    {
        fprintf(stderr, "Failed to parse JSON\n");
        return 1;
    }

    printf("Parsed JSON:\n");
    json_fwrite(json, stdout);
    printf("\n");

    json_free(json);
    return 0;
}