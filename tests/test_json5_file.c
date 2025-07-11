#include "libjson/json5.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Create a temporary JSON5 file
    FILE *temp_file = fopen("/tmp/test.json5", "w");
    assert(temp_file != NULL);
    
    fprintf(temp_file, "{\n");
    fprintf(temp_file, "  // This is a JSON5 file\n");
    fprintf(temp_file, "  name: 'Alice',\n");
    fprintf(temp_file, "  age: 30,\n");
    fprintf(temp_file, "  hobbies: ['reading', 'coding',], // trailing comma\n");
    fprintf(temp_file, "  'quoted-key': true\n");
    fprintf(temp_file, "}\n");
    fclose(temp_file);

    // Read the file using json5_read
    temp_file = fopen("/tmp/test.json5", "r");
    assert(temp_file != NULL);
    
    struct json *result = json5_read(temp_file, errbuf);
    fclose(temp_file);
    
    assert(result != NULL);
    assert(json_is_object(result));
    assert(json_object_length(result) == 4);
    
    // Check the parsed values
    struct json *name = json_object_get(result, "name");
    assert(json_is_string(name));
    assert(strcmp(json_string_value(name), "Alice") == 0);
    
    struct json *age = json_object_get(result, "age");
    assert(json_is_number(age));
    assert(json_int_value(age) == 30);
    
    struct json *hobbies = json_object_get(result, "hobbies");
    assert(json_is_array(hobbies));
    assert(json_array_length(hobbies) == 2);
    assert(strcmp(json_string_value(json_array_get(hobbies, 0)), "reading") == 0);
    assert(strcmp(json_string_value(json_array_get(hobbies, 1)), "coding") == 0);
    
    struct json *quoted_key = json_object_get(result, "quoted-key");
    assert(quoted_key == json_true());
    
    json_free(result);
    
    // Clean up
    remove("/tmp/test.json5");
    
    return 0;
}