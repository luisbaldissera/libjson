#include "json_internal.h"

// Static JSON singleton values
struct json json_null_value = {.type = JSON_NULL, .value = {0}};
struct json json_true_value = {.type = JSON_BOOLEAN, .value = {.boolean = 1}};
struct json json_false_value = {.type = JSON_BOOLEAN, .value = {.boolean = 0}};

// The default error buffer to store error when no error buffer is provided.
char __default_errbuf[LIBJSON_ERRBUF_SiZE];

// Helper function for strings
void strprep(char *dst, const char *src)
{
    int len_src = strlen(src);
    int len_dst = strlen(dst);
    // Shift existing string to the right
    memmove(dst + len_src, dst, len_dst + 1);
    // Copy new string to the beginning
    memcpy(dst, src, len_src);
}

// Helper for error handling
int update_error_context(struct error_context *errctx, const char c, int index)
{
    if (errctx)
    {
        if (c == '\n' || c == '\r')
        {
            errctx->line++;
            errctx->column = 0;
        }
        else if (c == ' ')
        {
            errctx->column = 0;
        }
        else
        {
            errctx->column++;
        }
        errctx->message[index] = c;
    }
    return (int)c;
}
