//
// stack.h - A lightweight, generic stack library for C
//
// USAGE:
//   Define a stack type, then use the macros to manipulate the stack.
//
//   Example:
//     #include "stack.h"
//
//     STACK(int, IntStack);
//
//     int main() {
//         IntStack my_stack = {0};
//         STACK_PUSH(my_stack, 42);
//         STACK_PUSH(my_stack, 100);
//
//         while (!STACK_IS_EMPTY(my_stack)) {
//             printf("%d\n", STACK_TOP(my_stack));
//             STACK_POP(my_stack);
//         }
//
//         STACK_RESET(my_stack);
//         return 0;
//     }
//
// FEATURES:
//   - Type-safe generic stacks using C macros
//   - LIFO (Last In, First Out) semantics
//   - Dynamic array-based implementation with automatic resizing
//   - Custom memory allocator support
//
// CUSTOMIZATION:
//   #define STACK_REALLOC to customize allocation (default: realloc)
//   #define STACK_FREE to customize deallocation (default: free)
//   #define STACK_ASSERT to customize assertions (default: assert)
//

#ifndef STACK_H_
#define STACK_H_

#ifndef STACK_REALLOC
#include <stdlib.h>
#define STACK_REALLOC realloc
#endif

#ifndef STACK_FREE
#include <stdlib.h>
#define STACK_FREE free
#endif

#ifndef STACK_ASSERT
#include <assert.h>
#define STACK_ASSERT assert
#endif

// STACK(type, name) - Define a new stack type
//
// Creates a typedef for a stack structure that holds elements of the specified type.
//
// Example:
//   STACK(int, IntStack);
//   IntStack my_stack = {0};  // Initialize empty
#define STACK(type, name) \
    typedef struct        \
    {                     \
        type *data;       \
        size_t size;      \
        size_t capacity;  \
    } name;

// STACK_PUSH(stack, value) - Push an element onto the stack
//
// Adds the given value to the top of the stack, doubling the capacity if necessary.
#define STACK_PUSH(stack, value)                                                                  \
    do                                                                                            \
    {                                                                                             \
        if ((stack).size >= (stack).capacity)                                                     \
        {                                                                                         \
            (stack).capacity = ((stack).capacity == 0) ? 1 : (stack).capacity * 2;                \
            (stack).data = STACK_REALLOC((stack).data, (stack).capacity * sizeof(*(stack).data)); \
        }                                                                                         \
        (stack).data[(stack).size++] = (value);                                                   \
    } while (0)

// STACK_POP(stack) - Remove the top element from the stack
//
// Removes the element at the top of the stack.
// Does nothing if the stack is empty.
#define STACK_POP(stack)      \
    do                        \
    {                         \
        if ((stack).size > 0) \
        {                     \
            (stack).size--;   \
        }                     \
    } while (0)

// STACK_TOP(stack) - Get the top element without removing it
//
// Returns the value of the element at the top of the stack.
// Undefined behavior if the stack is empty.
#define STACK_TOP(stack) ((stack).data[(stack).size - 1])

// STACK_RESET(stack) - Free all memory and reset the stack
//
// Deallocates the internal data buffer and resets the stack to its initial state.
// After calling this, the stack is ready to be reused.
#define STACK_RESET(stack)        \
    do                            \
    {                             \
        STACK_FREE((stack).data); \
        (stack).data = NULL;      \
        (stack).size = 0;         \
        (stack).capacity = 0;     \
    } while (0)

// STACK_SIZE(stack) - Get the current number of elements
//
// Returns the number of elements currently in the stack.
#define STACK_SIZE(stack) ((stack).size)

// STACK_CAPACITY(stack) - Get the allocated capacity
//
// Returns the number of elements that can be stored without reallocation.
#define STACK_CAPACITY(stack) ((stack).capacity)

// STACK_IS_EMPTY(stack) - Check if the stack is empty
//
// Returns true (non-zero) if the stack has no elements.
#define STACK_IS_EMPTY(stack) ((stack).size == 0)

// STACK_CLEAR(stack) - Reset size to zero without deallocating memory
//
// Sets the size to 0, allowing elements to be overwritten without freeing
// the allocated buffer. The capacity remains unchanged.
#define STACK_CLEAR(stack) \
    do                     \
    {                      \
        (stack).size = 0;  \
    } while (0)

#endif // STACK_H_
