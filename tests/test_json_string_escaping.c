#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libjson/json.h"

/**
 * Helper function to serialize a JSON value to a string
 */
char *json_to_string(struct json *json_value)
{
    FILE *memfile = tmpfile();
    if (!memfile)
    {
        return NULL;
    }

    json_fwrite(json_value, memfile);

    long size = ftell(memfile);
    rewind(memfile);

    char *buffer = (char *)malloc(size + 1);
    if (!buffer)
    {
        fclose(memfile);
        return NULL;
    }

    fread(buffer, 1, size, memfile);
    buffer[size] = '\0';

    fclose(memfile);
    return buffer;
}

void test_string_escaping()
{
    printf("Testing string value escaping...\n");

    // Test cases for special character escaping
    struct
    {
        const char *input;
        const char *expected;
    } test_cases[] = {
        // Basic string
        {"Hello World", "\"Hello World\""},

        // Quotes
        {"Hello \"World\"", "\"Hello \\\"World\\\"\""},

        // Backslash
        {"C:\\Program Files\\App", "\"C:\\\\Program Files\\\\App\""},

        // Control characters
        {"Line 1\nLine 2", "\"Line 1\\nLine 2\""},
        {"Tab\tCharacter", "\"Tab\\tCharacter\""},
        {"Form\fFeed", "\"Form\\fFeed\""},
        {"Carriage\rReturn", "\"Carriage\\rReturn\""},
        {"Back\bSpace", "\"Back\\bSpace\""},

        // Mixed special characters
        {"\"Quoted\", newline:\n tab:\t backslash:\\",
         "\"\\\"Quoted\\\", newline:\\n tab:\\t backslash:\\\\\""},

        // Empty string
        {"", "\"\""}};

    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    for (int i = 0; i < num_tests; i++)
    {
        struct json *json_string = json_string(test_cases[i].input);
        char *result = json_to_string(json_string);

        printf("Test %d: %s -> %s\n", i + 1, test_cases[i].input, result);
        assert(result != NULL);
        assert(strcmp(result, test_cases[i].expected) == 0);

        free(result);
        json_free(json_string);
    }

    printf("All string value escaping tests passed!\n");
}

void test_object_key_escaping()
{
    printf("Testing object key escaping...\n");

    // Test cases for object keys with special characters
    struct
    {
        const char *key;
        const char *value;
        const char *expected;
    } test_cases[] = {
        // Basic key
        {"simple", "value", "{\"simple\":\"value\"}"},

        // Key with quotes
        {"\"quoted\"", "value", "{\"\\\"quoted\\\"\":\"value\"}"},

        // Key with backslash
        {"path\\to\\file", "value", "{\"path\\\\to\\\\file\":\"value\"}"},

        // Key with control characters
        {"line1\nline2", "value", "{\"line1\\nline2\":\"value\"}"},

        // Mixed special characters in key
        {"special\"chars\\\n\t", "value",
         "{\"special\\\"chars\\\\\\n\\t\":\"value\"}"}};

    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    for (int i = 0; i < num_tests; i++)
    {
        struct json *obj = json_object();
        struct json *value = json_string(test_cases[i].value);
        json_object_set(obj, test_cases[i].key, value);

        char *result = json_to_string(obj);

        printf("Test %d: key='%s' -> %s\n", i + 1, test_cases[i].key, result);
        assert(result != NULL);
        assert(strcmp(result, test_cases[i].expected) == 0);

        free(result);
        json_free(obj); // This will free the value too
    }

    printf("All object key escaping tests passed!\n");
}

void test_control_character_escaping()
{
    printf("Testing control character escaping...\n");

    // Create a string with various control characters
    char control_chars[33];
    for (int i = 0; i < 32; i++)
    {
        control_chars[i] = (char)i;
    }
    control_chars[32] = '\0';

    struct json *json_string = json_string(control_chars);
    char *result = json_to_string(json_string);

    // Since checking all escapes would be verbose, we'll just verify
    // a few key traits of correct escaping
    assert(result != NULL);
    assert(strstr(result, "\\u0000") != NULL); // Null char escaped
    assert(strstr(result, "\\u0001") != NULL); // SOH escaped
    assert(strstr(result, "\\n") != NULL);     // Newline as \n
    assert(strstr(result, "\\t") != NULL);     // Tab as \t
    assert(strstr(result, "\\r") != NULL);     // CR as \r

    printf("Control character escaping test passed!\n");

    free(result);
    json_free(json_string);
}

int main()
{
    test_string_escaping();
    test_object_key_escaping();
    test_control_character_escaping();

    printf("All JSON string escaping tests passed!\n");
    return 0;
}