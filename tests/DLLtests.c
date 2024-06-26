#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/DLL.h"

void print_data(void* data) {
    printf("%s\n", (char*)data);
}

// Custom free function for tests
void free_data(void* data) {
    free(data);
}

// Custom assertion macros
#define ASSERT_EQ(expected, actual) do { \
    if ((expected) != (actual)) { \
        printf("Assertion failed: %s == %s, file %s, line %d\n", #expected, #actual, __FILE__, __LINE__); \
        exit(1); \
    } \
} while (0)

#define ASSERT_STREQ(expected, actual) do { \
    if (strcmp((expected), (actual)) != 0) { \
        printf("Assertion failed: %s == %s, file %s, line %d\n", #expected, #actual, __FILE__, __LINE__); \
        exit(1); \
    } \
} while (0)

// Function prototypes for test cases
void test_insert_front();
void test_insert_back();
void test_delete_node();
void test_find_node();
void test_traverse_forward();
void test_traverse_backward();

int main() {
    test_insert_front();
    test_insert_back();
    test_delete_node();
    test_find_node();
    test_traverse_forward();
    test_traverse_backward();

    printf("All tests passed.\n");
    return 0;
}

void test_insert_front() {
    DoublyLinkedList* list = create_list();
    char* data = strdup("Hello");
    insert_front(list, data);
    ASSERT_EQ(1, list->size);
    ASSERT_STREQ("Hello", (char*)list->head->data);
    destroy_list(list, free_data);
}

void test_insert_back() {
    DoublyLinkedList* list = create_list();
    char* data = strdup("World");
    insert_back(list, data);
    ASSERT_EQ(1, list->size);
    ASSERT_STREQ("World", (char*)list->tail->data);
    destroy_list(list, free_data);
}

void test_delete_node() {
    DoublyLinkedList* list = create_list();
    char* data1 = strdup("Hello");
    char* data2 = strdup("World");
    insert_back(list, data1);
    insert_back(list, data2);
    Node* node = find_node(list, data1, strcmp);
    delete_node(list, node, free_data);
    ASSERT_EQ(1, list->size);
    ASSERT_STREQ("World", (char*)list->head->data);
    destroy_list(list, free_data);
}

void test_find_node() {
    DoublyLinkedList* list = create_list();
    char* data = strdup("Hello");
    insert_back(list, data);
    Node* node = find_node(list, data, strcmp);
    ASSERT_EQ(node != NULL, 1);
    ASSERT_STREQ("Hello", (char*)node->data);
    destroy_list(list, free_data);
}

void test_traverse_forward() {
    DoublyLinkedList* list = create_list();
    char* data1 = strdup("Hello");
    char* data2 = strdup("World");
    insert_back(list, data1);
    insert_back(list, data2);
    Node* current = list->head;
    ASSERT_STREQ("Hello", (char*)current->data);
    current = current->next;
    ASSERT_STREQ("World", (char*)current->data);
    destroy_list(list, free_data);
}

void test_traverse_backward() {
    DoublyLinkedList* list = create_list();
    char* data1 = strdup("Hello");
    char* data2 = strdup("World");
    insert_back(list, data1);
    insert_back(list, data2);
    Node* current = list->tail;
    ASSERT_STREQ("World", (char*)current->data);
    current = current->prev;
    ASSERT_STREQ("Hello", (char*)current->data);
    destroy_list(list, free_data);
}
