/**
 * @file stack.h
 * @brief A lightweight, generic stack library for C
 * 
 * This header-only library provides type-safe generic stacks using C macros.
 * Stacks follow LIFO (Last In, First Out) semantics.
 * 
 * @author Konstantin Eid
 * 
 * Usage Example:
 * ```c
 * #include "stack.h"
 * 
 * // Define a stack type for integers
 * STACK(int, IntStack);
 * 
 * int main() {
 *     IntStack my_stack = {0};
 *     STACK_PUSH(my_stack, 42);
 *     STACK_PUSH(my_stack, 100);
 *     
 *     while (!STACK_IS_EMPTY(my_stack)) {
 *         printf("%d\n", STACK_TOP(my_stack));
 *         STACK_POP(my_stack);
 *     }
 *     
 *     STACK_RESET(my_stack);
 *     return 0;
 * }
 * ```
 */

#ifndef STACK_H
#define STACK_H

/**
 * @brief Custom memory allocation function
 * 
 * Define STACK_REALLOC before including stack.h to use a custom realloc function.
 * Default: standard library realloc
 */
#ifndef STACK_REALLOC
#include <stdlib.h>
#define STACK_REALLOC realloc
#endif

/**
 * @brief Custom memory deallocation function
 * 
 * Define STACK_FREE before including stack.h to use a custom free function.
 * Default: standard library free
 */
#ifndef STACK_FREE
#include <stdlib.h>
#define STACK_FREE free
#endif

/**
 * @brief Custom assertion function
 * 
 * Define STACK_ASSERT before including stack.h to use a custom assert function.
 * Default: standard library assert
 */
#ifndef STACK_ASSERT
#include <assert.h>
#define STACK_ASSERT assert
#endif

/**
 * @def STACK(type, name)
 * @brief Define a new stack type
 * 
 * Creates a typedef for a stack structure that holds elements of the specified type.
 * 
 * @param type The element type (e.g., int, double, struct MyStruct)
 * @param name The name of the resulting typedef
 * 
 * Example:
 * ```c
 * STACK(int, IntStack);
 * IntStack my_stack = {0};  // Initialize empty
 * ```
 */
#define STACK(type, name) \
    typedef struct {               \
        type* data;                \
        size_t size;               \
        size_t capacity;           \
    } name;

/**
 * @def STACK_PUSH(stack, value)
 * @brief Push an element onto the stack
 * 
 * Adds the given value to the top of the stack, doubling the capacity if necessary.
 * 
 * @param stack The stack to modify
 * @param value The value to add
 */
#define STACK_PUSH(stack, value) do {          \
    if ((stack).size >= (stack).capacity) {      \
        (stack).capacity = ((stack).capacity == 0) ? 1 : (stack).capacity * 2; \
        (stack).data = STACK_REALLOC((stack).data, (stack).capacity * sizeof(*(stack).data)); \
    }                                           \
    (stack).data[(stack).size++] = (value);    \
} while(0)

/**
 * @def STACK_POP(stack)
 * @brief Remove the top element from the stack
 * 
 * Removes the element at the top of the stack.
 * Does nothing if the stack is empty.
 * 
 * @param stack The stack to modify
 */
#define STACK_POP(stack) do {               \
    if ((stack).size > 0) {                    \
        (stack).size--;                        \
    }                                           \
} while(0)

/**
 * @def STACK_TOP(stack)
 * @brief Get the top element without removing it
 * 
 * Returns the value of the element at the top of the stack.
 * Undefined behavior if the stack is empty.
 * 
 * @param stack The stack to read from
 * @return The top element
 */
#define STACK_TOP(stack) ((stack).data[(stack).size - 1])

/**
 * @def STACK_RESET(stack)
 * @brief Free all memory and reset the stack
 * 
 * Deallocates the internal data buffer and resets the stack to its initial state.
 * After calling this, the stack is ready to be reused.
 * 
 * @param stack The stack to reset
 */
