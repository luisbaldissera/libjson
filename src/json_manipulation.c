#include "json_internal.h"

/**
 * @section JSON manipulation functions
 */

/**
 * @subsection JSON array manipulation functions
 */

void json_array_push(struct json *array, struct json *value)
{
    if (!array || !value || !json_is_array(array))
        return;

    if (!array->value.array)
    {
        // Create the first node
        array->value.array = linked_list_json_new(value);
    }
    else
    {
        // Find the last node and insert after it
        struct linked_list_json *node = array->value.array;
        while (linked_list_json_next(node))
        {
            node = linked_list_json_next(node);
        }
        linked_list_json_insert(node, value);
    }
}

int json_array_length(struct json *array)
{
    if (!array || !json_is_array(array))
        return 0;

    return linked_list_json_length(array->value.array);
}

struct json *json_array_get(const struct json *array, int index)
{
    if (!array || !json_is_array((struct json *)array) || index < 0)
        return NULL;

    return linked_list_json_get(array->value.array, index);
}

/**
 * @subsection JSON object manipulation functions
 */

void json_object_set(struct json *object, const char *key, struct json *value)
{
    if (!object || !key || !value || !json_is_object(object))
        return;

    hash_table_set(object->value.object, key, value);
}

struct json *json_object_get(const struct json *object, const char *key)
{
    if (!object || !key || !json_is_object((struct json *)object))
        return NULL;

    return (struct json *)hash_table_get(object->value.object, key);
}

int json_object_length(struct json *object)
{
    if (!object || !json_is_object(object))
        return 0;

    return hash_table_keys(object->value.object, NULL);
}

struct json *json_object_remove(struct json *object, const char *key)
{
    if (!object || !key || !json_is_object(object))
        return NULL;

    return (struct json *)hash_table_remove(object->value.object, key);
}
