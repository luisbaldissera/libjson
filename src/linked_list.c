#include "libjson/linked_list.h"

#include <stdlib.h>

struct linked_list
{
    void *value;
    struct linked_list *next;
};

struct linked_list *linked_list_new(void *value)
{
    struct linked_list *node = malloc(sizeof(struct linked_list));
    node->value = value;
    node->next = NULL;
    return node;
}

int linked_list_length(struct linked_list *list)
{
    int length = 0;
    struct linked_list *current = list;
    while (current != NULL)
    {
        length++;
        current = current->next;
    }
    return length;
}

struct linked_list *linked_list_insert(struct linked_list *last, void *value)
{
    struct linked_list *node = linked_list_new(value);
    last->next = node;
    return node;
}

struct linked_list *linked_list_next(struct linked_list *node)
{
    return node ? node->next : NULL;
}

void *linked_list_value(struct linked_list *node)
{
    return node ? node->value : NULL;
}

struct linked_list *linked_list_find(struct linked_list *node, struct closure *closure, struct linked_list **prev)
{
    if (!closure)
    {
        return NULL;
    }
    struct linked_list *current = node;
    struct linked_list *previous = NULL;
    while (current != NULL)
    {
        if (closure_invoke(closure, current->value))
        {
            if (prev)
            {
                *prev = previous;
            }
            return current;
        }
        previous = current;
        current = current->next;
    }
    return NULL;
}

void linked_list_free(struct linked_list *list, struct closure *free_value)
{
    struct linked_list *current = list, *next;
    while (current != NULL)
    {
        next = current->next;
        if (free_value)
        {
            closure_invoke(free_value, current->value);
        }
        free(current);
        current = next;
    }
}

void linked_list_foreach(struct linked_list *list, struct closure *closure)
{
    struct linked_list *current = list;
    while (current != NULL && closure != NULL)
    {
        closure_invoke(closure, current->value);
        current = current->next;
    }
}

struct linked_list *linked_list_remove(struct linked_list *node, struct closure *closure, struct linked_list **prev)
{
    if (!closure)
    {
        return NULL;
    }
    struct linked_list *current = node;
    struct linked_list *previous = NULL;
    while (current != NULL)
    {
        if (closure_invoke(closure, current->value))
        {
            if (prev)
            {
                *prev = previous;
            }
            if (previous)
            {
                previous->next = current->next;
            }
            return current;
        }
        previous = current;
        current = current->next;
    }
    return NULL;
}