#define STACK_RESET(stack) do {        \
    STACK_FREE((stack).data);               \
    (stack).data = NULL;              \
    (stack).size = 0;                 \
    (stack).capacity = 0;             \
} while(0)

/**
 * @def STACK_SIZE(stack)
 * @brief Get the current number of elements
 * 
 * Returns the number of elements currently in the stack.
 * 
 * @param stack The stack to query
 * @return The number of elements
 */
#define STACK_SIZE(stack) ((stack).size)

/**
 * @def STACK_CAPACITY(stack)
 * @brief Get the allocated capacity
 * 
 * Returns the number of elements that can be stored without reallocation.
 * 
 * @param stack The stack to query
 * @return The current capacity
 */
#define STACK_CAPACITY(stack) ((stack).capacity)

/**
 * @def STACK_IS_EMPTY(stack)
 * @brief Check if the stack is empty
 * 
 * Returns true (non-zero) if the stack has no elements.
 * 
 * @param stack The stack to check
 * @return Non-zero if empty, 0 otherwise
 */
#define STACK_IS_EMPTY(stack) ((stack).size == 0)

/**
 * @def STACK_CLEAR(stack)
 * @brief Reset size to zero without deallocating memory
 * 
 * Sets the size to 0, allowing elements to be overwritten without freeing
 * the allocated buffer. The capacity remains unchanged.
 * 
 * @param stack The stack to clear
 */
#define STACK_CLEAR(stack) do {      \
    (stack).size = 0;                 \
} while(0)

/**
 * @section Testing
 * 
 * Define STACK_TEST before including this header to compile built-in tests.
 * Example: gcc -DSTACK_TEST tests.c -o tests && ./tests
 */
#ifdef STACK_TEST
#include <stdio.h>

STACK(int, IntStack);

int stack_test() {
    printf("=== Stack Macro Tests ===\n");

    // Test 1: Basic initialization and push
    printf("Test 1: Basic initialization and STACK_PUSH\n");
    IntStack my_stack = {0};
    assert(STACK_SIZE(my_stack) == 0);
    assert(STACK_CAPACITY(my_stack) == 0);
    
    for (int i = 0; i < 10; ++i) {
        STACK_PUSH(my_stack, i);
    }
    assert(STACK_SIZE(my_stack) == 10);
    assert(STACK_CAPACITY(my_stack) >= 10);

    // Test 2: STACK_TOP
    printf("Test 2: STACK_TOP\n");
    assert(STACK_TOP(my_stack) == 9);

    // Test 3: STACK_POP
    printf("Test 3: STACK_POP\n");
    size_t size_before = STACK_SIZE(my_stack);
    STACK_POP(my_stack);
    STACK_POP(my_stack);
    assert(STACK_SIZE(my_stack) == size_before - 2);
    assert(STACK_TOP(my_stack) == 7);

    // Test 4: STACK_IS_EMPTY
    printf("Test 4: STACK_IS_EMPTY\n");
    assert(!STACK_IS_EMPTY(my_stack));
    STACK_CLEAR(my_stack);
    assert(STACK_IS_EMPTY(my_stack));

    // Test 5: LIFO order
    printf("Test 5: LIFO order\n");
    STACK_CLEAR(my_stack);
    for (int i = 1; i <= 5; ++i) {
        STACK_PUSH(my_stack, i * 10);
    }
    assert(STACK_TOP(my_stack) == 50);
    STACK_POP(my_stack);
    assert(STACK_TOP(my_stack) == 40);
    STACK_POP(my_stack);
    assert(STACK_TOP(my_stack) == 30);

    // Test 6: STACK_RESET
    printf("Test 6: STACK_RESET\n");
    assert(!STACK_IS_EMPTY(my_stack));
    STACK_RESET(my_stack);
    assert(STACK_IS_EMPTY(my_stack));
    assert(STACK_SIZE(my_stack) == 0);
    assert(STACK_CAPACITY(my_stack) == 0);
    assert(my_stack.data == NULL);

    printf("=== All Stack Tests Passed ===\n");
    return 0;
}

#endif
#endif // STACK_H
