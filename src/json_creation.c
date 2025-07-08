#include "json_internal.h"

/**
 * @section JSON creation functions
 */

struct json *json_null()
{
    return &json_null_value;
}

struct json *json_true()
{
    return &json_true_value;
}

struct json *json_false()
{
    return &json_false_value;
}

struct json *json_number(double value)
{
    struct json *node = (struct json *)malloc(sizeof(struct json));
    if (!node)
        return NULL;

    node->type = JSON_NUMBER;
    node->value.number = value;
    return node;
}

struct json *json_string(const char *value)
{
    if (!value)
        return &json_null_value;
    struct json *node = (struct json *)malloc(sizeof(struct json));
    if (!node)
        return NULL;

    node->type = JSON_STRING;
    node->value.string = strdup(value);
    if (!node->value.string)
    {
        free(node);
        return NULL;
    }
    return node;
}

struct json *__json_array_macro(struct json *elements[])
{
    struct json *node = (struct json *)malloc(sizeof(struct json));
    if (!node)
        return NULL;

    node->type = JSON_ARRAY;
    node->value.array = NULL; // Empty array initially
    while (elements && *elements)
    {
        json_array_push(node, *elements);
        elements++;
    }
    return node;
}

struct json *__json_object_macro(struct json_key_value elements[])
{
    struct json *node = (struct json *)malloc(sizeof(struct json));
    if (!node)
        return NULL;

    node->type = JSON_OBJECT;
    node->value.object = hash_table_new();
    if (!node->value.object)
    {
        free(node);
        return NULL;
    }
    while (elements && elements->key)
    {
        struct json *value = elements->value;
        hash_table_set(node->value.object, elements->key, value);
        elements++;
    }
    return node;
}

struct json *json_copy(struct json *json)
{
    // Null and singleton static values can be returned directly
    if (!json || json == &json_null_value || json == &json_true_value || json == &json_false_value)
        return json;

    struct json *copy = (struct json *)malloc(sizeof(struct json));
    if (!copy)
        return NULL;

    copy->type = json->type;
    switch (json->type)
    {
    case JSON_NULL:
        // This case should not happen since static values are returned early
        break;
    case JSON_BOOLEAN:
        copy->value.boolean = json->value.boolean;
        break;
    case JSON_NUMBER:
        copy->value.number = json->value.number;
        break;
    case JSON_STRING:
        copy->value.string = strdup(json->value.string);
        if (!copy->value.string)
        {
            free(copy);
            return NULL;
        }
        break;
    case JSON_ARRAY:
    {
        struct linked_list_iter *ll_iter = linked_list_iter_new(json->value.array);
        if (!ll_iter)
        {
            free(copy);
            return NULL;
        }
        copy->value.array = NULL;
        struct json *element;
        while ((element = linked_list_iter_next(ll_iter)))
        {
            struct json *element_copy = json_copy(element);
            if (!element_copy)
            {
                linked_list_iter_free(ll_iter);
                linked_list_free(copy->value.array, (free_func)json_free);
                free(copy);
                return NULL;
            }
            json_array_push(copy, element_copy);
        }
        linked_list_iter_free(ll_iter);
        break;
    }
    case JSON_OBJECT:
    {
        copy->value.object = hash_table_new();
        struct hash_table_iter *ht_iter = hash_table_iter_new(json->value.object);
        if (!copy->value.object || !ht_iter)
        {
            free(copy);
            return NULL;
        }
        struct hash_table_entry *entry;
        while ((entry = hash_table_iter_next(ht_iter)))
        {
            struct json *value_copy = json_copy((struct json *)hash_table_entry_value(entry));
            if (!value_copy)
            {
                hash_table_iter_free(ht_iter);
                hash_table_free(copy->value.object, (free_func)json_free);
                free(copy);
                return NULL;
            }
            hash_table_set(copy->value.object, hash_table_entry_key(entry), value_copy);
        }
        hash_table_iter_free(ht_iter);
        break;
    }
    }
    return copy;
}

void json_free(struct json *json)
{
    if (!json || json == &json_null_value || json == &json_true_value || json == &json_false_value)
        return;

    switch (json->type)
    {
    case JSON_ARRAY:
        linked_list_free(json->value.array, (free_func)json_free);
        break;
    case JSON_OBJECT:
        hash_table_free(json->value.object, (free_func)json_free);
        break;
    case JSON_STRING:
        free(json->value.string);
        break;
    default:
        break;
    }
    free(json);
}
