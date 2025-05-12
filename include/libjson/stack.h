#ifndef LIBJSON_STACK_H
#define LIBJSON_STACK_H

/**
 * @file stack.h
 */

#include "libjson/types.h"

/**
 * @brief Stack structure
 */
struct stack;

/**
 * @brief Creates a new stack
 * @return Pointer to the new stack, or NULL on allocation failure
 */
struct stack *stack_new();

/**
 * @brief Frees a stack and optionally its values
 * @param stack The stack to free
 * @param free_value Function to free the value stored in each node, or NULL if not needed
 */
void stack_free(struct stack *stack, free_func free_value);

/**
 * @brief Pushes a value onto the stack
 * @param stack The stack to push the value onto
 * @param value Pointer to the value to store in the stack
 */
void stack_push(struct stack *stack, void *value);

/**
 * @brief Pops a value from the stack
 * @param stack The stack to pop the value from
 * @return Pointer to the popped value, or NULL if the stack is empty
 */
void *stack_pop(struct stack *stack);

/**
 * @brief Checks the length of the stack
 * @param stack The stack to check
 * @return the length of the stack
 */
int stack_length(struct stack *stack);

/**
 * @brief Peeks at the top value of the stack without removing it
 * @param stack The stack to check
 * @return Pointer to the top value, or NULL if the stack is empty
 */
void *stack_peek(struct stack *stack);

#endif // LIBJSON_STACK_H