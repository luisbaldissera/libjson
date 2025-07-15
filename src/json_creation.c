#include "json_internal.h"

struct
{
    struct json *buffer;
    size_t size;
    size_t capacity;
    size_t next_index;
} json_memory = {
    .buffer = NULL,
    .size = 0,
    .capacity = 0,
    .next_index = 0,
};

void json_memory_init()
{
    json_memory.buffer = (struct json *)malloc(LIBJSON_INSTANCES_MIN * sizeof(struct json));
    if (!json_memory.buffer)
    {
        fprintf(stderr, "Failed to allocate initial JSON buffer\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < LIBJSON_INSTANCES_MIN; i++)
    {
        json_memory.buffer[i].type = JSON_UNSET;
        json_memory.buffer[i].self_index = i;
    }
    json_memory.size = 0;
    json_memory.capacity = LIBJSON_INSTANCES_MIN;
    json_memory.next_index = 0;
}

void json_memory_increase()
{
    size_t new_capacity = json_memory.capacity * 2;
    struct json *new_buffer = (struct json *)realloc(json_memory.buffer, new_capacity * sizeof(struct json));
    if (!new_buffer)
    {
        fprintf(stderr, "Failed to reallocate JSON buffer\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = json_memory.capacity; i < new_capacity; i++)
    {
        new_buffer[i].type = JSON_UNSET;
        new_buffer[i].self_index = i;
    }
    json_memory.buffer = new_buffer;
    json_memory.capacity = new_capacity;
}

void json_memory_shrink()
{
    if (json_memory.capacity <= LIBJSON_INSTANCES_MIN)
    {
        return; // Cannot shrink below initial size
    }
    size_t new_capacity = json_memory.capacity / 2;
    struct json *new_buffer = (struct json *)realloc(json_memory.buffer, new_capacity * sizeof(struct json));
    if (!new_buffer)
    {
        fprintf(stderr, "Failed to shrink JSON buffer\n");
        exit(EXIT_FAILURE);
    }
    json_memory.buffer = new_buffer;
    json_memory.capacity = new_capacity;
}

struct json *get_json_instance()
{
    struct json *instance;
    if (json_memory.size >= json_memory.capacity)
    {
        json_memory_increase();
        json_memory.next_index = json_memory.size;
    }
    instance = &json_memory.buffer[json_memory.next_index];
    json_memory.size++;
    while (json_memory.next_index < json_memory.capacity && json_memory.buffer[json_memory.next_index].type != JSON_UNSET)
    {
        json_memory.next_index = (json_memory.next_index + 1) % json_memory.capacity;
    }
    return instance;
}

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
    struct json *node = get_json_instance();
    node->type = JSON_NUMBER;
    node->value.number = value;
    return node;
}

struct json *json_string(const char *value)
{
    if (!value)
        return &json_null_value;
    struct json *node = get_json_instance();
    node->type = JSON_STRING;
    node->value.string = strdup(value);
    if (!node->value.string)
    {
        json_free(node);
        return NULL;
    }
    return node;
}

struct json *__json_array_macro(struct json *elements[])
{
    struct json *node = get_json_instance();
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
    struct json *node = get_json_instance();
    node->type = JSON_OBJECT;
    node->value.object = hash_table_new();
    if (!node->value.object)
    {
        json_free(node);
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

    struct json *copy = get_json_instance();

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
            json_free(copy);
            return NULL;
        }
        break;
    case JSON_ARRAY:
    {
        struct linked_list_json_iter *ll_iter = linked_list_json_iter_new(json->value.array);
        if (!ll_iter)
        {
            json_free(copy);
            return NULL;
        }
        copy->value.array = NULL;
        struct json *element;
        while ((element = linked_list_json_iter_next(ll_iter)))
        {
            struct json *element_copy = json_copy(element);
            if (!element_copy)
            {
                linked_list_json_iter_free(ll_iter);
                linked_list_json_free(copy->value.array);
                json_free(copy);
                return NULL;
            }
            json_array_push(copy, element_copy);
        }
        linked_list_json_iter_free(ll_iter);
        break;
    }
    case JSON_OBJECT:
    {
        copy->value.object = hash_table_new();
        struct hash_table_iter *ht_iter = hash_table_iter_new(json->value.object);
        if (!copy->value.object || !ht_iter)
        {
            json_free(copy);
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
    {
        // Free all JSON elements in the array first
        struct linked_list_json *current = json->value.array;
        while (current)
        {
            json_free(linked_list_json_value(current));
            current = linked_list_json_next(current);
        }
        // Then free the linked list structure
        linked_list_json_free(json->value.array);
        break;
    }
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
