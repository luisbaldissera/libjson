#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test various string creation scenarios
    struct json *simple = json_string("simple");
    assert(json_is_string(simple));
    assert(strcmp(json_string_value(simple), "simple") == 0);
    json_free(simple);

    // Test empty string
    struct json *empty = json_string("");
    assert(json_is_string(empty));
    assert(strcmp(json_string_value(empty), "") == 0);
    json_free(empty);

    // Test string with special characters
    struct json *special = json_string("Hello\nWorld\t\"Quote\"\\Backslash");
    assert(json_is_string(special));
    assert(strcmp(json_string_value(special), "Hello\nWorld\t\"Quote\"\\Backslash") == 0);
    json_free(special);

    // Test very long string
    char long_str[1000];
    for (int i = 0; i < 999; i++)
    {
        long_str[i] = 'a' + (i % 26);
    }
    long_str[999] = '\0';

    struct json *long_json = json_string(long_str);
    assert(json_is_string(long_json));
    assert(strcmp(json_string_value(long_json), long_str) == 0);
    json_free(long_json);

    // Test string escaping in JSON output/input round trip
    struct json *escape_test = json_string("Line1\nLine2\tTab\"Quote\"\\Backslash");

    // Write to temporary file and parse back
    FILE *temp = tmpfile();
    assert(temp != NULL);
    json_write(escape_test, temp);
    rewind(temp);

    struct json *parsed = json_read(temp, errbuf);
    fclose(temp);
    assert(parsed != NULL);
    assert(json_is_string(parsed));
    assert(strcmp(json_string_value(parsed), "Line1\nLine2\tTab\"Quote\"\\Backslash") == 0);

    json_free(escape_test);
    json_free(parsed);

    // Test Unicode string (if supported)
    struct json *unicode = json_string("Hello 世界 🌍");
    assert(json_is_string(unicode));
    assert(strcmp(json_string_value(unicode), "Hello 世界 🌍") == 0);
    json_free(unicode);

    // Test NULL string handling
    struct json *null_str = json_string(NULL);
    // When NULL is passed to json_string, it returns a null JSON value
    if (null_str != NULL)
    {
        assert(json_is_null(null_str)); // Should be a null value, not a string
        // Don't free since it's a static singleton
    }

    // Test string with only whitespace
    struct json *whitespace = json_string("   \t\n   ");
    assert(json_is_string(whitespace));
    assert(strcmp(json_string_value(whitespace), "   \t\n   ") == 0);
    json_free(whitespace);

    // Test string with null bytes (may not be supported)
    char null_byte_str[] = {'a', 'b', '\0', 'c', 'd', '\0'};
    struct json *null_bytes = json_string(null_byte_str);
    assert(json_is_string(null_bytes));
    // Should only contain "ab" due to null termination
    assert(strcmp(json_string_value(null_bytes), "ab") == 0);
    json_free(null_bytes);

    return 0;
}
