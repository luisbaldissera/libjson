#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char errbuf[1024];

    // Test writing a string
    struct json *string_val = json_string("Hello World");
    FILE *temp = tmpfile();
    assert(temp != NULL);
    
    int bytes = yaml_write(string_val, temp);
    assert(bytes > 0);
    
    rewind(temp);
    char buffer[256];
    fgets(buffer, sizeof(buffer), temp);
    fclose(temp);
    
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    assert(strcmp(buffer, "Hello World") == 0);
    json_free(string_val);

    // Test writing a number
    struct json *number_val = json_number(42);
    temp = tmpfile();
    assert(temp != NULL);
    
    bytes = yaml_write(number_val, temp);
    assert(bytes > 0);
    
    rewind(temp);
    fgets(buffer, sizeof(buffer), temp);
    fclose(temp);
    
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    assert(strcmp(buffer, "42") == 0);
    json_free(number_val);

    // Test writing a boolean
    struct json *bool_val = json_true();
    temp = tmpfile();
    assert(temp != NULL);
    
    bytes = yaml_write(bool_val, temp);
    assert(bytes > 0);
    
    rewind(temp);
    fgets(buffer, sizeof(buffer), temp);
    fclose(temp);
    
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    assert(strcmp(buffer, "true") == 0);
    json_free(bool_val);

    // Test writing null
    struct json *null_val = json_null();
    temp = tmpfile();
    assert(temp != NULL);
    
    bytes = yaml_write(null_val, temp);
    assert(bytes > 0);
    
    rewind(temp);
    fgets(buffer, sizeof(buffer), temp);
    fclose(temp);
    
    buffer[strcspn(buffer, "\n")] = 0; // Remove newline
    assert(strcmp(buffer, "null") == 0);
    json_free(null_val);

    // Test writing an array
    struct json *array_val = json_array(
        json_string("apple"),
        json_string("banana"),
        json_number(42)
    );
    temp = tmpfile();
    assert(temp != NULL);
    
    bytes = yaml_write(array_val, temp);
    assert(bytes > 0);
    
    rewind(temp);
    size_t total_size = 0;
    char full_buffer[1024] = "";
    while (fgets(buffer, sizeof(buffer), temp)) {
        strcat(full_buffer, buffer);
    }
    fclose(temp);
    
    // Should contain array elements
    assert(strstr(full_buffer, "apple") != NULL);
    assert(strstr(full_buffer, "banana") != NULL);
    assert(strstr(full_buffer, "42") != NULL);
    json_free(array_val);

    // Test writing an object
    struct json *object_val = json_object(
        (struct json_key_value){"name", json_string("Alice")},
        (struct json_key_value){"age", json_number(30)}
    );
    temp = tmpfile();
    assert(temp != NULL);
    
    bytes = yaml_write(object_val, temp);
    assert(bytes > 0);
    
    rewind(temp);
    full_buffer[0] = '\0';
    while (fgets(buffer, sizeof(buffer), temp)) {
        strcat(full_buffer, buffer);
    }
    fclose(temp);
    
    // Should contain object key-value pairs
    assert(strstr(full_buffer, "name") != NULL);
    assert(strstr(full_buffer, "Alice") != NULL);
    assert(strstr(full_buffer, "age") != NULL);
    assert(strstr(full_buffer, "30") != NULL);
    json_free(object_val);

    printf("YAML write comprehensive test passed!\n");
    return 0;
}