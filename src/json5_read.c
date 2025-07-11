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
    // For now, delegate to the standard JSON tokenizer
    // We'll enhance this to support JSON5 features
    return json_read_token(in, errctx);
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
        *dest = __json_object_macro((struct json_key_value[]){0}); // Create empty object
        *token = json5_read_token(in, errctx);
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
    // Not an object
    return 0;
}

int json5_parser_key_value(FILE *in, struct json_token *token, struct json *object, struct error_context *errctx)
{
    // For now, delegate to the standard JSON key-value parser
    return json_parser_key_value(in, token, object, errctx);
}