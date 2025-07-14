#include "libjson/yaml.h"
#include "json_internal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

// YAML token types
typedef enum {
    YAML_TOKEN_EOF,
    YAML_TOKEN_SCALAR,
    YAML_TOKEN_ARRAY_ITEM,  // "-"
    YAML_TOKEN_KEY,         // key followed by ":"
    YAML_TOKEN_NEWLINE,
    YAML_TOKEN_COMMENT,
    YAML_TOKEN_ERROR
} yaml_token_type;

typedef struct {
    yaml_token_type type;
    char *value;
    int indent_level;
    bool is_quoted;
} yaml_token;

typedef struct {
    const char *input;
    size_t position;
    size_t length;
    int line;
    int column;
    char *errbuf;
} yaml_parser;

static void yaml_error(yaml_parser *parser, const char *message) {
    if (parser->errbuf) {
        snprintf(parser->errbuf, LIBJSON_ERRBUF_SiZE, 
                "YAML parse error at line %d, column %d: %s", 
                parser->line, parser->column, message);
    }
}

static char yaml_peek(yaml_parser *parser) {
    if (parser->position >= parser->length) {
        return '\0';
    }
    return parser->input[parser->position];
}

static char yaml_next(yaml_parser *parser) {
    if (parser->position >= parser->length) {
        return '\0';
    }
    char c = parser->input[parser->position++];
    if (c == '\n') {
        parser->line++;
        parser->column = 1;
    } else {
        parser->column++;
    }
    return c;
}

static void yaml_skip_whitespace(yaml_parser *parser) {
    while (yaml_peek(parser) == ' ' || yaml_peek(parser) == '\t') {
        yaml_next(parser);
    }
}

static int yaml_count_indent(yaml_parser *parser) {
    int indent = 0;
    while (yaml_peek(parser) == ' ') {
        yaml_next(parser);
        indent++;
    }
    return indent;
}

