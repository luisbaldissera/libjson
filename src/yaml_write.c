#include "libjson/yaml.h"
#include "json_internal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static int yaml_write_indent(FILE *out, int indent_level) {
    int bytes = 0;
    for (int i = 0; i < indent_level; i++) {
        bytes += fprintf(out, "  ");
    }
    return bytes;
}

static int yaml_write_string_escaped(FILE *out, const char *str) {
    int bytes = 0;
    bool needs_quotes = false;
    
    // Check if string needs quotes
    if (strlen(str) == 0 || 
        strchr(str, ':') != NULL || 
        strchr(str, '#') != NULL ||
        strchr(str, '\n') != NULL ||
        strchr(str, '\t') != NULL ||
        str[0] == ' ' || str[strlen(str)-1] == ' ' ||
        strcmp(str, "true") == 0 || strcmp(str, "false") == 0 || strcmp(str, "null") == 0) {
        needs_quotes = true;
    }
    
    // Check if it looks like a number
    if (!needs_quotes) {
        char *endptr;
        strtod(str, &endptr);
        if (*endptr == '\0' && endptr != str) {
            needs_quotes = true;
        }
    }
    
    if (needs_quotes) {
        bytes += fprintf(out, "\"");
        for (const char *p = str; *p; p++) {
            switch (*p) {
                case '"':
                    bytes += fprintf(out, "\\\"");
                    break;
                case '\\':
                    bytes += fprintf(out, "\\\\");
                    break;
                case '\n':
                    bytes += fprintf(out, "\\n");
                    break;
                case '\r':
                    bytes += fprintf(out, "\\r");
                    break;
                case '\t':
                    bytes += fprintf(out, "\\t");
                    break;
                default:
                    bytes += fprintf(out, "%c", *p);
                    break;
            }
        }
        bytes += fprintf(out, "\"");
    } else {
        bytes += fprintf(out, "%s", str);
    }
    
    return bytes;
}

static int yaml_write_recursive(struct json *element, FILE *out, int indent_level, bool is_array_item) {
    if (element == NULL) {
        return -1;
    }
    
    int bytes = 0;
    
    if (json_is_null(element)) {
        bytes += fprintf(out, "null");
    } else if (json_is_boolean(element)) {
        if (element == json_true()) {
            bytes += fprintf(out, "true");
        } else {
            bytes += fprintf(out, "false");
        }
    } else if (json_is_number(element)) {
        double val = json_double_value(element);
        if (val == (int)val) {
            bytes += fprintf(out, "%d", (int)val);
        } else {
            bytes += fprintf(out, "%.17g", val);
        }
    } else if (json_is_string(element)) {
        bytes += yaml_write_string_escaped(out, json_string_value(element));
    } else if (json_is_array(element)) {
        int length = json_array_length(element);
        if (length == 0) {
            bytes += fprintf(out, "[]");
        } else {
            bytes += fprintf(out, "\n");
            for (int i = 0; i < length; i++) {
                bytes += yaml_write_indent(out, indent_level + 1);
                bytes += fprintf(out, "- ");
                struct json *item = json_array_get(element, i);
                if (json_is_array(item) || json_is_object(item)) {
                    bytes += yaml_write_recursive(item, out, indent_level + 1, true);
                } else {
                    bytes += yaml_write_recursive(item, out, indent_level + 1, true);
                }
                if (i < length - 1) {
                    bytes += fprintf(out, "\n");
                }
            }
        }
    } else if (json_is_object(element)) {
        int length = json_object_length(element);
        if (length == 0) {
            bytes += fprintf(out, "{}");
        } else {
            if (!is_array_item) {
                bytes += fprintf(out, "\n");
            }
            
            struct hash_table_iter *iter = hash_table_iter_new(element->value.object);
            if (!iter) {
                return -1;
            }
            
            struct hash_table_entry *entry;
            bool first = true;
            while ((entry = hash_table_iter_next(iter))) {
                if (!first) {
                    bytes += fprintf(out, "\n");
                }
                first = false;
                
                bytes += yaml_write_indent(out, indent_level + 1);
                bytes += yaml_write_string_escaped(out, hash_table_entry_key(entry));
                bytes += fprintf(out, ": ");
                
                struct json *value = (struct json *)hash_table_entry_value(entry);
                if (json_is_array(value) || json_is_object(value)) {
                    bytes += yaml_write_recursive(value, out, indent_level + 1, false);
                } else {
                    bytes += yaml_write_recursive(value, out, indent_level + 1, false);
                }
            }
            hash_table_iter_free(iter);
        }
    }
    
    return bytes;
}

int yaml_write(struct json *element, FILE *out) {
    if (element == NULL || out == NULL) {
        return -1;
    }
    
    int bytes = yaml_write_recursive(element, out, 0, false);
    if (bytes >= 0) {
        bytes += fprintf(out, "\n");
    }
    
    return bytes;
}