#include "libjson/yaml.h"
#include "libjson/json.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // Test basic scalar writing
    struct json *string_val = json_string("hello world");
    FILE *temp = tmpfile();
    assert(temp != NULL);
    
    int bytes = yaml_write(string_val, temp);
    assert(bytes > 0);
    
    rewind(temp);
    char buffer[256];
    fgets(buffer, sizeof(buffer), temp);
    fclose(temp);
    
    // Remove newline
    buffer[strcspn(buffer, "\n")] = 0;
    assert(strcmp(buffer, "hello world") == 0);
    json_free(string_val);

    // Test number writing
    struct json *number_val = json_number(42.5);
    temp = tmpfile();
    assert(temp != NULL);
    
    bytes = yaml_write(number_val, temp);
    assert(bytes > 0);
    
    rewind(temp);
    fgets(buffer, sizeof(buffer), temp);
    fclose(temp);
    
    buffer[strcspn(buffer, "\n")] = 0;
    assert(strcmp(buffer, "42.5") == 0);
    json_free(number_val);

    // Test boolean writing
    struct json *bool_val = json_true();
    temp = tmpfile();
    assert(temp != NULL);
    
    bytes = yaml_write(bool_val, temp);
    assert(bytes > 0);
    
    rewind(temp);
    fgets(buffer, sizeof(buffer), temp);
    fclose(temp);
    
    buffer[strcspn(buffer, "\n")] = 0;
    assert(strcmp(buffer, "true") == 0);
    json_free(bool_val);

    // Test null writing
    struct json *null_val = json_null();
    temp = tmpfile();
    assert(temp != NULL);
    
    bytes = yaml_write(null_val, temp);
    assert(bytes > 0);
    
    rewind(temp);
    fgets(buffer, sizeof(buffer), temp);
    fclose(temp);
    
    buffer[strcspn(buffer, "\n")] = 0;
    assert(strcmp(buffer, "null") == 0);
    json_free(null_val);

    printf("YAML write test passed!\n");
    return 0;
}