static yaml_token yaml_read_token(yaml_parser *parser) {
    yaml_token token = {.is_quoted = false};
    
    // Skip empty lines and comments
    while (true) {
        char c = yaml_peek(parser);
        if (c == '\0') {
            token.type = YAML_TOKEN_EOF;
            return token;
        }
        
        if (c == '\n') {
            yaml_next(parser);
            token.type = YAML_TOKEN_NEWLINE;
            return token;
        }
        
        if (c == '#') {
            // Skip comment line
            while (yaml_peek(parser) != '\n' && yaml_peek(parser) != '\0') {
                yaml_next(parser);
            }
            continue;
        }
        
        break;
    }
    
    // Count indentation
    token.indent_level = yaml_count_indent(parser);
    
    char c = yaml_peek(parser);
    if (c == '\0') {
        token.type = YAML_TOKEN_EOF;
        return token;
    }
    
    // Check for document separator "---"
    if (c == '-' && parser->position + 2 < parser->length && 
        parser->input[parser->position + 1] == '-' && parser->input[parser->position + 2] == '-') {
        // Check if it's a line with only "---" (possibly followed by whitespace/comment)
        size_t pos = parser->position + 3;
        while (pos < parser->length && (parser->input[pos] == ' ' || parser->input[pos] == '\t')) {
            pos++;
        }
        if (pos >= parser->length || parser->input[pos] == '\n' || parser->input[pos] == '#') {
            // This is a document separator
            parser->position = pos;
            if (pos < parser->length && parser->input[pos] == '#') {
                // Skip comment after ---
                while (pos < parser->length && parser->input[pos] != '\n') {
                    pos++;
                }
                parser->position = pos;
            }
            token.type = YAML_TOKEN_EOF; // Signal end of current document
            return token;
        }
    }
    
    if (c == '-' && (parser->position + 1 >= parser->length || parser->input[parser->position + 1] == ' ' || parser->input[parser->position + 1] == '\n')) {
        yaml_next(parser); // consume '-'
        yaml_skip_whitespace(parser);
        token.type = YAML_TOKEN_ARRAY_ITEM;
        
        // Now read the value after the array item marker
        if (yaml_peek(parser) == '\0' || yaml_peek(parser) == '\n') {
            // Empty array item
            token.value = malloc(1);
            token.value[0] = '\0';
            return token;
        }
        
        // Continue to read the scalar value that follows
    }
    
    // Read scalar/key (this handles both normal scalars and values after array markers)
    size_t start = parser->position;
    bool is_quoted = false;
    
    c = yaml_peek(parser); // Re-read current character
    
    if (c == '"' || c == '\'') {
        is_quoted = true;
        token.is_quoted = true;
        char quote_char = yaml_next(parser);
        start = parser->position; // Start after the quote
        
        while (yaml_peek(parser) != quote_char && yaml_peek(parser) != '\0') {
            if (yaml_peek(parser) == '\\') {
                yaml_next(parser); // skip escape char
                if (yaml_peek(parser) != '\0') {
                    yaml_next(parser); // skip escaped char
                }
            } else {
                yaml_next(parser);
            }
        }
        
        if (yaml_peek(parser) == quote_char) {
            size_t end = parser->position;
            yaml_next(parser); // consume closing quote
            
            token.value = malloc(end - start + 1);
            strncpy(token.value, &parser->input[start], end - start);
            token.value[end - start] = '\0';
        } else {
            yaml_error(parser, "Unterminated quoted string");
            token.type = YAML_TOKEN_ERROR;
            return token;
        }
    } else {
        // Unquoted scalar - read until colon, newline, or comment
        while (yaml_peek(parser) != '\0' && 
               yaml_peek(parser) != '\n' && 
               yaml_peek(parser) != '#' &&
               yaml_peek(parser) != ':') {
            yaml_next(parser);
        }
        
        size_t end = parser->position;
        
        // Trim trailing whitespace
        while (end > start && (parser->input[end-1] == ' ' || parser->input[end-1] == '\t')) {
            end--;
        }
        
        token.value = malloc(end - start + 1);
        strncpy(token.value, &parser->input[start], end - start);
        token.value[end - start] = '\0';
    }
    
    yaml_skip_whitespace(parser);
    
    // If we already determined this is an array item, don't change the type
    if (token.type != YAML_TOKEN_ARRAY_ITEM) {
        // Check if this is a key (followed by colon)
        if (yaml_peek(parser) == ':') {
            yaml_next(parser); // consume ':'
            yaml_skip_whitespace(parser);
            token.type = YAML_TOKEN_KEY;
        } else {
            token.type = YAML_TOKEN_SCALAR;
        }
    }
    
    return token;
}

static int yaml_strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = (*s1 >= 'A' && *s1 <= 'Z') ? *s1 + 32 : *s1;
        char c2 = (*s2 >= 'A' && *s2 <= 'Z') ? *s2 + 32 : *s2;
        if (c1 != c2) {
            return c1 - c2;
        }
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

static struct json *yaml_parse_scalar(const char *value, bool is_quoted) {
    if (!value) {
        return json_null();
    }
    
    // If quoted, always treat as string (except for empty which can still be string)
    if (is_quoted) {
        return json_string(value);
    }
    
    // Handle special values (case-insensitive) for unquoted only
    if (yaml_strcasecmp(value, "null") == 0 || strcmp(value, "~") == 0 || strcmp(value, "") == 0) {
        return json_null();
    }
    if (yaml_strcasecmp(value, "true") == 0 || yaml_strcasecmp(value, "yes") == 0 || yaml_strcasecmp(value, "on") == 0) {
        return json_true();
    }
    if (yaml_strcasecmp(value, "false") == 0 || yaml_strcasecmp(value, "no") == 0 || yaml_strcasecmp(value, "off") == 0) {
        return json_false();
    }
    
    // Try to parse as number
    char *endptr;
    double num = strtod(value, &endptr);
    if (*endptr == '\0' && endptr != value) {
        return json_number(num);
    }
    
    // Default to string
    return json_string(value);
}

static struct json *yaml_parse_value(yaml_parser *parser, int base_indent);

static struct json *yaml_parse_array(yaml_parser *parser, int base_indent) {
    struct json *array = json_array();
    
