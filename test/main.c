#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "vec.h"
#include "queue.h"
#include "stack.h"
#include "list.h"

// Vec types
VEC(int, IntVec);

// Queue types
QUEUE(int, IntQueue);

// Stack types
STACK(int, IntStack);

// List types
LIST_NODE(int, IntNode);
LIST(IntNode, IntList);

int vec_test()
{
    printf("=== Vector Macro Tests ===\n");

    // Test 1: Basic initialization and push
    printf("Test 1: Basic initialization and VEC_PUSH\n");
    IntVec my_vec = {0};
    assert(VEC_SIZE(my_vec) == 0);
    assert(VEC_CAPACITY(my_vec) == 0);

    for (int i = 0; i < 10; ++i)
    {
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
    for (int i = 1; i <= 5; ++i)
    {
        VEC_PUSH(my_vec, i * 10);
    }
    int expected[] = {10, 20, 30, 40, 50};
    int index = 0;
    VEC_FOR_EACH(int, it, my_vec)
    {
        assert(*it == expected[index++]);
    }
    assert(index == 5);

    // Test 10: VEC_RESET
    printf("Test 10: VEC_RESET\n");
    assert(!VEC_IS_EMPTY(my_vec));
    VEC_RESET(my_vec);
    assert(VEC_IS_EMPTY(my_vec));
    assert(VEC_SIZE(my_vec) == 0);
    assert(VEC_CAPACITY(my_vec) == 0);
    assert(my_vec.data == NULL);

    printf("=== All Vector Tests Passed ===\n");
    return 0;
}

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
        int value = i;
        QUEUE_ENQUEUE(my_queue, value);
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

int stack_test()
{
    printf("=== Stack Macro Tests ===\n");

    // Test 1: Basic initialization and push
    printf("Test 1: Basic initialization and STACK_PUSH\n");
    IntStack my_stack = {0};
    assert(STACK_SIZE(my_stack) == 0);
    assert(STACK_CAPACITY(my_stack) == 0);

    for (int i = 0; i < 10; ++i)
    {
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
    for (int i = 1; i <= 5; ++i)
    {
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

#include "raylib.h"

int main()
{
    vec_test();
    queue_test();
    stack_test();
    list_test();

    InitWindow(800, 600, "Klib Test Window");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("All tests passed! Check console output for details.", 10, 10, 20, BLACK);
        EndDrawing();
    }

    return 0;
}
