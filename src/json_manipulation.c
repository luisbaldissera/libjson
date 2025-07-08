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
        array->value.array = linked_list_new(value);
    }
    else
    {
        // Find the last node and insert after it
        struct linked_list *node = array->value.array;
        while (linked_list_next(node))
        {
            node = linked_list_next(node);
        }
        linked_list_insert(node, value);
    }
}

int json_array_length(struct json *array)
{
    if (!array || !json_is_array(array))
        return 0;

    return linked_list_length(array->value.array);
}

struct json *json_array_get(const struct json *array, int index)
{
    if (!array || !json_is_array((struct json *)array) || index < 0)
        return NULL;

    struct linked_list *node = array->value.array;
    for (int i = 0; i < index && node; i++)
    {
        node = linked_list_next(node);
    }
    return (struct json *)linked_list_value(node);
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

    // We need to use hash_table_get first to get the value before removal
    struct json *value = (struct json *)hash_table_get(object->value.object, key);

    // Custom closure to remove a key but not free its value
    struct closure *remove_closure = closure_new((closure_func)NULL, NULL);
    if (remove_closure)
    {
        hash_table_set(object->value.object, key, NULL);
        closure_free(remove_closure);
    }

    return value;
}
