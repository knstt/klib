/**
 * @file vec.h
 * @brief A lightweight, generic vector (dynamic array) library for C
 * 
 * This header-only library provides type-safe generic vectors using C macros.
 * It supports dynamic resizing, element access, iteration, and memory management.
 * 
 * @author Konstantin Eid
 * 
 * Usage Example:
 * ```c
 * #include "vec.h"
 * 
 * // Define a vector type for integers
 * VEC(int, IntVec);
 * 
 * int main() {
 *     IntVec my_vec = {0};
 *     VEC_PUSH(my_vec, 42);
 *     VEC_PUSH(my_vec, 100);
 *     
 *     VEC_FOR_EACH(int, val, my_vec) {
 *         printf("%d\n", *val);
 *     }
 *     
 *     VEC_RESET(my_vec);
 *     return 0;
 * }
 * ```
 */

#ifndef VEC_H
#define VEC_H

/**
 * @brief Custom memory allocation function
 * 
 * Define VEC_REALLOC before including vec.h to use a custom realloc function.
 * Default: standard library realloc
 */
#ifndef VEC_REALLOC
#include <stdlib.h>
#define VEC_REALLOC realloc
#endif

/**
 * @brief Custom memory deallocation function
 * 
 * Define VEC_FREE before including vec.h to use a custom free function.
 * Default: standard library free
 */
#ifndef VEC_FREE
#include <stdlib.h>
#define VEC_FREE free
#endif

/**
 * @brief Custom memory copy function
 * 
 * Define VEC_MEMCPY before including vec.h to use a custom memcpy function.
 * Default: standard library memcpy
 */
#ifndef VEC_MEMCPY
#include <string.h>
#define VEC_MEMCPY memcpy
#endif

/**
 * @brief Custom assertion function
 * 
 * Define VEC_ASSERT before including vec.h to use a custom assert function.
 * Default: standard library assert
 */
#ifndef VEC_ASSERT
#include <assert.h>
#define VEC_ASSERT assert
#endif

/**
 * @def VEC(type, name)
 * @brief Define a new vector type
 * 
 * Creates a typedef for a vector structure that holds elements of the specified type.
 * 
 * @param type The element type (e.g., int, double, struct MyStruct)
 * @param name The name of the resulting typedef
 * 
 * Example:
 * ```c
 * VEC(int, IntVec);
 * IntVec my_vec = {0};  // Initialize empty
 * ```
 */
#define VEC(type, name) \
    typedef struct {               \
        type* data;       \
        size_t size;       \
        size_t capacity;   \
    } name;

/**
 * @def VEC_PUSH(vec, value)
 * @brief Add an element to the end of the vector
 * 
 * Appends the given value to the vector, doubling the capacity if necessary.
 * 
 * @param vec The vector to modify
 * @param value The value to add
 */
#define VEC_PUSH(vec, value) do {          \
    if ((vec).size >= (vec).capacity) {      \
        (vec).capacity = ((vec).capacity == 0) ? 1 : (vec).capacity * 2; \
        (vec).data = VEC_REALLOC((vec).data, (vec).capacity * sizeof(*(vec).data)); \
    }                                           \
    (vec).data[(vec).size++] = (value);    \
} while(0)

/**
 * @def VEC_POP(vec)
 * @brief Remove the last element from the vector
 * 
 * Decrements the size if the vector is not empty.
 * Does not deallocate memory (use VEC_RESET for that).
 * 
 * @param vec The vector to modify
 */
#define VEC_POP(vec) do {               \
    if ((vec).size > 0) {                    \
        (vec).size--;                        \
    }                                           \
} while(0)

/**
 * @def VEC_PEEK(vec)
 * @brief Get the last element without removing it
 * 
 * Returns the value of the last element in the vector.
 * Undefined behavior if the vector is empty.
 * 
 * @param vec The vector to read from
 * @return The last element
 */
#define VEC_PEEK(vec) ((vec).data[(vec).size - 1])


/**
 * @def VEC_RESET(vec)
 * @brief Free all memory and reset the vector
 * 
 * Deallocates the internal data buffer and resets the vector to its initial state.
 * After calling this, the vector is ready to be reused.
 * 
 * @param vec The vector to reset
 */
