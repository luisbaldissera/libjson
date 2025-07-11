#include "json_internal.h"
#include "libjson/json5.h"

/**
 * @section JSON5 deserialization/read functions
 */

struct json *json5_read(FILE *in, char *errbuf)
{
    struct json_token token;
    char linecol[64];
    if (!in)
        return NULL;
    if (!errbuf)
        errbuf = __default_errbuf;
    struct error_context errctx = {
        .message = errbuf,
        .line = 0,
        .column = 0};
    token = json5_read_token(in, &errctx);
    struct json *result = NULL;
    if (token.type != JSON_TOKEN_INVALID && json5_parser_json(in, &token, &result, &errctx))
    {
        // Successfully parsed JSON5
        errbuf[0] = '\0';
        return result;
    }
    int initial_message_length = strlen(errctx.message);
    sprintf(linecol, "(%d:%d): ", errctx.line + 1, errctx.column);
    strprep(errctx.message, linecol);
    strprep(errctx.message, "Error parsing JSON5 ");
    if (result)
    {
        json_free(result);
        result = NULL;
    }
    return result;
}

struct json *json5_read_string(const char *json5_string, char *errbuf)
{
    if (!json5_string)
        return NULL;

    FILE *memfile = fmemopen((void *)json5_string, strlen(json5_string), "r");
    if (!memfile)
        return NULL;

    struct json *result = json5_read(memfile, errbuf);
    fclose(memfile);
    return result;
}

// JSON5 tokenizer - extends JSON tokenizer to support JSON5 features
struct json_token json5_read_token(FILE *in, struct error_context *errctx)
{
    struct json_token token = {0};
    int c, i = 0;
    const char *error = "Invalid JSON5 token: ";
    int error_length = strlen(error);

    // Skip whitespace and comments
    while (1) {
        while (isspace(c = update_error_context(errctx, fgetc(in), i)))
            ;
        
        // Check for comments
        if (c == '/') {
            int next_c = update_error_context(errctx, fgetc(in), i + 1);
            if (next_c == '/') {
                // Single-line comment - skip until end of line
                i += 2;
                while ((c = update_error_context(errctx, fgetc(in), i++)) != '\n' && c != EOF)
                    ;
                if (c == '\n') {
                    continue; // Skip whitespace again after comment
                }
            } else if (next_c == '*') {
                // Multi-line comment - skip until */
                i += 2;
                while (1) {
                    c = update_error_context(errctx, fgetc(in), i++);
                    if (c == EOF) {
                        token.type = JSON_TOKEN_INVALID;
                        strcpy(errctx->message, "Unterminated comment");
                        return token;
                    }
                    if (c == '*') {
                        int end_c = update_error_context(errctx, fgetc(in), i++);
                        if (end_c == '/') {
                            break; // End of comment
                        } else {
                            ungetc(end_c, in);
                            i--;
                        }
                    }
                }
                continue; // Skip whitespace again after comment
            } else {
                // Not a comment, put back the next character
                ungetc(next_c, in);
                break;
            }
        } else {
            break; // Not whitespace or comment
        }
    }

    if (c == EOF)
    {
        token.type = JSON_TOKEN_EOF;
        return token;
    }

    // Check for JSON5 unquoted identifiers first (before keywords)
    if (isalpha(c) || c == '_' || c == '$')
    {
        ungetc(c, in);
        char buffer[256];
        int j = 0;
        i = 0; // Because of ungetc, we need to reset i
        while (j < sizeof(buffer) - 1)
        {
            c = update_error_context(errctx, fgetc(in), i++);
            if (isalnum(c) || c == '_' || c == '$')
            {
                buffer[j++] = c;
            }
            else
            {
                break;
            }
        }
        if (c != EOF)
        {
            ungetc(c, in);
            i--; // Adjust index for ungetc
        }
        buffer[j] = '\0';
        
        // Check if this is a JSON keyword
        if (strcmp(buffer, "null") == 0)
        {
            token.type = JSON_TOKEN_NULL;
        }
        else if (strcmp(buffer, "true") == 0)
        {
            token.type = JSON_TOKEN_TRUE;
        }
        else if (strcmp(buffer, "false") == 0)
        {
            token.type = JSON_TOKEN_FALSE;
        }
        else if (j > 0)
        {
            // It's an unquoted identifier
            token.value = strdup(buffer);
            token.type = JSON_TOKEN_STRING; // Treat unquoted identifiers as strings
        }
        else
        {
            token.type = JSON_TOKEN_INVALID;
        }
        return token;
    }

