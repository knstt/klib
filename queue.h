/**
 * @file queue.h
 * @brief A lightweight, generic queue library for C
 *
 * This header-only library provides type-safe generic queues using C macros.
 * Queues follow FIFO (First In, First Out) semantics.
 *
 * @author Konstantin Eid
 *
 * Usage Example:
 * ```c
 * #include "queue.h"
 *
 * // Define a queue type for integers
 * QUEUE(int, IntQueue);
 *
 * int main() {
 *     IntQueue my_queue = {0};
 *     QUEUE_ENQUEUE(my_queue, 42);
 *     QUEUE_ENQUEUE(my_queue, 100);
 *
 *     while (!QUEUE_IS_EMPTY(my_queue)) {
 *         printf("%d\n", QUEUE_FRONT(my_queue));
 *         QUEUE_DEQUEUE(my_queue);
 *     }
 *
 *     QUEUE_RESET(my_queue);
 *     return 0;
 * }
 * ```
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <string.h>

/**
 * @brief Custom memory allocation function
 *
 * Define QUEUE_REALLOC before including queue.h to use a custom realloc function.
 * Default: standard library realloc
 */
#ifndef QUEUE_REALLOC
#include <stdlib.h>
#define QUEUE_REALLOC realloc
#endif

/**
 * @brief Custom memory deallocation function
 *
 * Define QUEUE_FREE before including queue.h to use a custom free function.
 * Default: standard library free
 */
#ifndef QUEUE_FREE
#include <stdlib.h>
#define QUEUE_FREE free
#endif

/**
 * @brief Custom assertion function
 *
 * Define QUEUE_ASSERT before including queue.h to use a custom assert function.
 * Default: standard library assert
 */
#ifndef QUEUE_ASSERT
#include <assert.h>
#define QUEUE_ASSERT assert
#endif

/**
 * @def QUEUE(type, name)
 * @brief Define a new queue type
 *
 * Creates a typedef for a queue structure that holds elements of the specified type.
 * Uses a simple dynamic array-based implementation with a front offset.
 *
 * @param type The element type (e.g., int, double, struct MyStruct)
 * @param name The name of the resulting typedef
 *
 * Example:
 * ```c
 * QUEUE(int, IntQueue);
 * IntQueue my_queue = {0};  // Initialize empty
 * ```
 */
#define QUEUE(type, name) \
    typedef struct        \
    {                     \
        type *data;       \
        size_t front;     \
        size_t size;      \
        size_t capacity;  \
    } name;

/**
 * @def QUEUE_ENQUEUE(queue, value)
 * @brief Add an element to the back of the queue
 *
 * Appends the given value to the back of the queue, doubling the capacity if necessary.
 * Also compacts the queue if too much space is wasted at the front.
 *
 * @param queue The queue to modify
 * @param value The value to add
 */
#define QUEUE_ENQUEUE(queue, value)                                                                        \
    do                                                                                                     \
    {                                                                                                      \
        if ((queue).front + (queue).size >= (queue).capacity)                                              \
        {                                                                                                  \
            if ((queue).capacity == 0)                                                                     \
            {                                                                                              \
                (queue).capacity = 1;                                                                      \
                (queue).data = QUEUE_REALLOC((queue).data, (queue).capacity * sizeof(*(queue).data));      \
            }                                                                                              \
            else if ((queue).front > (queue).capacity / 2)                                                 \
            {                                                                                              \
                /* Compact if too much space wasted at front */                                            \
                memmove((queue).data, (queue).data + (queue).front, (queue).size * sizeof(*(queue).data)); \
                (queue).front = 0;                                                                         \
            }                                                                                              \
            else                                                                                           \
            {                                                                                              \
                /* Expand capacity */                                                                      \
                (queue).capacity *= 2;                                                                     \
                (queue).data = QUEUE_REALLOC((queue).data, (queue).capacity * sizeof(*(queue).data));      \
            }                                                                                              \
        }                                                                                                  \
        (queue).data[(queue).front + (queue).size] = (value);\
        (queue).size += 1;                                           \
    } while (0)

/**
 * @def QUEUE_DEQUEUE(queue)
 * @brief Remove the front element from the queue
 *
 * Removes the element at the front of the queue.
 * Does nothing if the queue is empty.
 *
 * @param queue The queue to modify
 */
#define QUEUE_DEQUEUE(queue)  \
    do                        \
    {                         \
        if ((queue).size > 0) \
        {                     \
            (queue).front++;  \
            (queue).size--;   \
        }                     \
    } while (0)

/**
 * @def QUEUE_FRONT(queue)
 * @brief Get the front element without removing it
 *
 * Returns the value of the element at the front of the queue.
 * Undefined behavior if the queue is empty.
 *
 * @param queue The queue to read from
 * @return The front element
 */
#define QUEUE_FRONT(queue) ((queue).data[(queue).front])

/**
 * @def QUEUE_BACK(queue)
 * @brief Get the back element without removing it
 *
 * Returns the value of the element at the back of the queue.
 * Undefined behavior if the queue is empty.
 *
 * @param queue The queue to read from
 * @return The back element
 */