    while (true) {
        yaml_token token = yaml_read_token(parser);
        
        if (token.type == YAML_TOKEN_EOF) {
            break;
        }
        
        if (token.type == YAML_TOKEN_NEWLINE) {
            continue;
        }
        
        if (token.type == YAML_TOKEN_ARRAY_ITEM && token.indent_level == base_indent) {
            struct json *value = yaml_parse_value(parser, base_indent);
            if (value) {
                json_array_push(array, value);
            }
        } else {
            // Put token back by adjusting position - this is a simplified approach
            if (token.value) {
                free(token.value);
            }
            break;
        }
        
        if (token.value) {
            free(token.value);
        }
    }
    
    return array;
}

static struct json *yaml_parse_object(yaml_parser *parser, int base_indent) {
    struct json *object = json_object();
    
    while (true) {
        yaml_token token = yaml_read_token(parser);
        
        if (token.type == YAML_TOKEN_EOF) {
            break;
        }
        
        if (token.type == YAML_TOKEN_NEWLINE) {
            continue;
        }
        
        if (token.type == YAML_TOKEN_KEY && token.indent_level == base_indent) {
            char *key = token.value;
            struct json *value = yaml_parse_value(parser, base_indent);
            if (value) {
                json_object_set(object, key, value);
            }
            free(key);
        } else {
            if (token.value) {
                free(token.value);
            }
            break;
        }
    }
    
    return object;
}

static struct json *yaml_parse_value(yaml_parser *parser, int base_indent) {
    yaml_token token = yaml_read_token(parser);
    
    if (token.type == YAML_TOKEN_EOF) {
        return NULL;
    }
    
    if (token.type == YAML_TOKEN_NEWLINE) {
        if (token.value) free(token.value);
        return yaml_parse_value(parser, base_indent);
    }
    
    if (token.type == YAML_TOKEN_SCALAR) {
        struct json *result = yaml_parse_scalar(token.value, token.is_quoted);
        free(token.value);
        return result;
    }
    
    if (token.type == YAML_TOKEN_ARRAY_ITEM) {
        // We've found the first array item, now parse the entire array starting from this item
        struct json *array = json_array();
        
        // Add the first item we just read
        struct json *first_item = yaml_parse_scalar(token.value, token.is_quoted);
        json_array_push(array, first_item);
        free(token.value);
        
        // Continue reading more array items at the same indentation level
        while (true) {
            yaml_token next_token = yaml_read_token(parser);
            
            if (next_token.type == YAML_TOKEN_EOF) {
                break;
            }
            
            if (next_token.type == YAML_TOKEN_NEWLINE) {
                if (next_token.value) free(next_token.value);
                continue;
            }
            
            if (next_token.type == YAML_TOKEN_ARRAY_ITEM && next_token.indent_level == token.indent_level) {
                struct json *item = yaml_parse_scalar(next_token.value, next_token.is_quoted);
                json_array_push(array, item);
                free(next_token.value);
            } else {
                // This token doesn't belong to our array - we should put it back
                // For now, just break (this is a simplified approach)
                if (next_token.value) free(next_token.value);
                break;
            }
        }
        
        return array;
    }
    
    if (token.type == YAML_TOKEN_KEY) {
        // We've found the first key, now parse the entire object starting from this key
        struct json *object = json_object();
        
        // Add the first key-value pair
        char *key = token.value;
        struct json *value = yaml_parse_value(parser, base_indent);
        if (value) {
            json_object_set(object, key, value);
        }
        free(key);
        
        // Continue reading more key-value pairs at the same indentation level
        while (true) {
            yaml_token next_token = yaml_read_token(parser);
            
            if (next_token.type == YAML_TOKEN_EOF) {
                break;
            }
            
            if (next_token.type == YAML_TOKEN_NEWLINE) {
                if (next_token.value) free(next_token.value);
                continue;
            }
            
            if (next_token.type == YAML_TOKEN_KEY && next_token.indent_level == token.indent_level) {
                char *next_key = next_token.value;
                struct json *next_value = yaml_parse_value(parser, base_indent);
                if (next_value) {
                    json_object_set(object, next_key, next_value);
                }
                free(next_key);
            } else {
                // This token doesn't belong to our object
                if (next_token.value) free(next_token.value);
                break;
            }
        }
        
        return object;
    }
    
    if (token.value) {
        free(token.value);
    }
    return NULL;
}

