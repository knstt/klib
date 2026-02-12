/**
 * @file list.h
 * @brief A lightweight, generic doubly-linked list library for C
 *
 * This header-only library provides type-safe generic doubly-linked lists using C macros.
 * Lists support efficient insertion and deletion at any position.
 *
 * @author Konstantin Eid
 *
 * Usage Example:
 * ```c
 * #include "list.h"
 *
 * // Define a list node type for integers
 * LIST_NODE(int, IntNode);
 *
 * // Define a list type that uses the node
 * LIST(IntNode, IntList);
 *
 * int main() {
 *     IntList my_list = {NULL};
 *     LIST_PUSH_BACK(IntNode, my_list, 42);
 *     LIST_PUSH_BACK(IntNode, my_list, 100);
 *
 *     LIST_FOR_EACH(IntNode, node, my_list) {
 *         printf("%d\n", node->value);
 *     }
 *
 *     LIST_RESET(IntNode, my_list);
 *     return 0;
 * }
 * ```
 */

#ifndef LIST_H
#define LIST_H

/**
 * @brief Custom memory allocation function
 *
 * Define LIST_MALLOC before including list.h to use a custom malloc function.
 * Default: standard library malloc
 */
#ifndef LIST_MALLOC
#include <stdlib.h>
#define LIST_MALLOC malloc
#endif

/**
 * @brief Custom memory deallocation function
 *
 * Define LIST_FREE before including list.h to use a custom free function.
 * Default: standard library free
 */
#ifndef LIST_FREE
#include <stdlib.h>
#define LIST_FREE free
#endif

/**
 * @brief Custom assertion function
 *
 * Define LIST_ASSERT before including list.h to use a custom assert function.
 * Default: standard library assert
 */
#ifndef LIST_ASSERT
#include <assert.h>
#define LIST_ASSERT assert
#endif

/**
 * @def LIST_NODE(type, name)
 * @brief Define a new list node type
 *
 * Creates a typedef for a doubly-linked list node that holds elements of the specified type.
 *
 * @param type The element type (e.g., int, double, struct MyStruct)
 * @param name The name of the resulting typedef
 *
 * Example:
 * ```c
 * LIST_NODE(int, IntNode);
 * ```
 */
#define LIST_NODE(type, name) \
    typedef struct name##_s {      \
        type value;                \
        struct name##_s* next;     \
        struct name##_s* prev;     \
    } name;

/**
 * @def LIST(node_type, name)
 * @brief Define a new list type
 *
 * Creates a typedef for a list structure that manages nodes of the specified node type.
 *
 * @param node_type The node type (created with LIST_NODE)
 * @param name The name of the resulting typedef
 *
 * Example:
 * ```c
 * LIST_NODE(int, IntNode);
 * LIST(IntNode, IntList);
 * IntList my_list = {NULL};  // Initialize empty
 * ```
 */
#define LIST(node_type, name) \
    typedef struct            \
    {                         \
        node_type *head;      \
    } name;

/**
 * @def LIST_PUSH_BACK(node_type, list, value)
 * @brief Add an element to the back of the list
 *
 * Appends the given value to the end of the list.
 *
 * @param node_type The node type
 * @param list The list to modify
 * @param value The value to add
 */
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

/**
 * @def LIST_PUSH_FRONT(node_type, list, value)
 * @brief Add an element to the front of the list
 *
 * Prepends the given value to the beginning of the list.
 *
 * @param node_type The node type
 * @param list The list to modify
 * @param value The value to add
 */
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

/**
 * @def LIST_POP_BACK(node_type, list)
 * @brief Remove the last element from the list
 *
 * Removes and deallocates the element at the end of the list.
 * Does nothing if the list is empty.
 *
 * @param node_type The node type
 * @param list The list to modify
 */
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

/**
 * @def LIST_POP_FRONT(node_type, list)
 * @brief Remove the first element from the list
 *
 * Removes and deallocates the element at the beginning of the list.
 * Does nothing if the list is empty.
 *
 * @param node_type The node type
 * @param list The list to modify
 */
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

/**
 * @def LIST_FRONT(list)
 * @brief Get the first element without removing it
 *
 * Returns a pointer to the first node in the list.
 * Returns NULL if the list is empty.
 *
 * @param list The list to read from
 * @return Pointer to the front node or NULL
 */
#define LIST_FRONT(list) ((list).head)

/**
 * @def LIST_BACK(node_type, list)
 * @brief Get the last element without removing it
 *
 * Returns a pointer to the last node in the list.
 * Returns NULL if the list is empty.
 *
 * @param node_type The node type
 * @param list The list to read from
 * @return Pointer to the back node or NULL
 */
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

/**
 * @def LIST_RESET(node_type, list)
 * @brief Free all memory and reset the list
 *
 * Deallocates all nodes and resets the list to its initial state.
 * After calling this, the list is ready to be reused.
 *
 * @param node_type The node type
 * @param list The list to reset
 */
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

