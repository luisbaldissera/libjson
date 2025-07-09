#include "json_internal.h"

#include <stdlib.h>

/**
 * JSON-specific linked list implementation
 * This provides type safety for JSON array operations
 */
struct linked_list_json
{
    struct json *value;
    struct linked_list_json *next;
};

struct linked_list_json *linked_list_json_new(struct json *value)
{
    struct linked_list_json *node = malloc(sizeof(struct linked_list_json));
    if (!node)
        return NULL;

    node->value = value;
    node->next = NULL;
    return node;
}

int linked_list_json_length(struct linked_list_json *list)
{
    int length = 0;
    while (list)
    {
        length++;
        list = list->next;
    }
    return length;
}

void linked_list_json_free(struct linked_list_json *list)
{
    while (list)
    {
        struct linked_list_json *next = list->next;
        // Note: We don't free the JSON values here - that's the caller's responsibility
        free(list);
        list = next;
    }
}

struct linked_list_json *linked_list_json_insert(struct linked_list_json *last, struct json *value)
{
    struct linked_list_json *new_node = linked_list_json_new(value);
    if (!new_node)
        return NULL;

    if (last)
    {
        new_node->next = last->next;
        last->next = new_node;
    }

    return new_node;
}

struct json *linked_list_json_value(struct linked_list_json *node)
{
    return node ? node->value : NULL;
}

struct linked_list_json *linked_list_json_next(struct linked_list_json *node)
{
    return node ? node->next : NULL;
}

struct json *linked_list_json_get(struct linked_list_json *list, int index)
{
    if (index < 0)
        return NULL;

    while (list && index > 0)
    {
        list = list->next;
        index--;
    }

    return list ? list->value : NULL;
}

/**
 * JSON-specific linked list iterator
 */
struct linked_list_json_iter
{
    struct linked_list_json *current;
};

struct linked_list_json_iter *linked_list_json_iter_new(struct linked_list_json *list)
{
    struct linked_list_json_iter *iter = malloc(sizeof(struct linked_list_json_iter));
    if (!iter)
        return NULL;

    iter->current = list;
    return iter;
}

void linked_list_json_iter_free(struct linked_list_json_iter *iter)
{
    if (iter)
        free(iter);
}

struct json *linked_list_json_iter_next(struct linked_list_json_iter *iter)
{
    if (!iter || !iter->current)
        return NULL;

    struct json *value = iter->current->value;
    iter->current = iter->current->next;
    return value;
}

int linked_list_json_iter_has_next(struct linked_list_json_iter *iter)
{
    return iter && iter->current != NULL;
}