#define QUEUE_BACK(queue) ((queue).data[(queue).front + (queue).size - 1])

/**
 * @def QUEUE_RESET(queue)
 * @brief Free all memory and reset the queue
 *
 * Deallocates the internal data buffer and resets the queue to its initial state.
 * After calling this, the queue is ready to be reused.
 *
 * @param queue The queue to reset
 */
#define QUEUE_RESET(queue)        \
    do                            \
    {                             \
        QUEUE_FREE((queue).data); \
        (queue).data = NULL;      \
        (queue).front = 0;        \
        (queue).size = 0;         \
        (queue).capacity = 0;     \
    } while (0)

/**
 * @def QUEUE_SIZE(queue)
 * @brief Get the current number of elements
 *
 * Returns the number of elements currently in the queue.
 *
 * @param queue The queue to query
 * @return The number of elements
 */
#define QUEUE_SIZE(queue) ((queue).size)

/**
 * @def QUEUE_CAPACITY(queue)
 * @brief Get the allocated capacity
 *
 * Returns the total allocated buffer size.
 *
 * @param queue The queue to query
 * @return The current capacity
 */
#define QUEUE_CAPACITY(queue) ((queue).capacity)

/**
 * @def QUEUE_IS_EMPTY(queue)
 * @brief Check if the queue is empty
 *
 * Returns true (non-zero) if the queue has no elements.
 *
 * @param queue The queue to check
 * @return Non-zero if empty, 0 otherwise
 */
#define QUEUE_IS_EMPTY(queue) ((queue).size == 0)

/**
 * @def QUEUE_CLEAR(queue)
 * @brief Reset size to zero without deallocating memory
 *
 * Sets the size and front offset to 0, allowing elements to be overwritten
 * without freeing the allocated buffer. The capacity remains unchanged.
 *
 * @param queue The queue to clear
 */
#define QUEUE_CLEAR(queue) \
    do                     \
    {                      \
        (queue).front = 0; \
        (queue).size = 0;  \
    } while (0)

/**
 * @section Testing
 *
 * Define QUEUE_TEST before including this header to compile built-in tests.
 * Example: gcc -DQUEUE_TEST tests.c -o tests && ./tests
 */
#ifdef QUEUE_TEST
#include <stdio.h>
#include <string.h>

QUEUE(int, IntQueue);

int queue_test()
{
    printf("=== Queue Macro Tests ===\n");

    // Test 1: Basic initialization and enqueue
    printf("Test 1: Basic initialization and QUEUE_ENQUEUE\n");
    IntQueue my_queue = {0};
    assert(QUEUE_SIZE(my_queue) == 0);
    assert(QUEUE_CAPACITY(my_queue) == 0);

    for (int i = 0; i < 10; ++i)
    {
        QUEUE_ENQUEUE(my_queue, i);
    }
    assert(QUEUE_SIZE(my_queue) == 10);
    assert(QUEUE_CAPACITY(my_queue) >= 10);

    // Test 2: QUEUE_FRONT
    printf("Test 2: QUEUE_FRONT\n");
    assert(QUEUE_FRONT(my_queue) == 0);

    // Test 3: QUEUE_BACK
    printf("Test 3: QUEUE_BACK\n");
    assert(QUEUE_BACK(my_queue) == 9);

    // Test 4: QUEUE_DEQUEUE
    printf("Test 4: QUEUE_DEQUEUE\n");
    size_t size_before = QUEUE_SIZE(my_queue);
    QUEUE_DEQUEUE(my_queue);
    QUEUE_DEQUEUE(my_queue);
    assert(QUEUE_SIZE(my_queue) == size_before - 2);
    assert(QUEUE_FRONT(my_queue) == 2);

    // Test 5: FIFO order
    printf("Test 5: FIFO order\n");
    QUEUE_CLEAR(my_queue);
    for (int i = 1; i <= 5; ++i)
    {
        QUEUE_ENQUEUE(my_queue, i * 10);
    }
    assert(QUEUE_FRONT(my_queue) == 10);
    QUEUE_DEQUEUE(my_queue);
    assert(QUEUE_FRONT(my_queue) == 20);
    QUEUE_DEQUEUE(my_queue);
    assert(QUEUE_FRONT(my_queue) == 30);

    // Test 6: QUEUE_IS_EMPTY
    printf("Test 6: QUEUE_IS_EMPTY\n");
    assert(!QUEUE_IS_EMPTY(my_queue));
    QUEUE_CLEAR(my_queue);
    assert(QUEUE_IS_EMPTY(my_queue));

    // Test 7: QUEUE_RESET
    printf("Test 7: QUEUE_RESET\n");
    QUEUE_ENQUEUE(my_queue, 42);
    assert(!QUEUE_IS_EMPTY(my_queue));
    QUEUE_RESET(my_queue);
    assert(QUEUE_IS_EMPTY(my_queue));
    assert(QUEUE_SIZE(my_queue) == 0);
    assert(QUEUE_CAPACITY(my_queue) == 0);
    assert(my_queue.data == NULL);

    printf("=== All Queue Tests Passed ===\n");
    return 0;
}

#endif
#endif // QUEUE_H
