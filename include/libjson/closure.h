/**
 * @file closure.h
 * @brief Closure structure for context-aware callbacks
 */

#ifndef LIBJSON_CLOSURE_H
#define LIBJSON_CLOSURE_H

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
 * @brief Function type for closures with function argument only.
 */
typedef void *(*pure_func)(void *arg);

/**
 * @brief Function type for closures with only args and no return value.
 */
typedef void (*call_func)(void *arg);

/**
 * @brief Adapter to create closure for pure functions
 */
struct closure *closure_pure(pure_func func);

/**
 * @brief Adapter to create closure for functions with no return value
 */
struct closure *closure_call(call_func func);

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
 * @brief Invokes the closure function with the given arg and internal context
 * @param closure The closure to invoke
 * @param arg The arg to pass to the closure function
 * @return The result of the closure function
 */
void *closure_invoke(struct closure *closure, void *arg);

#endif // LIBJSON_CLOSURE_H
