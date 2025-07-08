#include "json_internal.h"

/**
 * @section JSON type checking functions
 */

int json_is_null(struct json *node)
{
    return node == &json_null_value;
}

int json_is_boolean(struct json *node)
{
    return node && node->type == JSON_BOOLEAN;
}

int json_is_number(struct json *node)
{
    return node && node->type == JSON_NUMBER;
}

int json_is_string(struct json *node)
{
    return node && node->type == JSON_STRING;
}

int json_is_array(struct json *node)
{
    return node && node->type == JSON_ARRAY;
}

int json_is_object(struct json *node)
{
    return node && node->type == JSON_OBJECT;
}

/**
 * @section JSON access functions
 */

double json_double_value(const struct json *node)
{
    if (!node || node->type != JSON_NUMBER)
        return 0.0;
    return node->value.number;
}

int json_int_value(const struct json *node)
{
    if (!node || node->type != JSON_NUMBER)
        return 0;
    return (int)node->value.number;
}

const char *json_string_value(const struct json *node)
{
    if (!node || node->type != JSON_STRING)
        return NULL;
    return strdup(node->value.string);
}

const char *json_error(char *errbuf)
{
    if (!errbuf)
        errbuf = __default_errbuf;
    if (!errbuf || strlen(errbuf) == 0)
        return NULL;

    return strdup(errbuf);
}
