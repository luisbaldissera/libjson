#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

void test_json_parsing()
{
    const char *json_string = "{\"key\": \"value\", \"number\": 123, \"boolean\": true}";
    json json = JSON_parse(json_string);

    if (json_isobject(json))
    {
        json value = json_object_get(json, "key");
        if (json_isstring(value) && strcmp(value->value.string, "value") == 0)
        {
            printf("Test passed: key value is correct.\n");
        }
        else
        {
            printf("Test failed: key value is incorrect.\n");
        }

        value = json_object_get(json, "number");
        if (json_isnumber(value) && value->value.integer == 123)
        {
            printf("Test passed: number value is correct.\n");
        }
        else
        {
            printf("Test failed: number value is incorrect.\n");
        }

        value = json_object_get(json, "boolean");
        if (json_isboolean(value) && value == json_true())
        {
            printf("Test passed: boolean value is correct.\n");
        }
        else
        {
            printf("Test failed: boolean value is incorrect.\n");
        }
    }
    else
    {
        printf("Test failed: JSON is not an object.\n");
    }

    JSON_free(json);
}

int main()
{
    test_json_parsing();
    return 0;
}