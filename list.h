//
// list.h - A lightweight, generic doubly-linked list library for C
//
// USAGE:
//   Define a list node type and list type, then use the macros to manipulate the list.
//
//   Example:
//     #include "list.h"
//
//     LIST_NODE(int, IntNode);
//     LIST(IntNode, IntList);
//
//     int main() {
//         IntList my_list = {NULL};
//         LIST_PUSH_BACK(IntNode, my_list, 42);
//         LIST_PUSH_BACK(IntNode, my_list, 100);
//
//         LIST_FOR_EACH(IntNode, node, my_list) {
//             printf("%d\n", node->value);
//         }
//
//         LIST_RESET(IntNode, my_list);
//         return 0;
//     }
//
// FEATURES:
//   - Type-safe generic doubly-linked lists using C macros
//   - Efficient insertion and deletion at front and back
//   - Forward and reverse iteration
//   - Custom memory allocator support
//
// CUSTOMIZATION:
//   #define LIST_MALLOC to customize allocation (default: malloc)
//   #define LIST_FREE to customize deallocation (default: free)
//   #define LIST_ASSERT to customize assertions (default: assert)
//

#ifndef LIST_H_
#define LIST_H_

#ifndef LIST_MALLOC
#include <stdlib.h>
#define LIST_MALLOC malloc
#endif

#ifndef LIST_FREE
#include <stdlib.h>
#define LIST_FREE free
#endif

#ifndef LIST_ASSERT
#include <assert.h>
#define LIST_ASSERT assert
#endif

// LIST_NODE(type, name) - Define a new list node type
//
// Creates a typedef for a doubly-linked list node that holds elements of the
// specified type.
//
// Example:
//   LIST_NODE(int, IntNode);
#define LIST_NODE(type, name)  \
    typedef struct name##_s    \
    {                          \
        type value;            \
        struct name##_s *next; \
        struct name##_s *prev; \
    } name;

// LIST(node_type, name) - Define a new list type
//
// Creates a typedef for a list structure that manages nodes of the specified
// node type.
//
// Example:
//   LIST_NODE(int, IntNode);
//   LIST(IntNode, IntList);
//   IntList my_list = {NULL};  // Initialize empty
#define LIST(node_type, name) \
    typedef struct            \
    {                         \
        node_type *head;      \
    } name;

// LIST_PUSH_BACK(node_type, list, value) - Add an element to the back of the list
//
// Appends the given value to the end of the list.
#define LIST_PUSH_BACK(node_type, list, value)                             \
    do                                                                     \
    {                                                                      \
        node_type *new_node = (node_type *)LIST_MALLOC(sizeof(node_type)); \
        LIST_ASSERT(new_node != NULL);                                     \
        new_node->value = (value);                                         \
        new_node->next = NULL;                                             \
        new_node->prev = NULL;                                             \
                                                                           \
        if ((list).head == NULL)                                           \
        {                                                                  \
            (list).head = new_node;                                        \
        }                                                                  \
        else                                                               \
        {                                                                  \
            node_type *tail = (list).head;                                 \
            while (tail->next != NULL)                                     \
            {                                                              \
                tail = tail->next;                                         \
            }                                                              \
            tail->next = new_node;                                         \
            new_node->prev = tail;                                         \
        }                                                                  \
    } while (0)

// LIST_PUSH_FRONT(node_type, list, value) - Add an element to the front of the list
//
// Prepends the given value to the beginning of the list.
#define LIST_PUSH_FRONT(node_type, list, value)                            \
    do                                                                     \
    {                                                                      \
        node_type *new_node = (node_type *)LIST_MALLOC(sizeof(node_type)); \
        LIST_ASSERT(new_node != NULL);                                     \
        new_node->value = (value);                                         \
        new_node->next = NULL;                                             \
        new_node->prev = NULL;                                             \
                                                                           \
        if ((list).head != NULL)                                           \
        {                                                                  \
            (list).head->prev = new_node;                                  \
            new_node->next = (list).head;                                  \
        }                                                                  \
        (list).head = new_node;                                            \
    } while (0)

