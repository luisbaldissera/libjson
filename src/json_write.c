#include "json_internal.h"

/**
 * @section JSON serialization/write functions
 */

/**
 * Helper function to write escaped strings according to JSON specification
 * Handles escaping of control characters, double quotes, and backslashes
 */
int json_write_escaped_string(const char *str, FILE *out)
{
    if (!str || !out)
        return -1;

    int bytes_written = 0;
    int ret;

    ret = fputc('"', out);
    if (ret == EOF)
        return -1;
    bytes_written++;

    for (const unsigned char *ptr = (const unsigned char *)str; *ptr; ptr++)
    {
        switch (*ptr)
        {
        case '"': // Double quote
            ret = fputs("\\\"", out);
            break;
        case '\\': // Backslash
            ret = fputs("\\\\", out);
            break;
        case '\b': // Backspace
            ret = fputs("\\b", out);
            break;
        case '\f': // Form feed
            ret = fputs("\\f", out);
            break;
        case '\n': // Newline
            ret = fputs("\\n", out);
            break;
        case '\r': // Carriage return
            ret = fputs("\\r", out);
            break;
        case '\t': // Tab
            ret = fputs("\\t", out);
            break;
        default:
            // Control characters (0-31) need special handling
            if (*ptr < 32)
            {
                // Use the \u escape sequence for control characters
                char escape_seq[8];
                sprintf(escape_seq, "\\u%04x", *ptr);
                ret = fputs(escape_seq, out);
            }
            else
            {
                // Normal character, output directly
                ret = fputc(*ptr, out);
                if (ret != EOF)
                    ret = 1; // fputc returns the character, but we want byte count
            }
            break;
        }

        if (ret < 0)
            return -1;
        bytes_written += ret;
    }

    ret = fputc('"', out);
    if (ret == EOF)
        return -1;
    bytes_written++;

    return bytes_written;
}

int json_write_null(struct json *node, FILE *out)
{
    if (!node || !out)
        return -1;

    return fprintf(out, "null");
}

int json_write_boolean(struct json *node, FILE *out)
{
    if (!node || !out)
        return -1;

    return fprintf(out, node->value.boolean ? "true" : "false");
}

int json_write_number(struct json *node, FILE *out)
{
    if (!node || !out)
        return -1;

    // Check if the number is an integer to avoid printing decimals unnecessarily
    double intpart;
    if (modf(node->value.number, &intpart) == 0.0)
    {
        return fprintf(out, "%.0f", node->value.number);
    }
    else
    {
        // Use higher precision to minimize round-trip loss
        // %.15g provides enough precision for double while avoiding
        // unnecessary trailing digits
        return fprintf(out, "%.15g", node->value.number);
    }
}

int json_write_string(struct json *node, FILE *out)
{
    if (!node || !out)
        return -1;

    return json_write_escaped_string(node->value.string, out);
}

int json_write_array(struct json *node, FILE *out)
{
    if (!node || !out)
        return -1;

    int ret, bytes_written = 0;
    struct linked_list_iter *iter = linked_list_iter_new(node->value.array);
    if (!iter)
    {
        return -1;
    }
    ret = fprintf(out, "[");
    if (ret < 0)
        return ret;
    bytes_written += ret;
    struct json *element;
    while ((element = linked_list_iter_next(iter)))
    {
        int ret = json_write(element, out);
        if (ret < 0)
        {
            linked_list_iter_free(iter);
            return ret;
        }
        bytes_written += ret;
        if (linked_list_iter_has_next(iter))
        {
            ret = fprintf(out, ",");
            if (ret < 0)
            {
                linked_list_iter_free(iter);
                return ret;
            }
            bytes_written += ret;
        }
    }
    linked_list_iter_free(iter);
    ret = fprintf(out, "]");
    if (ret < 0)
        return ret;
    bytes_written += ret;
    return bytes_written;
}

int json_write_object(struct json *node, FILE *out)
{
    if (!node || !out)
        return -1;

    int ret, bytes_written = 0;
    struct hash_table_iter *iter = hash_table_iter_new(node->value.object);
    if (!iter)
    {
        return -1;
    }
    ret = fprintf(out, "{");
    if (ret < 0)
        return ret;
    bytes_written += ret;
    struct hash_table_entry *entry;
    while ((entry = hash_table_iter_next(iter)))
    {
        // Properly escape the object key
        ret = json_write_escaped_string(hash_table_entry_key(entry), out);
        if (ret < 0)
        {
            hash_table_iter_free(iter);
            return ret;
        }
        bytes_written += ret;

        ret = fprintf(out, ":");
        if (ret < 0)
        {
            hash_table_iter_free(iter);
            return ret;
        }
        bytes_written += ret;

        ret = json_write(hash_table_entry_value(entry), out);
        if (ret < 0)
        {
            hash_table_iter_free(iter);
            return ret;
        }
        bytes_written += ret;

        if (hash_table_iter_has_next(iter))
        {
            ret = fprintf(out, ",");
            if (ret < 0)
            {
                hash_table_iter_free(iter);
                return ret;
            }
            bytes_written += ret;
        }
    }
    hash_table_iter_free(iter);
    ret = fprintf(out, "}");
    if (ret < 0)
        return ret;
    bytes_written += ret;
    return bytes_written;
}

int json_write(struct json *node, FILE *out)
{
    if (!node || !out)
        return -1;

    switch (node->type)
    {
    case JSON_NULL:
        return json_write_null(node, out);
    case JSON_BOOLEAN:
        return json_write_boolean(node, out);
    case JSON_NUMBER:
        return json_write_number(node, out);
    case JSON_STRING:
        return json_write_string(node, out);
    case JSON_ARRAY:
        return json_write_array(node, out);
    case JSON_OBJECT:
        return json_write_object(node, out);
    }
    return -1; // Should never reach here
}
