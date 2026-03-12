//
// queue.h - A lightweight, generic queue library for C
//
// USAGE:
//   Define a queue type with a maximum size, then use the macros to manipulate the queue.
//
//   Example:
//     #include "queue.h"
//
//     QUEUE(int, IntQueue, 100);
//
//     int main() {
//         IntQueue my_queue = {0};
//         QUEUE_ENQUEUE(my_queue, 42);
//         QUEUE_ENQUEUE(my_queue, 100);
//
//         while (!QUEUE_IS_EMPTY(my_queue)) {
//             printf("%d\n", QUEUE_FRONT(my_queue));
//             QUEUE_DEQUEUE(my_queue);
//         }
//
//         QUEUE_CLEAR(my_queue);
//         return 0;
//     }
//
// FEATURES:
//   - Type-safe generic queues using C macros
//   - FIFO (First In, First Out) semantics
//   - Fixed-size array-based implementation
//   - No dynamic memory allocation
//
// CUSTOMIZATION:
//   #define QUEUE_ASSERT to customize assertions (default: assert)
//

#ifndef QUEUE_H_
#define QUEUE_H_

#include <string.h>

#ifndef QUEUE_ASSERT
#include <assert.h>
#define QUEUE_ASSERT assert
#endif

// QUEUE(type, name, max_size) - Define a new queue type with fixed maximum size
//
// Creates a typedef for a queue structure that holds up to max_size elements of the specified type.
// Uses a simple array-based implementation with a front offset.
//
// Example:
//   QUEUE(int, IntQueue, 100);
//   IntQueue my_queue = {0};  // Initialize empty
#define QUEUE(type, name, max_size) \
    typedef struct                  \
    {                               \
        type data[max_size];        \
        size_t front;               \
        size_t size;                \
    } name;

// QUEUE_ENQUEUE(queue, value) - Add an element to the back of the queue
//
// Appends the given value to the back of the queue.
// Asserts if the queue is already at maximum capacity.
// Automatically compacts the queue if necessary to reclaim space.
#define QUEUE_ENQUEUE(queue, value)                                                                           \
    do                                                                                                        \
    {                                                                                                         \
        size_t max_cap = sizeof((queue).data) / sizeof(*(queue).data);                                        \
        if ((queue).front + (queue).size >= max_cap)                                                          \
        {                                                                                                     \
            if ((queue).front > 0)                                                                            \
            {                                                                                                 \
                memmove((queue).data, (queue).data + (queue).front, (queue).size * sizeof(*(queue).data));   \
                (queue).front = 0;                                                                            \
            }                                                                                                 \
        }                                                                                                     \
        QUEUE_ASSERT((queue).front + (queue).size < max_cap);                                                 \
        (queue).data[(queue).front + (queue).size] = (value);                                                  \
        (queue).size += 1;                                                                                    \
    } while (0)

// QUEUE_DEQUEUE(queue) - Remove the front element from the queue
//
// Removes the element at the front of the queue.
// Does nothing if the queue is empty.
#define QUEUE_DEQUEUE(queue)  \
    do                        \
    {                         \
        if ((queue).size > 0) \
        {                     \
            (queue).front++;  \
            (queue).size--;   \
        }                     \
    } while (0)

// QUEUE_FRONT(queue) - Get the front element without removing it
//
// Returns the value of the element at the front of the queue.
// Undefined behavior if the queue is empty.
#define QUEUE_FRONT(queue) ((queue).data[(queue).front])

// QUEUE_BACK(queue) - Get the back element without removing it
//
// Returns the value of the element at the back of the queue.
// Undefined behavior if the queue is empty.
#define QUEUE_BACK(queue) ((queue).data[(queue).front + (queue).size - 1])

// QUEUE_RESET(queue) - Reset the queue to empty state
//
// Sets the queue to its initial empty state. No memory deallocation needed for fixed-size queues.
#define QUEUE_RESET(queue)   \
    do                       \
    {                        \
        (queue).front = 0;   \
        (queue).size = 0;    \
    } while (0)

// QUEUE_SIZE(queue) - Get the current number of elements
//
// Returns the number of elements currently in the queue.
#define QUEUE_SIZE(queue) ((queue).size)

// QUEUE_CAPACITY(queue) - Get the maximum capacity
//
// Returns the maximum number of elements that can be stored in the queue.
#define QUEUE_CAPACITY(queue) (sizeof((queue).data) / sizeof(*(queue).data))

// QUEUE_IS_EMPTY(queue) - Check if the queue is empty
//
// Returns true (non-zero) if the queue has no elements.
#define QUEUE_IS_EMPTY(queue) ((queue).size == 0)

// QUEUE_CLEAR(queue) - Reset size to zero without deallocating memory
//
// Sets the size and front offset to 0, allowing elements to be overwritten
// without freeing the allocated buffer. The capacity remains unchanged.
#define QUEUE_CLEAR(queue) \
    do                     \
    {                      \
        (queue).front = 0; \
        (queue).size = 0;  \
    } while (0)

#endif // QUEUE_H_