// LIST_POP_BACK(node_type, list) - Remove the last element from the list
//
// Removes and deallocates the element at the end of the list.
// Does nothing if the list is empty.
#define LIST_POP_BACK(node_type, list)     \
    do                                     \
    {                                      \
        if ((list).head != NULL)           \
        {                                  \
            node_type *tail = (list).head; \
            while (tail->next != NULL)     \
            {                              \
                tail = tail->next;         \
            }                              \
            if (tail->prev != NULL)        \
            {                              \
                tail->prev->next = NULL;   \
            }                              \
            else                           \
            {                              \
                (list).head = NULL;        \
            }                              \
            LIST_FREE(tail);               \
        }                                  \
    } while (0)

// LIST_POP_FRONT(node_type, list) - Remove the first element from the list
//
// Removes and deallocates the element at the beginning of the list.
// Does nothing if the list is empty.
#define LIST_POP_FRONT(node_type, list)      \
    do                                       \
    {                                        \
        if ((list).head != NULL)             \
        {                                    \
            node_type *temp = (list).head;   \
            (list).head = (list).head->next; \
            if ((list).head != NULL)         \
            {                                \
                (list).head->prev = NULL;    \
            }                                \
            LIST_FREE(temp);                 \
        }                                    \
    } while (0)

// LIST_FRONT(list) - Get the first element without removing it
//
// Returns a pointer to the first node in the list.
// Returns NULL if the list is empty.
#define LIST_FRONT(list) ((list).head)

// LIST_BACK(node_type, list) - Get the last element without removing it
//
// Returns a pointer to the last node in the list.
// Returns NULL if the list is empty.
#define LIST_BACK(node_type, list) ({ \
    node_type *tail = (list).head;    \
    if (tail != NULL)                 \
    {                                 \
        while (tail->next != NULL)    \
        {                             \
            tail = tail->next;        \
        }                             \
    }                                 \
    tail;                             \
})

// LIST_RESET(node_type, list) - Free all memory and reset the list
//
// Deallocates all nodes and resets the list to its initial state.
// After calling this, the list is ready to be reused.
#define LIST_RESET(node_type, list)       \
    do                                    \
    {                                     \
        node_type *current = (list).head; \
        while (current != NULL)           \
        {                                 \
            node_type *temp = current;    \
            current = current->next;      \
            LIST_FREE(temp);              \
        }                                 \
        (list).head = NULL;               \
    } while (0)

// LIST_SIZE(node_type, list) - Get the current number of elements
//
// Counts and returns the number of elements in the list.
// This is an O(n) operation.
#define LIST_SIZE(node_type, list) ({ \
    size_t count = 0;                 \
    node_type *current = (list).head; \
    while (current != NULL)           \
    {                                 \
        count++;                      \
        current = current->next;      \
    }                                 \
    count;                            \
})

// LIST_IS_EMPTY(list) - Check if the list is empty
//
// Returns true (non-zero) if the list has no elements.
#define LIST_IS_EMPTY(list) ((list).head == NULL)

// LIST_FOR_EACH(node_type, iter, list) - Iterate over all elements in the list
//
// Provides a for-loop to iterate over each node.
// The iterator is a pointer to the node.
//
// Example:
//   LIST_FOR_EACH(IntNode, node, my_list) {
//       printf("%d\n", node->value);
//   }
#define LIST_FOR_EACH(node_type, iter, list) for (node_type *iter = (list).head; iter != NULL; iter = iter->next)

// LIST_FOR_EACH_REVERSE(node_type, iter, list) - Iterate over all elements in reverse order
//
// Provides a for-loop to iterate over each node in reverse.
// The iterator is a pointer to the node.
// Note: Finding the tail is O(n), so this starts from the tail each time.
#define LIST_FOR_EACH_REVERSE(node_type, iter, list) \
    for (node_type *iter = LIST_BACK(node_type, list); iter != NULL; iter = iter->prev)

#endif // LIST_H_
