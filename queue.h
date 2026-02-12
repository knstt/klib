//
// queue.h - A lightweight, generic queue library for C
//
// USAGE:
//   Define a queue type, then use the macros to manipulate the queue.
//
//   Example:
//     #include "queue.h"
//
//     QUEUE(int, IntQueue);
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
//         QUEUE_RESET(my_queue);
//         return 0;
//     }
//
// FEATURES:
//   - Type-safe generic queues using C macros
//   - FIFO (First In, First Out) semantics
//   - Dynamic array-based implementation with automatic compaction
//   - Custom memory allocator support
//
// CUSTOMIZATION:
//   #define QUEUE_REALLOC to customize allocation (default: realloc)
//   #define QUEUE_FREE to customize deallocation (default: free)
//   #define QUEUE_ASSERT to customize assertions (default: assert)
//

#ifndef QUEUE_H_
#define QUEUE_H_

#include <string.h>

#ifndef QUEUE_REALLOC
#include <stdlib.h>
#define QUEUE_REALLOC realloc
#endif

#ifndef QUEUE_FREE
#include <stdlib.h>
#define QUEUE_FREE free
#endif

#ifndef QUEUE_ASSERT
#include <assert.h>
#define QUEUE_ASSERT assert
#endif

// QUEUE(type, name) - Define a new queue type
//
// Creates a typedef for a queue structure that holds elements of the specified type.
// Uses a simple dynamic array-based implementation with a front offset.
//
// Example:
//   QUEUE(int, IntQueue);
//   IntQueue my_queue = {0};  // Initialize empty
#define QUEUE(type, name) \
    typedef struct        \
    {                     \
        type *data;       \
        size_t front;     \
        size_t size;      \
        size_t capacity;  \
    } name;

// QUEUE_ENQUEUE(queue, value) - Add an element to the back of the queue
//
// Appends the given value to the back of the queue, doubling the capacity if
// necessary. Also compacts the queue if too much space is wasted at the front.
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
                                                                                                           \
                memmove((queue).data, (queue).data + (queue).front, (queue).size * sizeof(*(queue).data)); \
                (queue).front = 0;                                                                         \
            }                                                                                              \
            else                                                                                           \
            {                                                                                              \
                (queue).capacity *= 2;                                                                     \
                (queue).data = QUEUE_REALLOC((queue).data, (queue).capacity * sizeof(*(queue).data));      \
            }                                                                                              \
        }                                                                                                  \
        (queue).data[(queue).front + (queue).size] = (value);                                              \
        (queue).size += 1;                                                                                 \
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

// QUEUE_RESET(queue) - Free all memory and reset the queue
//
// Deallocates the internal data buffer and resets the queue to its initial state.
// After calling this, the queue is ready to be reused.
#define QUEUE_RESET(queue)        \
    do                            \
    {                             \
        QUEUE_FREE((queue).data); \
        (queue).data = NULL;      \
        (queue).front = 0;        \
        (queue).size = 0;         \
        (queue).capacity = 0;     \
    } while (0)

// QUEUE_SIZE(queue) - Get the current number of elements
//
// Returns the number of elements currently in the queue.
#define QUEUE_SIZE(queue) ((queue).size)

// QUEUE_CAPACITY(queue) - Get the allocated capacity
//
// Returns the total allocated buffer size.
#define QUEUE_CAPACITY(queue) ((queue).capacity)

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
