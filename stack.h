//
// stack.h - A lightweight, generic stack library for C
//
// USAGE:
//   Define a stack type with a maximum size, then use the macros to manipulate the stack.
//
//   Example:
//     #include "stack.h"
//
//     STACK(int, IntStack, 100);
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
//         STACK_CLEAR(my_stack);
//         return 0;
//     }
//
// FEATURES:
//   - Type-safe generic stacks using C macros
//   - LIFO (Last In, First Out) semantics
//   - Fixed-size array-based implementation
//   - No dynamic memory allocation
//
// CUSTOMIZATION:
//   #define STACK_ASSERT to customize assertions (default: assert)
//

#ifndef STACK_H_
#define STACK_H_

#ifndef STACK_ASSERT
#include <assert.h>
#define STACK_ASSERT assert
#endif

// STACK(type, name, max_size) - Define a new stack type with fixed maximum size
//
// Creates a typedef for a stack structure that holds up to max_size elements of the specified type.
//
// Example:
//   STACK(int, IntStack, 100);
//   IntStack my_stack = {0};  // Initialize empty
#define STACK(type, name, max_size) \
    typedef struct                  \
    {                               \
        type data[max_size];        \
        size_t size;                \
    } name;

// STACK_PUSH(stack, value) - Push an element onto the stack
//
// Adds the given value to the top of the stack.
// Asserts if the stack is already at maximum capacity.
#define STACK_PUSH(stack, value)           \
    do                                     \
    {                                      \
        STACK_ASSERT((stack).size < sizeof((stack).data) / sizeof(*(stack).data)); \
        (stack).data[(stack).size++] = (value);  \
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

// STACK_RESET(stack) - Reset the stack to empty state
//
// Sets the stack size to 0. No memory deallocation needed for fixed-size stacks.
#define STACK_RESET(stack)   \
    do                       \
    {                        \
        (stack).size = 0;    \
    } while (0)

// STACK_SIZE(stack) - Get the current number of elements
//
// Returns the number of elements currently in the stack.
#define STACK_SIZE(stack) ((stack).size)

// STACK_CAPACITY(stack) - Get the maximum capacity
//
// Returns the maximum number of elements that can be stored in the stack.
#define STACK_CAPACITY(stack) (sizeof((stack).data) / sizeof(*(stack).data))

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
