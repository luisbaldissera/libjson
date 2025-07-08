#include "json_internal.h"

/**
 * @section JSON deserialization/read functions
 */

struct json *json_read(FILE *in, char *errbuf)
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
    token = json_read_token(in, &errctx);
    struct json *result = NULL;
    if (token.type != JSON_TOKEN_INVALID && json_parser_json(in, &token, &result, &errctx))
    {
        // Successfully parsed JSON
        errbuf[0] = '\0';
        return result;
    }
    int initial_message_length = strlen(errctx.message);
    sprintf(linecol, "(%d:%d): ", errctx.line + 1, errctx.column);
    strprep(errctx.message, linecol);
    strprep(errctx.message, "Error parsing JSON ");
    if (result)
    {
        json_free(result);
        result = NULL;
    }
    return result;
}

struct json *json_read_string(const char *json_string, char *errbuf)
{
    if (!json_string)
        return NULL;

    FILE *memfile = fmemopen((void *)json_string, strlen(json_string), "r");
    if (!memfile)
        return NULL;

    struct json *result = json_read(memfile, errbuf);
    fclose(memfile);
    return result;
}

int json_parser_json(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx)
{
    return json_parser_literal(in, token, dest, errctx) || json_parser_array(in, token, dest, errctx) || json_parser_object(in, token, dest, errctx);
}

int json_parser_array(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx)
{
    if (token->type == JSON_TOKEN_ARRAY_START)
    {
        *dest = __json_array_macro(NULL); // Create empty array
        *token = json_read_token(in, errctx);
        struct json *element = NULL;
        if (json_parser_json(in, token, &element, errctx))
        {
            json_array_push(*dest, element);
            *token = json_read_token(in, errctx);
            while (token->type == JSON_TOKEN_COMMA)
            {
                *token = json_read_token(in, errctx);
                if (json_parser_json(in, token, &element, errctx))
                {
                    json_array_push(*dest, element);
                    *token = json_read_token(in, errctx);
                }
                else // Error: Unexpected inner JSON
                {
                    strcpy(errctx->message, "Expected JSON value after comma in array.");
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

int json_parser_object(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx)
{
    if (token->type == JSON_TOKEN_OBJECT_START)
    {
        *dest = __json_object_macro(NULL); // Create empty object
        *token = json_read_token(in, errctx);

        // Check for empty object first
        if (token->type == JSON_TOKEN_OBJECT_END)
        {
            return 1; // Empty object is valid
        }

        if (json_parser_key_value(in, token, *dest, errctx))
        {
            *token = json_read_token(in, errctx);
            while (token->type == JSON_TOKEN_COMMA)
            {
                *token = json_read_token(in, errctx);
                if (json_parser_key_value(in, token, *dest, errctx))
                {
                    *token = json_read_token(in, errctx);
                }
                else
                {
                    strcpy(errctx->message, "Expected key-value pair after comma in object.");
                    if (*dest && json_is_object(*dest))
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
                if (*dest && json_is_object(*dest))
                    json_free(*dest);
                *dest = NULL;
                return 0;
            }
        }
        else
        {
            // Failed to parse first key-value pair
            if (*dest && json_is_object(*dest))
                json_free(*dest);
            *dest = NULL;
            return 0;
        }
    }
    // Not an object
    return 0;
}

int json_parser_key_value(FILE *in, struct json_token *token, struct json *object, struct error_context *errctx)
{
    if (token->type == JSON_TOKEN_STRING)
    {
        char *key = strdup(token->value);
        *token = json_read_token(in, errctx);
        if (token->type == JSON_TOKEN_COLON)
        {
            *token = json_read_token(in, errctx);
            struct json *value = NULL;
            if (json_parser_json(in, token, &value, errctx))
            {
                json_object_set(object, key, value);
                free(key);
                return 1;
            }
            else
            {
                strcpy(errctx->message, "Expected JSON value after ':' in object.");
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
    else
    {
        return 0;
    }
}

int json_parser_literal(FILE *in, struct json_token *token, struct json **dest, struct error_context *errctx)
{
    if (token->type == JSON_TOKEN_NULL)
    {
        *dest = json_null();
        return 1;
    }
    else if (token->type == JSON_TOKEN_TRUE)
    {
        *dest = json_true();
        return 1;
    }
    else if (token->type == JSON_TOKEN_FALSE)
    {
        *dest = json_false();
        return 1;
    }
    else if (token->type == JSON_TOKEN_NUMBER)
    {
        *dest = json_number(atof(token->value));
        return 1;
    }
    else if (token->type == JSON_TOKEN_STRING)
    {
        *dest = json_string(token->value);
        return 1;
    }
    return 0;
}

struct json_token json_read_token(FILE *in, struct error_context *errctx)
{
    struct json_token token = {0};
    int c, i = 0;
    const char *error = "Invalid token: ";
    int error_length = strlen(error);

    // Skip whitespace
    while (isspace(c = update_error_context(errctx, fgetc(in), i)))
        ;

    if (c == EOF)
    {
        token.type = JSON_TOKEN_EOF;
        return token;
    }

    switch (c)
    {
    case 'n':
    {
        i = 1;
        if (
            update_error_context(errctx, fgetc(in), i++) == 'u' &&
            update_error_context(errctx, fgetc(in), i++) == 'l' &&
            update_error_context(errctx, fgetc(in), i++) == 'l')
        {
            token.type = JSON_TOKEN_NULL;
        }
        else
        {
            token.type = JSON_TOKEN_INVALID;
        }
        break;
    }
    case 't':
    {
        i = 1;
        if (
            update_error_context(errctx, fgetc(in), i++) == 'r' &&
            update_error_context(errctx, fgetc(in), i++) == 'u' &&
            update_error_context(errctx, fgetc(in), i++) == 'e')
        {
            token.type = JSON_TOKEN_TRUE;
        }
        else
        {
            token.type = JSON_TOKEN_INVALID;
        }
        break;
    }
    case 'f':
    {
        i = 1;
        if (
            update_error_context(errctx, fgetc(in), i++) == 'a' &&
            update_error_context(errctx, fgetc(in), i++) == 'l' &&
            update_error_context(errctx, fgetc(in), i++) == 's' &&
            update_error_context(errctx, fgetc(in), i++) == 'e')
        {
            token.type = JSON_TOKEN_FALSE;
        }
        else
        {
            token.type = JSON_TOKEN_INVALID;
        }
        break;
    }
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
    {
        char *str = NULL;
        struct linked_list *char_list = NULL, *char_list_ptr = NULL;
        i = 1;
        while ((c = update_error_context(errctx, fgetc(in), i++)) != '"')
        {
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