#define VEC_RESET(vec) do {        \
    VEC_FREE((vec).data);               \
    (vec).data = NULL;              \
    (vec).size = 0;                 \
    (vec).capacity = 0;             \
} while(0)

/**
 * @def VEC_GET(vec, index)
 * @brief Access an element by index
 * 
 * Returns the element at the specified index.
 * Undefined behavior if index >= size.
 * 
 * @param vec The vector to read from
 * @param index The zero-based index
 * @return The element at the given index
 */
#define VEC_GET(vec, index) ((vec).data[(index)])

/**
 * @def VEC_SIZE(vec)
 * @brief Get the current number of elements
 * 
 * Returns the number of elements currently in the vector.
 * 
 * @param vec The vector to query
 * @return The number of elements
 */
#define VEC_SIZE(vec) ((vec).size)

/**
 * @def VEC_CAPACITY(vec)
 * @brief Get the allocated capacity
 * 
 * Returns the number of elements that can be stored without reallocation.
 * 
 * @param vec The vector to query
 * @return The current capacity
 */
#define VEC_CAPACITY(vec) ((vec).capacity)

/**
 * @def VEC_CLEAR(vec)
 * @brief Reset size to zero without deallocating memory
 * 
 * Sets the size to 0, allowing elements to be overwritten without freeing
 * the allocated buffer. The capacity remains unchanged.
 * 
 * @param vec The vector to clear
 */
#define VEC_CLEAR(vec) do {      \
    (vec).size = 0;                 \
} while(0)

/**
 * @def VEC_IS_EMPTY(vec)
 * @brief Check if the vector is empty
 * 
 * Returns true (non-zero) if the vector has no elements.
 * 
 * @param vec The vector to check
 * @return Non-zero if empty, 0 otherwise
 */
#define VEC_IS_EMPTY(vec) ((vec).size == 0)

/**
 * @def VEC_RESERVE(vec, new_size)
 * @brief Ensure the capacity is at least new_size
 * 
 * Reallocates the buffer if the current capacity is less than new_size.
 * Size remains unchanged. Does nothing if capacity is already sufficient.
 * 
 * @param vec The vector to reserve space in
 * @param new_size The minimum desired capacity
 */
#define VEC_RESERVE(vec, new_size) do {          \
    if ((new_size) > (vec).capacity) {            \
        (vec).capacity = (new_size);               \
        (vec).data = VEC_REALLOC((vec).data, (vec).capacity * sizeof(*(vec).data)); \
    }                                               \
} while(0)

/**
 * @def VEC_RESIZE(vec, new_size)
 * @brief Resize the vector to the specified size
 * 
 * Changes both capacity and size to new_size. Reallocates if necessary.
 * New elements are uninitialized if expanding the vector.
 * 
 * @param vec The vector to resize
 * @param new_size The desired size and capacity
 */
#define VEC_RESIZE(vec, new_size) do {          \
    VEC_RESERVE(vec, new_size);                  \
    (vec).size = (new_size);                      \
} while(0)

/**
 * @def VEC_PUSH_MANY(vec, values, count)
 * @brief Add multiple elements to the vector at once
 * 
 * Appends count elements from the values array to the vector.
 * More efficient than calling VEC_PUSH in a loop.
 * 
 * @param vec The vector to modify
 * @param values Pointer to an array of elements to add
 * @param count The number of elements to add
 */
#define VEC_PUSH_MANY(vec, values, count) do {          \
    VEC_RESERVE(vec, (vec).size + count);                         \
    VEC_MEMCPY((vec).data + (vec).size, (values), (count) * sizeof(*(vec).data)); \
    (vec).size += (count);                           \
} while(0)

/**
 * @def VEC_FOR_EACH(type, iter, vec)
 * @brief Iterate over all elements in the vector
 * 
 * Provides a for-loop to iterate over each element.
 * The iterator is a pointer to the element type.
 * 
 * Example:
 * ```c
 * VEC_FOR_EACH(int, it, my_vec) {
 *     printf("%d\n", *it);
 * }
 * ```
 * 
 * @param type The element type
 * @param iter The loop variable (pointer to type)
 * @param vec The vector to iterate over
 */
