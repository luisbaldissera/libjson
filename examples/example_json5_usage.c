#include <libjson/json5.h>
#include <libjson/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // Example from the documentation: JSON5 with comments and trailing commas
    const char *json5_data =
        "{\n"
        "  // This is a comment\n"
        "  name: 'John Doe',  // Unquoted key, single quotes\n"
        "  age: 30,\n"
        "  active: true,\n"
        "  scores: [95, 87, 92,], // Trailing comma\n"
        "  /* Multi-line comment\n"
        "     can span multiple lines */\n"
        "  class: 'developer'\n"
        "}";

    char errbuf[1024];
    struct json *parsed = json5_read_string(json5_data, errbuf);

    if (!parsed)
    {
        fprintf(stderr, "JSON5 parse error: %s\n", errbuf);
        return 1;
    }

    printf("JSON5 parsing successful!\n");

    // Access data using standard JSON API
    struct json *name = json_object_get(parsed, "name");
    printf("Name: %s\n", json_string_value(name));

    struct json *class_value = json_object_get(parsed, "class");
    printf("Class: %s\n", json_string_value(class_value));

    struct json *scores = json_object_get(parsed, "scores");
    printf("Number of scores: %d\n", json_array_length(scores));

    // Write back as regular JSON to see the result
    printf("\nConverted back to JSON:\n");
    json_write(parsed, stdout);
    printf("\n");

    json_free(parsed);
    return 0;
}
