#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>


int main()
{
    // Create a complex JSON structure
    struct json *json = json_object(
        (struct json_key_value){"name", json_string("Alice")},
        (struct json_key_value){"age", json_number(25.5)},
        (struct json_key_value){"active", json_true()},
        (struct json_key_value){"scores", json_array(
            json_number(95),
            json_number(87.5),
            json_number(92)
        )},
        (struct json_key_value){"address", json_object(
            (struct json_key_value){"street", json_string("123 Main St")},
            (struct json_key_value){"city", json_string("Anytown")},
            (struct json_key_value){"zip", json_number(12345)}
        )},
        (struct json_key_value){"empty_array", json_array()},
        (struct json_key_value){"null_value", json_null()}
    );

    // Create temporary file for writing
    FILE *temp = tmpfile();
    assert(temp != NULL);

    // Write JSON to file
    int bytes_written = json_write(json, temp);
    assert(bytes_written > 0);

    // Rewind file for reading
    rewind(temp);

    // Read back the JSON
    char errbuf[1024];
    struct json *parsed = json_read(temp, errbuf);
    fclose(temp);

    // Verify the parsed JSON matches original structure
    assert(parsed != NULL);
    assert(json_is_object(parsed));
    assert(json_object_length(parsed) == 7);

    // Check string value
    struct json *name = json_object_get(parsed, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Alice") == 0);

    // Check number value  
    struct json *age = json_object_get(parsed, "age");
    assert(json_is_number(age));
    assert(json_double_value(age) == 25.5);

    // Check boolean value
    struct json *active = json_object_get(parsed, "active");
    assert(json_is_boolean(active));
    assert(active == json_true());

    // Check array
    struct json *scores = json_object_get(parsed, "scores");
    assert(json_is_array(scores));
    assert(json_array_length(scores) == 3);
    assert(json_int_value(json_array_get(scores, 0)) == 95);
    assert(json_double_value(json_array_get(scores, 1)) == 87.5);
    assert(json_int_value(json_array_get(scores, 2)) == 92);

    // Check nested object
    struct json *address = json_object_get(parsed, "address");
    assert(json_is_object(address));
    assert(json_object_length(address) == 3);
    
    struct json *street = json_object_get(address, "street");
    assert(json_is_string(street));
    assert(strcmp(json_string_value(street), "123 Main St") == 0);

    struct json *city = json_object_get(address, "city");
    assert(json_is_string(city));
    assert(strcmp(json_string_value(city), "Anytown") == 0);

    struct json *zip = json_object_get(address, "zip");
    assert(json_is_number(zip));
    assert(json_int_value(zip) == 12345);

    // Check empty array
    struct json *empty_array = json_object_get(parsed, "empty_array");
    assert(json_is_array(empty_array));
    assert(json_array_length(empty_array) == 0);

    // Check null value
    struct json *null_value = json_object_get(parsed, "null_value");
    assert(json_is_null(null_value));
    assert(null_value == json_null());

    json_free(json);
    json_free(parsed);
    return 0;
}
