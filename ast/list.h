#pragma once

#include <stdbool.h>
#include <stddef.h>


typedef struct ListNode {
    void* value;
    struct ListNode* prev;
    struct ListNode* next;
} ListNode;

ListNode* list_node_new(void* value, ListNode* prev, ListNode* next);
void list_node_free(ListNode** node, void (*destroy)(void**));


typedef struct List {
    size_t size;
    ListNode* head;
    ListNode* tail;
    void (*destroy)(void**);
} List;

List* list_new(void (*destroy)(void**));
void list_free(List** list);

size_t list_size(List** list);
bool list_is_empty(List** list);

void list_insert_first(List** list, void* object);
void list_insert_last(List** list, void* object);
void list_insert_at(List** list, size_t index, void* object);

void list_remove_first(List** list, void** return_buffer);
void list_remove_last(List** list, void** return_buffer);
void list_remove_at(List** list, size_t index, void** return_buffer);

void* list_get_at(List** list, size_t index);

void list_for_each(List** list, void (*cb)(const void**));
void* list_find_first(List** list, bool (*cb)(const void**));
char* list_join_str(List** list, const char* delimiter,
    char* (*to_string)(const void**),
    void (*free_str)(char**));
