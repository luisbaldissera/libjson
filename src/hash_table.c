#include "libjson/hash_table.h"
#include "libjson/linked_list.h"
#include "libjson/closure.h"

#include <stdlib.h>
#include <string.h>

#define LIBJSON_HASH_TABLE_BUCKET_SIZE (1 << 8)
#define LIBJSON_HASH_TABLE_KEY_MAX (1 << 8)

// Helper functions and structures

struct hash_table_entry
{
    char key[LIBJSON_HASH_TABLE_KEY_MAX];
    void *value;
};

struct hash_table
{
    struct linked_list *bucket[LIBJSON_HASH_TABLE_BUCKET_SIZE];
};

static void hash_table_init(struct hash_table *table)
{
    for (int i = 0; i < LIBJSON_HASH_TABLE_BUCKET_SIZE; i++)
    {
        table->bucket[i] = NULL;
    }
}

static int hash_table_hash(const char *key)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *key++))
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    hash = abs(hash);
    return hash % LIBJSON_HASH_TABLE_BUCKET_SIZE;
}

static int hash_table_key_equals_closure_func(struct hash_table_entry *entry, const char *key)
{
    if (!entry || !key)
        return 0;

    return strcmp(entry->key, key) == 0;
}

static struct closure *hash_table_key_equals(const char *key)
{
    return closure_new((closure_func)hash_table_key_equals_closure_func, (void *)key);
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

void *hash_table_get(const struct hash_table *table, const char *key)
{
    int hash = hash_table_hash(key);
    struct linked_list *ll_bucket = table->bucket[hash], *ll_entry;
    struct hash_table_entry *entry;
    struct closure *key_closure;

    if (!ll_bucket)
        return NULL;

    key_closure = hash_table_key_equals(key);
    ll_entry = linked_list_find(ll_bucket, key_closure, NULL);
    closure_free(key_closure);

    if (!ll_entry)
        return NULL;

    entry = linked_list_value(ll_entry);
    return entry->value;
}

void hash_table_free(struct hash_table *table, struct closure *free_value)
{
    if (!table)
    {
        return;
    }
    for (int i = 0; i < LIBJSON_HASH_TABLE_BUCKET_SIZE; i++)
    {
        struct linked_list *current = table->bucket[i];
        while (current)
        {
            struct hash_table_entry *entry = linked_list_value(current);
            if (free_value)
            {
                closure_invoke(free_value, entry->value);
            }
            free(entry);
            current = linked_list_next(current);
        }
        linked_list_free(table->bucket[i], free_value);
    }
    free(table);
}

int hash_table_keys(const struct hash_table *table, char **keys)
{
    int count = 0;
    for (int i = 0; i < LIBJSON_HASH_TABLE_BUCKET_SIZE; i++)
    {
        struct linked_list *current = table->bucket[i];
        while (current)
        {
            struct hash_table_entry *entry = linked_list_value(current);
            if (keys)
            {
                keys[count] = strdup(entry->key);
            }
            count++;
            current = linked_list_next(current);
        }
    }
    return count;
}

void hash_table_foreach(const struct hash_table *table, struct closure *closure)
{
    if (!table || !closure)
    {
        return;
    }
    for (int i = 0; i < LIBJSON_HASH_TABLE_BUCKET_SIZE; i++)
    {
        struct linked_list *current = table->bucket[i];
        while (current)
        {
            struct hash_table_entry *entry = linked_list_value(current);
            closure_invoke(closure, entry->value);
            current = linked_list_next(current);
        }
    }
}

const char *hash_table_entry_key(const struct hash_table_entry *entry)
{
    return entry ? entry->key : NULL;
}

void *hash_table_entry_value(const struct hash_table_entry *entry)
{
    return entry ? entry->value : NULL;
}