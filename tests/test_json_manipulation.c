#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libjson/json.h"
#include "test_utils.h"

void test_json_array_manipulation()
{
    json array = json_array();
    json value1 = json_integer(1);
    json value2 = json_integer(2);

    json_array_push(array, value1);
    json_array_push(array, value2);

    TEST_ASSERT_EQUAL_INT(2, JSON_array_length(array));
    TEST_ASSERT_EQUAL_INT(1, JSON_integer_value(JSON_array_get(array, 0)));
    TEST_ASSERT_EQUAL_INT(2, JSON_integer_value(JSON_array_get(array, 1)));

    JSON_free(value1);
    JSON_free(value2);
    JSON_free(array);
}

void test_json_object_manipulation()
{
    json object = json_object();
    json value = json_string("test");

    json_object_set(object, "key", value);

    TEST_ASSERT_EQUAL_STRING("test", JSON_string_value(json_object_get(object, "key")));

    JSON_free(value);
    JSON_free(object);
}

int main()
{
    test_json_array_manipulation();
    test_json_object_manipulation();

    printf("All tests passed!\n");
    return 0;
}