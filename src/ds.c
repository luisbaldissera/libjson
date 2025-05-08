#include "libjson/ds.h"
#include <stdlib.h>
#include <string.h>

#define LIBJSON_HT_BUCKET_SIZE (1 << 8)
#define LIBJSON_HT_KEY_MAX (1 << 8)

/////////////////////////////////////////////////////////
// Closure Implementation
/////////////////////////////////////////////////////////

struct closure
{
    closure_func func;
    void *ctx;
};

struct closure *closure_new(closure_func func, void *context)
{
    struct closure *closure = malloc(sizeof(struct closure));
    if (!closure)
        return NULL;

    closure->func = func;
    closure->ctx = context;
    return closure;
}

void closure_free(struct closure *closure)
{
    if (closure)
    {
        free(closure);
    }
}

int closure_invoke(struct closure *closure, void *value)
{
    if (!closure || !closure->func)
        return 0;

    return closure->func(value, closure->ctx);
}

/////////////////////////////////////////////////////////
// Data Structure Implementation
/////////////////////////////////////////////////////////

struct hash_table_entry
{
    char key[LIBJSON_HT_KEY_MAX];
    void *value;
};

struct hash_table
{
    struct linked_list *bucket[LIBJSON_HT_BUCKET_SIZE];
};

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

struct linked_list *linked_list_insert(struct linked_list *last, void *value)
{
    struct linked_list *node = linked_list_new(value);
    last->next = node;
    return node;
}

struct linked_list *linked_list_next(struct linked_list *node)
{
    return node->next;
}

void *linked_list_value(struct linked_list *node)
{
    return node ? node->value : NULL;
}

struct linked_list *linked_list_find(struct linked_list *node, struct closure *closure, struct linked_list **prev)
{
    if (!closure)
        return NULL;

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

void linked_list_free(struct linked_list *list)
{
    struct linked_list *current = list, *next;
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
}

void linked_list_foreach(struct linked_list *list, void (*func)(void *))
{
    struct linked_list *current = list;
    while (current != NULL)
    {
        func(current->value);
        current = current->next;
    }
}

void hash_table_init(struct hash_table *table)
{
    for (int i = 0; i < LIBJSON_HT_BUCKET_SIZE; i++)
    {
        table->bucket[i] = NULL;
    }
}

int hash_table_hash(const char *key)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *key++))
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    hash = abs(hash);
    return hash % LIBJSON_HT_BUCKET_SIZE;
}

// HT_entry helper functions
static int hash_table_key_equals_closure_func(struct hash_table_entry *entry, const char *key)
{
    if (!entry || !key)
        return 0;

    return strcmp(entry->key, key) == 0;
}

static struct closure *hash_table_key_equals(const char *key)
{
    return closure_new(hash_table_key_equals_closure_func, key);
}

struct hash_table *hash_table_new()
{
    struct hash_table *hash_table = malloc(sizeof(struct hash_table));
    hash_table_init(hash_table);
    return hash_table;
}

void hash_table_set(struct hash_table *table, const char *key, void *value)
{
    int hash = hash_table_hash(key);
    struct linked_list *ll_bucket = table->bucket[hash], *ll_prev, *ll_entry;
    struct hash_table_entry *entry;
    struct closure *key_closure;

    if (!ll_bucket)
    {
        entry = malloc(sizeof(struct hash_table_entry));
        strcpy(entry->key, key);
        entry->value = value;
        table->bucket[hash] = linked_list_new(entry);
    }
    else
    {
        key_closure = hash_table_key_equals(key);
        ll_entry = linked_list_find(ll_bucket, key_closure, &ll_prev);
        closure_free(key_closure);

        if (ll_entry)
        {
            entry = linked_list_value(ll_entry);
            entry->value = value;
        }
        else
        {
            entry = malloc(sizeof(struct hash_table_entry));
            strcpy(entry->key, key);
            entry->value = value;
            linked_list_insert(ll_prev, entry);
        }
    }
}

void *hash_table_get(struct hash_table *table, const char *key)
{
    int hash = hash_table_hash(key);
    struct linked_list *ll_bucket = table->bucket[hash], *ll_entry;
    struct hash_table_entry *entry;
    struct closure *key_closure;

    if (!ll_bucket)
        return NULL;

    key_closure = hash_table_key_equals(key);
    ll_entry = LL_find_with_closure(ll_bucket, key_closure, NULL);
    closure_free(key_closure);

    if (!ll_entry)
        return NULL;

    entry = linked_list_value(ll_entry);
    return entry->value;
}

void hash_table_foreach(struct hash_table *table, void (*func)(char *, void *))
{
    // Iterate over the hash table
    for (int i = 0; i < LIBJSON_HT_BUCKET_SIZE; i++)
    {
        struct linked_list *current = table->bucket[i];
        while (current)
        {
            struct hash_table_entry *entry = linked_list_value(current);
            func(entry->key, entry->value);
            current = linked_list_next(current);
        }
    }
}

void hash_table_free(struct hash_table *table, int free_values)
{
    // Free the hash table
    if (!table)
        return;

    for (int i = 0; i < LIBJSON_HT_BUCKET_SIZE; i++)
    {
        struct linked_list *current = table->bucket[i];
        while (current)
        {
            struct hash_table_entry *entry = linked_list_value(current);
            if (free_values && entry->value)
                free(entry->value);
            free(entry);
            current = linked_list_next(current);
        }
        linked_list_free(table->bucket[i]);
    }

    free(table);
}