struct json *yaml_read_string(const char *str, char *errbuf) {
    if (!str) {
        if (errbuf) {
            strcpy(errbuf, "YAML input string is NULL");
        }
        return NULL;
    }
    
    yaml_parser parser = {
        .input = str,
        .position = 0,
        .length = strlen(str),
        .line = 1,
        .column = 1,
        .errbuf = errbuf
    };
    
    return yaml_parse_value(&parser, 0);
}

struct json *yaml_read_document(FILE *in, char *errbuf) {
    if (!in) {
        if (errbuf) {
            strcpy(errbuf, "YAML input file is NULL");
        }
        return NULL;
    }
    
    // Read line by line until we find a non-empty line or document separator
    char line[4096];
    long start_pos = ftell(in);
    char *buffer = NULL;
    size_t buffer_size = 0;
    size_t buffer_len = 0;
    
    // Skip any document separators and whitespace at the beginning
    while (fgets(line, sizeof(line), in)) {
        // Trim leading and trailing whitespace
        char *trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        
        size_t len = strlen(trimmed);
        while (len > 0 && (trimmed[len-1] == '\n' || trimmed[len-1] == '\r' || 
                          trimmed[len-1] == ' ' || trimmed[len-1] == '\t')) {
            trimmed[--len] = '\0';
        }
        
        // Skip empty lines and comments
        if (len == 0 || trimmed[0] == '#') {
            continue;
        }
        
        // Check for document separator
        if (len >= 3 && strncmp(trimmed, "---", 3) == 0) {
            // Check if it's only "---" (possibly with comment)
            char *after_separator = trimmed + 3;
            while (*after_separator == ' ' || *after_separator == '\t') after_separator++;
            if (*after_separator == '\0' || *after_separator == '#') {
                continue; // Skip this separator and continue
            }
        }
        
        // This is the start of a document, seek back and break
        fseek(in, start_pos, SEEK_SET);
        break;
    }
    
    // Read the document until we hit EOF or next document separator
    while (fgets(line, sizeof(line), in)) {
        // Check for document separator
        char *trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        
        if (strlen(trimmed) >= 3 && strncmp(trimmed, "---", 3) == 0) {
            // Check if it's only "---" (possibly with comment)
            char *after_separator = trimmed + 3;
            while (*after_separator == ' ' || *after_separator == '\t') after_separator++;
            if (*after_separator == '\0' || *after_separator == '\n' || *after_separator == '#') {
                // This is a document separator, stop reading
                break;
            }
        }
        
        // Add this line to the buffer
        size_t line_len = strlen(line);
        if (buffer_len + line_len + 1 > buffer_size) {
            buffer_size = (buffer_size == 0) ? 4096 : buffer_size * 2;
            char *new_buffer = realloc(buffer, buffer_size);
            if (!new_buffer) {
                free(buffer);
                if (errbuf) {
                    strcpy(errbuf, "Failed to allocate memory for YAML document");
                }
                return NULL;
            }
            buffer = new_buffer;
        }
        
        if (buffer_len == 0) {
            strcpy(buffer, line);
        } else {
            strcat(buffer, line);
        }
        buffer_len += line_len;
        
        start_pos = ftell(in);
    }
    
    if (!buffer || buffer_len == 0) {
        free(buffer);
        return NULL; // No more documents
    }
    
    struct json *result = yaml_read_string(buffer, errbuf);
    free(buffer);
    
    return result;
}

struct json *yaml_read(FILE *in, char *errbuf) {
    if (!in) {
        if (errbuf) {
            strcpy(errbuf, "YAML input file is NULL");
        }
        return NULL;
    }
    
    // Read entire file into string
    fseek(in, 0, SEEK_END);
    long length = ftell(in);
    fseek(in, 0, SEEK_SET);
    
    char *buffer = malloc(length + 1);
    if (!buffer) {
        if (errbuf) {
            strcpy(errbuf, "Failed to allocate memory for YAML input");
        }
        return NULL;
    }
    
    size_t read_length = fread(buffer, 1, length, in);
    buffer[read_length] = '\0';
    
    struct json *result = yaml_read_string(buffer, errbuf);
    free(buffer);
    
    return result;
}