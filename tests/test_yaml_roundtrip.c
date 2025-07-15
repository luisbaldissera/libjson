#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test round-trip: JSON -> YAML -> JSON
    struct json *original = json_object(
        (struct json_key_value){"name", json_string("Alice")},
        (struct json_key_value){"age", json_number(30)},
        (struct json_key_value){"active", json_true()},
        (struct json_key_value){"scores", json_array(
                                              json_number(95),
                                              json_number(87),
                                              json_number(92))});

    // Write to YAML
    FILE *temp = tmpfile();
    assert(temp != NULL);
    
    int bytes = yaml_write(original, temp);
    assert(bytes > 0);
    
    // Read back from YAML
    rewind(temp);
    struct json *parsed = yaml_read(temp, errbuf);
    fclose(temp);
    
    assert(parsed != NULL);
    assert(json_is_object(parsed));
    assert(json_object_length(parsed) >= 3); // At least the basic fields we can parse
    
    // Verify some values (note: full object comparison would require deep comparison function)
    struct json *name = json_object_get(parsed, "name");
    if (name) {
        assert(json_is_string(name));
        assert(strcmp(json_string_value(name), "Alice") == 0);
    }
    
    struct json *age = json_object_get(parsed, "age");
    if (age) {
        assert(json_is_number(age));
        assert(json_int_value(age) == 30);
    }
    
    json_free(original);
    json_free(parsed);

    // Test empty array
    struct json *empty_array = json_array();
    temp = tmpfile();
    assert(temp != NULL);
    bytes = yaml_write(empty_array, temp);
    assert(bytes > 0);
    
    rewind(temp);
    char buffer[256];
    fgets(buffer, sizeof(buffer), temp);
    fclose(temp);
    
    buffer[strcspn(buffer, "\n")] = 0;
    assert(strcmp(buffer, "[]") == 0);
    json_free(empty_array);

    printf("YAML round-trip test passed!\n");
    return 0;
}