/**
 * @def LIST_SIZE(node_type, list)
 * @brief Get the current number of elements
 *
 * Counts and returns the number of elements in the list.
 * This is O(n) operation.
 *
 * @param node_type The node type
 * @param list The list to query
 * @return The number of elements
 */
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

/**
 * @def LIST_IS_EMPTY(list)
 * @brief Check if the list is empty
 *
 * Returns true (non-zero) if the list has no elements.
 *
 * @param list The list to check
 * @return Non-zero if empty, 0 otherwise
 */
#define LIST_IS_EMPTY(list) ((list).head == NULL)

/**
 * @def LIST_FOR_EACH(node_type, iter, list)
 * @brief Iterate over all elements in the list
 *
 * Provides a for-loop to iterate over each node.
 * The iterator is a pointer to the node.
 *
 * Example:
 * ```c
 * LIST_FOR_EACH(IntNode, node, my_list) {
 *     printf("%d\n", node->value);
 * }
 * ```
 *
 * @param node_type The node type
 * @param iter The loop variable (pointer to node)
 * @param list The list to iterate over
 */
#define LIST_FOR_EACH(node_type, iter, list) for (node_type *iter = (list).head; iter != NULL; iter = iter->next)

/**
 * @def LIST_FOR_EACH_REVERSE(node_type, iter, list)
 * @brief Iterate over all elements in reverse order
 *
 * Provides a for-loop to iterate over each node in reverse.
 * The iterator is a pointer to the node.
 * Note: Finding the tail is O(n), so this starts from the tail each time.
 *
 * @param node_type The node type
 * @param iter The loop variable (pointer to node)
 * @param list The list to iterate over
 */
#define LIST_FOR_EACH_REVERSE(node_type, iter, list) \
    for (node_type *iter = LIST_BACK(node_type, list); iter != NULL; iter = iter->prev)

/**
 * @section Testing
 *
 * Define LIST_TEST before including this header to compile built-in tests.
 * Example: gcc -DLIST_TEST tests.c -o tests && ./tests
 */
#ifdef LIST_TEST
#include <stdio.h>

LIST_NODE(int, IntNode);
LIST(IntNode, IntList);

int list_test()
{
    printf("=== List Macro Tests ===\n");

    // Test 1: Basic initialization and push_back
    printf("Test 1: Basic initialization and LIST_PUSH_BACK\n");
    IntList my_list = {NULL};
    assert(LIST_IS_EMPTY(my_list));

    for (int i = 0; i < 5; ++i)
    {
        int value = i * 10;
        LIST_PUSH_BACK(IntNode, my_list, value);
    }
    assert(!LIST_IS_EMPTY(my_list));
    assert(LIST_SIZE(IntNode, my_list) == 5);

    // Test 2: LIST_FRONT and LIST_BACK
    printf("Test 2: LIST_FRONT and LIST_BACK\n");
    assert(LIST_FRONT(my_list)->value == 0);
    assert(LIST_BACK(IntNode, my_list)->value == 40);

    // Test 3: LIST_PUSH_FRONT
    printf("Test 3: LIST_PUSH_FRONT\n");
    int value = -10;
    LIST_PUSH_FRONT(IntNode, my_list, value);
    assert(LIST_FRONT(my_list)->value == -10);
    assert(LIST_SIZE(IntNode, my_list) == 6);

    // Test 4: LIST_POP_FRONT
    printf("Test 4: LIST_POP_FRONT\n");
    LIST_POP_FRONT(IntNode, my_list);
    assert(LIST_FRONT(my_list)->value == 0);
    assert(LIST_SIZE(IntNode, my_list) == 5);

    // Test 5: LIST_POP_BACK
    printf("Test 5: LIST_POP_BACK\n");
    LIST_POP_BACK(IntNode, my_list);
    assert(LIST_BACK(IntNode, my_list)->value == 30);
    assert(LIST_SIZE(IntNode, my_list) == 4);

    // Test 6: LIST_FOR_EACH
    printf("Test 6: LIST_FOR_EACH\n");
    int expected[] = {0, 10, 20, 30};
    int index = 0;
    LIST_FOR_EACH(IntNode, node, my_list)
    {
        assert(node->value == expected[index++]);
    }
    assert(index == 4);

    // Test 7: LIST_FOR_EACH_REVERSE
    printf("Test 7: LIST_FOR_EACH_REVERSE\n");
    index = 3;
    LIST_FOR_EACH_REVERSE(IntNode, node, my_list)
    {
        assert(node->value == expected[index--]);
    }
    assert(index == -1);

    // Test 8: LIST_RESET
    printf("Test 8: LIST_RESET\n");
    assert(!LIST_IS_EMPTY(my_list));
    LIST_RESET(IntNode, my_list);
    assert(LIST_IS_EMPTY(my_list));
    assert(LIST_SIZE(IntNode, my_list) == 0);
    assert(my_list.head == NULL);

    printf("=== All List Tests Passed ===\n");
    return 0;
}

#endif
#endif // LIST_H
