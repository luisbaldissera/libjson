#include "libjson/closure.h"

#include <stdlib.h>

struct closure
{
    closure_func func;
    void *ctx;
};

struct closure *closure_new(closure_func func, void *ctx)
{
    struct closure *closure = malloc(sizeof(struct closure));
    if (!closure)
    {
        return NULL;
    }
    closure->func = func;
    closure->ctx = ctx;
    return closure;
}

void closure_free(struct closure *closure)
{
    if (closure)
    {
        free(closure);
    }
}

void *closure_invoke(struct closure *closure, void *arg)
{
    if (!closure || !closure->func)
    {
        return NULL;
    }
    return closure->func(arg, closure->ctx);
}