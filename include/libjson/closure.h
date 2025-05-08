/**
 * @file closure.h
 * @brief Closure structure for context-aware callbacks
 */

#ifndef LIBJSON_CLOSURE_H
#define LIBJSON_CLOSURE_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Closure structure for context-aware callbacks
     *
     * Enables passing additional context to predicates and callbacks
     */
    struct closure;

    /**
     * @brief Function type for closures with function argument and context.
     */
    typedef void *(*closure_func)(void *arg, void *ctx);

    /**
     * @brief Creates a new closure
     * @param func The function to call
     * @param context The context to pass to the function
     * @return A new closure, or NULL on allocation failure
     */
    struct closure *closure_new(closure_func func, void *ctx);

    /**
     * @brief Frees a closure
     * @param closure The closure to free
     */
    void closure_free(struct closure *closure);

    /**
     * @brief Invokes the closure function with the given value and internal context
     * @param closure The closure to invoke
     * @param value The value to pass to the closure function
     * @return The result of the closure function
     */
    void *closure_invoke(struct closure *closure, void *value);

#ifdef __cplusplus
}
#endif

#endif // LIBJSON_CLOSURE_H
