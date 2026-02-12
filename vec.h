//
// vec.h - A lightweight, generic vector (dynamic array) library for C
//
// USAGE:
//   Define a vector type, then use the macros to manipulate the vector.
//
//   Example:
//     #include "vec.h"
//
//     VEC(int, IntVec);
//
//     int main() {
//         IntVec my_vec = {0};
//         VEC_PUSH(my_vec, 42);
//         VEC_PUSH(my_vec, 100);
//
//         VEC_FOR_EACH(int, val, my_vec) {
//             printf("%d\n", *val);
//         }
//
//         VEC_RESET(my_vec);
//         return 0;
//     }
//
// FEATURES:
//   - Type-safe generic vectors using C macros
//   - Dynamic resizing with automatic capacity doubling
//   - Element access, iteration, and bulk operations
//   - Custom memory allocator support
//
// CUSTOMIZATION:
//   #define VEC_REALLOC to customize allocation (default: realloc)
//   #define VEC_FREE to customize deallocation (default: free)
//   #define VEC_MEMCPY to customize memory copy (default: memcpy)
//   #define VEC_ASSERT to customize assertions (default: assert)
//

#ifndef VEC_H_
#define VEC_H_

#ifndef VEC_REALLOC
#include <stdlib.h>
#define VEC_REALLOC realloc
#endif

#ifndef VEC_FREE
#include <stdlib.h>
#define VEC_FREE free
#endif

#ifndef VEC_MEMCPY
#include <string.h>
#define VEC_MEMCPY memcpy
#endif

#ifndef VEC_ASSERT
#include <assert.h>
#define VEC_ASSERT assert
#endif

// VEC(type, name) - Define a new vector type
//
// Creates a typedef for a vector structure that holds elements of the specified type.
//
// Example:
//   VEC(int, IntVec);
//   IntVec my_vec = {0};  // Initialize empty
#define VEC(type, name)  \
    typedef struct       \
    {                    \
        type *data;      \
        size_t size;     \
        size_t capacity; \
    } name;

// VEC_PUSH(vec, value) - Add an element to the end of the vector
//
// Appends the given value to the vector, doubling the capacity if necessary.
#define VEC_PUSH(vec, value)                                                            \
    do                                                                                  \
    {                                                                                   \
        if ((vec).size >= (vec).capacity)                                               \
        {                                                                               \
            (vec).capacity = ((vec).capacity == 0) ? 1 : (vec).capacity * 2;            \
            (vec).data = VEC_REALLOC((vec).data, (vec).capacity * sizeof(*(vec).data)); \
        }                                                                               \
        (vec).data[(vec).size++] = (value);                                             \
    } while (0)

// VEC_POP(vec) - Remove the last element from the vector
//
// Decrements the size if the vector is not empty.
// Does not deallocate memory (use VEC_RESET for that).
#define VEC_POP(vec)        \
    do                      \
    {                       \
        if ((vec).size > 0) \
        {                   \
            (vec).size--;   \
        }                   \
    } while (0)

// VEC_PEEK(vec) - Get the last element without removing it
//
// Returns the value of the last element in the vector.
// Undefined behavior if the vector is empty.
#define VEC_PEEK(vec) ((vec).data[(vec).size - 1])

// VEC_RESET(vec) - Free all memory and reset the vector
//
// Deallocates the internal data buffer and resets the vector to its initial state.
// After calling this, the vector is ready to be reused.
#define VEC_RESET(vec)        \
    do                        \
    {                         \
        VEC_FREE((vec).data); \
        (vec).data = NULL;    \
        (vec).size = 0;       \
        (vec).capacity = 0;   \
    } while (0)

// VEC_GET(vec, index) - Access an element by index
//
// Returns the element at the specified index.
// Undefined behavior if index >= size.
#define VEC_GET(vec, index) ((vec).data[(index)])

// VEC_SIZE(vec) - Get the current number of elements
//
// Returns the number of elements currently in the vector.
#define VEC_SIZE(vec) ((vec).size)

// VEC_CAPACITY(vec) - Get the allocated capacity
//
// Returns the number of elements that can be stored without reallocation.
#define VEC_CAPACITY(vec) ((vec).capacity)

// VEC_CLEAR(vec) - Reset size to zero without deallocating memory
//
// Sets the size to 0, allowing elements to be overwritten without freeing
// the allocated buffer. The capacity remains unchanged.
#define VEC_CLEAR(vec)  \
    do                  \
    {                   \
        (vec).size = 0; \
    } while (0)

// VEC_IS_EMPTY(vec) - Check if the vector is empty
//
// Returns true (non-zero) if the vector has no elements.
#define VEC_IS_EMPTY(vec) ((vec).size == 0)

// VEC_RESERVE(vec, new_size) - Ensure the capacity is at least new_size
//
// Reallocates the buffer if the current capacity is less than new_size.
// Size remains unchanged. Does nothing if capacity is already sufficient.
#define VEC_RESERVE(vec, new_size)                                                      \
    do                                                                                  \
    {                                                                                   \
        if ((new_size) > (vec).capacity)                                                \
        {                                                                               \
            (vec).capacity = (new_size);                                                \
            (vec).data = VEC_REALLOC((vec).data, (vec).capacity * sizeof(*(vec).data)); \
        }                                                                               \
    } while (0)

// VEC_RESIZE(vec, new_size) - Resize the vector to the specified size
//
// Changes both capacity and size to new_size. Reallocates if necessary.
// New elements are uninitialized if expanding the vector.
#define VEC_RESIZE(vec, new_size)   \
    do                              \
    {                               \
        VEC_RESERVE(vec, new_size); \
        (vec).size = (new_size);    \
    } while (0)

// VEC_PUSH_MANY(vec, values, count) - Add multiple elements to the vector at once
//
// Appends count elements from the values array to the vector.
// More efficient than calling VEC_PUSH in a loop.
#define VEC_PUSH_MANY(vec, values, count)                                             \
    do                                                                                \
    {                                                                                 \
        VEC_RESERVE(vec, (vec).size + count);                                         \
        VEC_MEMCPY((vec).data + (vec).size, (values), (count) * sizeof(*(vec).data)); \
        (vec).size += (count);                                                        \
    } while (0)

// VEC_FOR_EACH(type, iter, vec) - Iterate over all elements in the vector
//
// Provides a for-loop to iterate over each element.
// The iterator is a pointer to the element type.
//
// Example:
//   VEC_FOR_EACH(int, it, my_vec) {
//       printf("%d\n", *it);
//   }
#define VEC_FOR_EACH(type, iter, vec) for (type *iter = (vec).data; iter < (vec).data + (vec).size; ++iter)

#endif // VEC_H_
