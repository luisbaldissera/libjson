#include <stdio.h>
#include <stdlib.h>

#include "libjson/json.h"

int main()
{
    // Example 1: Create a JSON object, write to a file, then read it back
    FILE *temp_file = tmpfile();
    if (!temp_file)
    {
        perror("Failed to create temporary file");
        return 1;
    }

    // Create a sample JSON object with various data types and escaped strings
    struct json *original = json_object();
    json_object_set(original, "string", json_string("Hello,\nWorld!"));
    json_object_set(original, "number", json_number(3.14159));
    json_object_set(original, "integer", json_number(42));
    json_object_set(original, "boolean", json_true());
    json_object_set(original, "null", json_null());

    // Create a nested object
    struct json *nested = json_object();
    json_object_set(nested, "name", json_string("libjson"));
    json_object_set(nested, "version", json_string("1.0"));
    json_object_set(original, "info", nested);

    // Create an array with mixed types
    struct json *array = json_array();
    json_array_push(array, json_string("item1"));
    json_array_push(array, json_number(123));
    json_array_push(array, json_true());
    json_array_push(array, json_null());
    json_object_set(original, "items", array);

    // Write the JSON object to the temporary file
    printf("Original JSON:\n");
    json_write(original, stdout);
    printf("\n\n");
    json_write(original, temp_file);

    // Free the original JSON object
    json_free(original);

    // Reset file position to beginning for reading
    rewind(temp_file);

    // Read the JSON from the file
    struct json *parsed = json_read(temp_file);
    if (!parsed)
    {
        fprintf(stderr, "Parse error: %s\n", json_error());
        fclose(temp_file);
        return 1;
    }

    // Write the parsed JSON to stdout to verify it matches the original
    printf("Parsed JSON:\n");
    json_write(parsed, stdout);
    printf("\n");

    // Example 2: Parse a JSON string with escaped characters
    fclose(temp_file);
    temp_file = tmpfile();
    if (!temp_file)
    {
        perror("Failed to create temporary file");
        json_free(parsed);
        return 1;
    }

    // Write a JSON string with various escape sequences to the file
    const char *json_with_escapes =
        "{\"escaped_string\":\"Line 1\\nLine 2\\tTabbed\\r\\nWindows line\\u0041\","
        "\"special_chars\":\"\\\\path\\\\to\\\\file\\\\\","
        "\"quotes\":\"He said, \\\"Hello!\\\"\"}";

    fputs(json_with_escapes, temp_file);
    rewind(temp_file);

    printf("\nJSON with escaped characters:\n%s\n", json_with_escapes);

    // Parse the JSON with escapes
    struct json *escaped_json = json_read(temp_file);
    if (!escaped_json)
    {
        fprintf(stderr, "Parse error for escaped JSON: %s\n", json_error());
        json_free(parsed);
        fclose(temp_file);
        return 1;
    }

    // Print the parsed result
    printf("\nParsed JSON with escaped characters:\n");
    json_write(escaped_json, stdout);
    printf("\n");

    // Access and print individual escaped strings
    struct json *escaped_string = json_object_get(escaped_json, "escaped_string");
    printf("\nAccessing escaped_string value: %s\n", json_tostring(escaped_string));

    // Clean up
    json_free(parsed);
    json_free(escaped_json);
    fclose(temp_file);

    return 0;
}