    switch (c)
    {
    case 'n':
    case 't':
    case 'f':
        // These are now handled above in the identifier logic
        token.type = JSON_TOKEN_INVALID;
        break;
    case '[':
    {
        token.type = JSON_TOKEN_ARRAY_START;
        break;
    }
    case ']':
    {
        token.type = JSON_TOKEN_ARRAY_END;
        break;
    }
    case '{':
    {
        token.type = JSON_TOKEN_OBJECT_START;
        break;
    }
    case '}':
    {
        token.type = JSON_TOKEN_OBJECT_END;
        break;
    }
    case ',':
    {
        token.type = JSON_TOKEN_COMMA;
        break;
    }
    case ':':
    {
        token.type = JSON_TOKEN_COLON;
        break;
    }
    case '"':
    case '\'': // JSON5 supports single quotes
    {
        char quote_char = c;
        char *str = NULL;
        struct linked_list *char_list = NULL, *char_list_ptr = NULL;
        i = 1;
        while ((c = update_error_context(errctx, fgetc(in), i++)) != quote_char)
        {
            if (c == EOF)
            {
                token.type = JSON_TOKEN_INVALID;
                strcpy(errctx->message, "Unterminated string");
                if (char_list)
                    linked_list_free(char_list, NULL);
                return token;
            }
            if (c == '\\')
            {
                c = update_error_context(errctx, fgetc(in), i++);
                switch (c)
                {
                case 'b':
                    c = '\b';
                    break;
                case 'f':
                    c = '\f';
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 't':
                    c = '\t';
                    break;
                case '\\':
                    c = '\\';
                    break;
                case '"':
                    c = '"';
                    break;
                case '\'':
                    c = '\'';
                    break;
                case 'u':
                {
                    // Unicode escape sequence \uXXXX
                    char hex[5] = {0};
                    for (int j = 0; j < 4; j++)
                    {
                        int hex_char = update_error_context(errctx, fgetc(in), i++);
                        if (isxdigit(hex_char))
                        {
                            hex[j] = hex_char;
                        }
                        else
                        {
                            token.type = JSON_TOKEN_INVALID;
                            if (char_list)
                                linked_list_free(char_list, NULL);
                            return token;
                        }
                    }
                    c = (int)strtol(hex, NULL, 16);
                    break;
                }
                default: // Error: Invalid escape sequence
                    token.type = JSON_TOKEN_INVALID;
                    if (char_list)
                        linked_list_free(char_list, NULL);
                    return token;
                }
            }
            if (char_list)
            {
                char_list_ptr = linked_list_insert(char_list_ptr, (void *)(long)c);
            }
            else
            {
                char_list = linked_list_new((void *)(long)c);
                char_list_ptr = char_list;
            }
        }
        if (char_list)
        {
            int length = linked_list_length(char_list);
            str = (char *)malloc(length + 1);
            if (str)
            {
                struct linked_list *node = char_list;
                for (int j = 0; j < length; j++)
                {
                    str[j] = (char)(long)linked_list_value(node);
                    node = linked_list_next(node);
                }
                str[length] = '\0';
                token.value = str;
                token.type = JSON_TOKEN_STRING;
            }
            linked_list_free(char_list, NULL);
        }
        else
        {
            // Empty string
            str = (char *)malloc(1);
            if (str)
            {
                str[0] = '\0';
                token.value = str;
                token.type = JSON_TOKEN_STRING;
            }
            else
            {
                token.type = JSON_TOKEN_INVALID;
            }
        }
        break;
    }
    default: // Number or INVALID
    {
        // Check if this could be a valid number (must start with digit or minus)
        if (isdigit(c) || c == '-')
        {
            ungetc(c, in);
            char buffer[32];
            int j = 0;
            i = 0; // Because of ungetc, we need to reset i
            while (isdigit(c = update_error_context(errctx, fgetc(in), i++)) || c == '.' || c == '-' || c == '+' || c == 'e' || c == 'E')
            {
                if (j < sizeof(buffer) - 1)
                {
                    buffer[j++] = c;
                }
                else
                {
                    // Buffer overflow
                    token.type = JSON_TOKEN_INVALID;
                    return token;
                }
            }
            if (c != EOF)
            {
                ungetc(c, in);
                i--; // Adjust index for ungetc
            }
            buffer[j] = '\0';
            if (j > 0)
            {
                token.value = strdup(buffer);
                token.type = JSON_TOKEN_NUMBER;
            }
            else
            {
                token.type = JSON_TOKEN_INVALID;
            }
        }
        else
        {
            token.type = JSON_TOKEN_INVALID;
        }
        break;
    }
    }
    if (token.type == JSON_TOKEN_INVALID)
    {
        errctx->message[i] = '\0';
        strprep(errctx->message, error);
    }
    return token;
}

