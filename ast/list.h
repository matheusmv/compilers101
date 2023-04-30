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

void list_clear(List** list);

void* list_get_at(List** list, size_t index);

void list_replace_at(List** list, size_t index, void* object);

void list_for_each(List** list, void (*cb)(const void**));
int list_find_first(List** list,
    bool (*cb)(const void**, void**), void** key, void** found_object);
bool list_find_and_remove(List** list,
    bool (*cb)(const void**, void**), void** key);
List* list_map(List** list, void* (*cb)(void**), void (*new_destructor)(void**));
char* list_join_str(List** list, const char* delimiter,
    char* (*to_string)(const void**),
    void (*free_str)(char**));


#define list_foreach(item, list)                                               \
    for(ListNode *(item) = (list)->head; (item) != NULL; (item) = (item)->next)