#define VEC_FOR_EACH(type, iter, vec) for (type *iter = (vec).data; iter < (vec).data + (vec).size; ++iter) 


/**
 * @section Testing
 * 
 * Define VEC_TEST before including this header to compile built-in tests.
 * Example: gcc -DVEC_TEST tests.c -o tests && ./tests
 */
#ifdef VEC_TEST
#include <stdio.h>


VEC(int, my_type);

int vec_test() {
    printf("=== Vector Macro Tests ===\n");

    // Test 1: Basic initialization and push
    printf("Test 1: Basic initialization and VEC_PUSH\n");
    my_type my_vec = {0};
    assert(VEC_SIZE(my_vec) == 0);
    assert(VEC_CAPACITY(my_vec) == 0);
    
    for (int i = 0; i < 10; ++i) {
        VEC_PUSH(my_vec, i);
    }
    assert(VEC_SIZE(my_vec) == 10);
    assert(VEC_CAPACITY(my_vec) >= 10);

    // Test 2: VEC_PEEK
    printf("Test 2: VEC_PEEK\n");
    assert(VEC_PEEK(my_vec) == 9);

    // Test 3: VEC_POP
    printf("Test 3: VEC_POP\n");
    size_t size_before = VEC_SIZE(my_vec);
    VEC_POP(my_vec);
    VEC_POP(my_vec);
    assert(VEC_SIZE(my_vec) == size_before - 2);
    assert(VEC_PEEK(my_vec) == 7);

    // Test 4: VEC_GET
    printf("Test 4: VEC_GET\n");
    assert(VEC_GET(my_vec, 0) == 0);
    assert(VEC_GET(my_vec, 5) == 5);
    assert(VEC_GET(my_vec, 7) == 7);

    // Test 5: VEC_CLEAR
    printf("Test 5: VEC_CLEAR\n");
    size_t capacity_before = VEC_CAPACITY(my_vec);
    VEC_CLEAR(my_vec);
    assert(VEC_SIZE(my_vec) == 0);
    assert(VEC_CAPACITY(my_vec) == capacity_before);
    assert(VEC_IS_EMPTY(my_vec));

    // Test 6: VEC_RESIZE
    printf("Test 6: VEC_RESIZE\n");
    VEC_RESIZE(my_vec, 20);
    assert(VEC_SIZE(my_vec) == 20);
    assert(VEC_CAPACITY(my_vec) >= 20);

    // Test 7: VEC_PUSH_MANY
    printf("Test 7: VEC_PUSH_MANY\n");
    int values[] = {100, 200, 300, 400, 500};
    VEC_CLEAR(my_vec);
    VEC_PUSH_MANY(my_vec, values, 5);
    assert(VEC_SIZE(my_vec) == 5);
    assert(VEC_GET(my_vec, 0) == 100);
    assert(VEC_GET(my_vec, 1) == 200);
    assert(VEC_GET(my_vec, 4) == 500);

    // Test 8: VEC_IS_EMPTY
    printf("Test 8: VEC_IS_EMPTY\n");
    assert(!VEC_IS_EMPTY(my_vec));
    VEC_CLEAR(my_vec);
    assert(VEC_IS_EMPTY(my_vec));

    // Test 9: VEC_FOR_EACH
    printf("Test 9: VEC_FOR_EACH\n");
    VEC_CLEAR(my_vec);
    for (int i = 1; i <= 5; ++i) {
        VEC_PUSH(my_vec, i * 10);
    }
    int expected[] = {10, 20, 30, 40, 50};
    int index = 0;
    VEC_FOR_EACH(int, it, my_vec) {
        assert(*it == expected[index++]);
    }
    assert(index == 5);

    // Test 10: VEC_FREE
    printf("Test 10: VEC_RESET\n");
    assert(!VEC_IS_EMPTY(my_vec));
    VEC_RESET(my_vec);
    assert(VEC_IS_EMPTY(my_vec));
    assert(VEC_SIZE(my_vec) == 0);
    assert(VEC_CAPACITY(my_vec) == 0);
    assert(my_vec.data == NULL);

    printf("=== All Tests Passed ===\n");
    return 0;
}

#endif
#endif // VEC_H