// JSON5 parser - reuses JSON parser logic but with JSON5 tokenizer
int json5_parser_json(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx)
{
    return json5_parser_literal(in, token, dest, errctx) || 
           json5_parser_array(in, token, dest, errctx) || 
           json5_parser_object(in, token, dest, errctx);
}

int json5_parser_literal(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx)
{
    // For now, delegate to the standard JSON literal parser
    return json_parser_literal(in, token, dest, errctx);
}

int json5_parser_array(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx)
{
    if (token->type == JSON_TOKEN_ARRAY_START)
    {
        *dest = __json_array_macro(NULL); // Create empty array
        *token = json5_read_token(in, errctx);
        struct json *element = NULL;
        if (json5_parser_json(in, token, &element, errctx))
        {
            json_array_push(*dest, element);
            *token = json5_read_token(in, errctx);
            while (token->type == JSON_TOKEN_COMMA)
            {
                *token = json5_read_token(in, errctx);
                // JSON5 allows trailing commas - check for array end
                if (token->type == JSON_TOKEN_ARRAY_END)
                {
                    return 1; // Trailing comma is allowed
                }
                if (json5_parser_json(in, token, &element, errctx))
                {
                    json_array_push(*dest, element);
                    *token = json5_read_token(in, errctx);
                }
                else // Error: Unexpected inner JSON
                {
                    strcpy(errctx->message, "Expected JSON5 value after comma in array.");
                    json_free(*dest);
                    *dest = NULL;
                    return 0;
                }
            }
        }
        if (token->type == JSON_TOKEN_ARRAY_END)
        {
            return 1;
        }
        else
        {
            strcpy(errctx->message, "Expecting ']' or ','.");
            json_free(*dest);
            *dest = NULL;
            return 0;
        }
    }
    // Not an array
    return 0;
}

int json5_parser_object(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx)
{
    if (token->type == JSON_TOKEN_OBJECT_START)
    {
        *dest = __json_object_macro(NULL); // Create empty object
        *token = json5_read_token(in, errctx);

        // Check for empty object first
        if (token->type == JSON_TOKEN_OBJECT_END)
        {
            return 1; // Empty object is valid
        }

        if (json5_parser_key_value(in, token, *dest, errctx))
        {
            *token = json5_read_token(in, errctx);
            while (token->type == JSON_TOKEN_COMMA)
            {
                *token = json5_read_token(in, errctx);
                // JSON5 allows trailing commas - check for object end
                if (token->type == JSON_TOKEN_OBJECT_END)
                {
                    return 1; // Trailing comma is allowed
                }
                if (json5_parser_key_value(in, token, *dest, errctx))
                {
                    *token = json5_read_token(in, errctx);
                }
                else
                {
                    strcpy(errctx->message, "Expected key-value pair after comma in object.");
                    json_free(*dest);
                    *dest = NULL;
                    return 0;
                }
            }

            if (token->type == JSON_TOKEN_OBJECT_END)
            {
                return 1;
            }
            else
            {
                strcpy(errctx->message, "Expecting '}' or ','.");
                json_free(*dest);
                *dest = NULL;
                return 0;
            }
        }
        else
        {
            strcpy(errctx->message, "Expected key-value pair in object.");
            json_free(*dest);
            *dest = NULL;
            return 0;
        }
    }
    // Not an object
    return 0;
}

int json5_parser_key_value(FILE *in, struct json_token *token, struct json *object, struct error_context *errctx)
{
    if (token->type == JSON_TOKEN_STRING)
    {
        char *key = strdup(token->value);
        *token = json5_read_token(in, errctx);
        if (token->type == JSON_TOKEN_COLON)
        {
            *token = json5_read_token(in, errctx);
            struct json *value = NULL;
            if (json5_parser_json(in, token, &value, errctx))
            {
                json_object_set(object, key, value);
                free(key);
                return 1;
            }
            else
            {
                strcpy(errctx->message, "Expected JSON5 value after ':' in object.");
                free(key);
                return 0;
            }
        }
        else
        {
            strcpy(errctx->message, "Expecting ':' after key.");
            free(key);
            return 0;
        }
    }
    return 0;
}