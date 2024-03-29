#include "list.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "smem.h"


ListNode* list_node_new(void* value, ListNode* prev, ListNode* next) {
    ListNode* new_node = NULL;
    new_node = safe_malloc(sizeof(ListNode), NULL);
    if (new_node == NULL) {
        return NULL;
    }

    *new_node = (ListNode) {
        .value = value,
        .prev = prev,
        .next = next
    };

    return new_node;
}

void list_node_free(ListNode** node, void (*destroy)(void**)) {
    if (node == NULL || *node == NULL)
        return;

    if (destroy != NULL)
        destroy(&(*node)->value);

    safe_free((void**) node);
}

List* list_new(void (*destroy)(void**)) {
    List* new_list = NULL;
    new_list = safe_calloc(1, sizeof(List), NULL);
    if (new_list == NULL) {
        return NULL;
    }

    *new_list = (List) {
        .destroy = destroy
    };

    return new_list;
}

void list_free(List** list) {
    if (list == NULL || *list == NULL)
        return;

    while (!list_is_empty(list)) {
        list_remove_first(list, NULL);
    }

    safe_free((void**) list);
}

size_t list_size(List** list) {
    return (*list)->size;
}

bool list_is_empty(List** list) {
    if (list == NULL || *list == NULL)
        return true;

    return (*list)->size == 0;
}

static inline bool list_is_initialized(List** list) {
    return list != NULL && *list != NULL;
}

static inline void increase_list_size(List** list) {
    (*list)->size += 1;
}

static inline void decrease_list_size(List** list) {
    (*list)->size -= 1;
}

static ListNode* get_node(List** list, size_t index) {
    if (list_is_empty(list) || index >= list_size(list))
        return NULL;

    ListNode* node = (*list)->head;

    for (size_t i = 0; i < index; i++) {
        node = node->next;
    }

    return node;
}

void list_insert_first(List** list, void* object) {
    if (!list_is_initialized(list))
        return;

    ListNode* new_node = list_node_new(object, NULL, (*list)->head);
    if (new_node == NULL)
        return;

    if (!list_is_empty(list))
        (*list)->head->prev = new_node;

    if ((*list)->tail == NULL)
        (*list)->tail = new_node;

    (*list)->head = new_node;

    increase_list_size(list);
}

void list_insert_last(List** list, void* object) {
    if (!list_is_initialized(list))
        return;

    ListNode* new_node = list_node_new(object, (*list)->tail, NULL);
    if (new_node == NULL)
        return;

    if (!list_is_empty(list))
        (*list)->tail->next = new_node;

    if ((*list)->head == NULL)
        (*list)->head = new_node;

    (*list)->tail = new_node;

    increase_list_size(list);
}

void list_insert_at(List** list, size_t index, void* object) {
    if (!list_is_initialized(list) || index > list_size(list))
        return;

    if (index == 0) {
        list_insert_first(list, object);
        return;
    }

    if (index == (list_size(list) - 1)) {
        list_insert_last(list, object);
        return;
    }

    ListNode* current = get_node(list, index);
    ListNode* previous = current->prev;

    ListNode* new_node = list_node_new(object, previous, current);
    if (new_node == NULL)
        return;

    if (previous != NULL)
        previous->next = new_node;

    if (current != NULL)
        current->prev = new_node;

    increase_list_size(list);
}

void list_remove_first(List** list, void** return_buffer) {
    if (!list_is_initialized(list) || list_is_empty(list))
        return;

    ListNode* head = (*list)->head;

    if (list_size(list) == 1) {
        (*list)->head = NULL;
        (*list)->tail = NULL;
    } else {
        (*list)->head = head->next;
        (*list)->head->prev = NULL;
    }

    decrease_list_size(list);

    if (return_buffer != NULL && *return_buffer == NULL) {
        *return_buffer = head->value;
        list_node_free(&head, NULL);
    } else {
        list_node_free(&head, (*list)->destroy);
    }
}

void list_remove_last(List** list, void** return_buffer) {
    if (!list_is_initialized(list) || list_is_empty(list))
        return;

    ListNode* tail = (*list)->tail;

    if (list_size(list) == 1) {
        (*list)->head = NULL;
        (*list)->tail = NULL;
    } else {
        (*list)->tail = tail->prev;
        (*list)->tail->next = NULL;
    }

    decrease_list_size(list);

    if (return_buffer != NULL && *return_buffer == NULL) {
        *return_buffer = tail->value;
        list_node_free(&tail, NULL);
    } else {
        list_node_free(&tail, (*list)->destroy);
    }
}

void list_remove_at(List** list, size_t index, void** return_buffer) {
    if (!list_is_initialized(list) || list_is_empty(list) || index > list_size(list))
        return;

    if (index == 0) {
        list_remove_first(list, return_buffer);
        return;
    }

    if (index == (list_size(list) - 1)) {
        list_remove_last(list, return_buffer);
        return;
    }

    ListNode* current = get_node(list, index);
    ListNode* previous = current->prev;
    ListNode* next = current->next;

    if (previous != NULL)
        previous->next = next;

    if (next != NULL)
        next->prev = previous;

    decrease_list_size(list);

    if (return_buffer != NULL && *return_buffer == NULL) {
        *return_buffer = current->value;
        list_node_free(&current, NULL);
    } else {
        list_node_free(&current, (*list)->destroy);
    }
}

static void split_list(ListNode* source, ListNode** front, ListNode** back) {
    ListNode* slow = source;
    ListNode* fast = source->next;

    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front = source;
    *back = slow->next;
    slow->next = NULL;

    if (*back != NULL) {
        (*back)->prev = NULL;
    }
}

static ListNode* merge(ListNode* left, ListNode* right, int (*cmp)(const void*, const void*)) {
    if (left == NULL) {
        return right;
    } else if (right == NULL) {
        return left;
    }

    ListNode* result = NULL;

    if (cmp(left->value, right->value) <= 0) {
        result = left;
        result->next = merge(left->next, right, cmp);
        result->next->prev = result;
    } else {
        result = right;
        result->next = merge(left, right->next, cmp);
        result->next->prev = result;
    }

    return result;
}

static void merge_sort(ListNode** head_ref, int (*cmp)(const void*, const void*)) {
    if (head_ref == NULL || *head_ref == NULL || (*head_ref)->next == NULL)
        return;

    (*head_ref)->prev = NULL;
    ListNode* head = *head_ref;
    ListNode* left = NULL;
    ListNode* right = NULL;

    split_list(head, &left, &right);

    merge_sort(&left, cmp);
    merge_sort(&right, cmp);

    *head_ref = merge(left, right, cmp);
}

void list_sort(List** list, int (*cmp)(const void*, const void*)) {
    if (!list_is_initialized(list) || list_size(list) < 2 || cmp == NULL)
        return;

    merge_sort(&(*list)->head, cmp);

    ListNode* tail = (*list)->head;
    while (tail->next != NULL) {
        tail = tail->next;
    }
    (*list)->tail = tail;
}

void list_clear(List** list) {
    if (!list_is_initialized(list) || list_is_empty(list))
        return;

    while (!list_is_empty(list)) {
        list_remove_first(list, NULL);
    }
}

void* list_get_at(List** list, size_t index) {
    if (!list_is_initialized(list) || list_is_empty(list) || index > list_size(list))
        return NULL;

    return get_node(list, index)->value;
}

void list_replace_at(List** list, size_t index, void* object) {
    if (list == NULL || *list == NULL || object == NULL)
        return;

    ListNode* old_node = get_node(list, index);
    if (old_node != NULL) {
        void* old_value = old_node->value;

        old_node->value = object;

        if ((*list)->destroy != NULL)
            (*list)->destroy(&old_value);
    }
}

void list_for_each(List** list, void (*cb)(const void**)) {
    if (list == NULL || *list == NULL || cb == NULL)
        return;

    for (ListNode* node = (*list)->head; node != NULL; node = node->next) {
        cb((const void**) &node->value);
    }
}

int list_find_first(List** list, bool (*cb)(const void**, void**), void** key, void** found_object) {
    if (list == NULL || *list == NULL || cb == NULL || key == NULL)
        return -1;

    int index = 0;
    for (ListNode* node = (*list)->head; node != NULL; node = node->next) {
        if (cb((const void**) &node->value, key)) {
            if (found_object != NULL) {
                *found_object = node->value;
            }
            return index;
        }
        index++;
    }

    return -1;
}

bool list_find_and_remove(List** list, bool (*cb)(const void**, void**), void** key) {
    if (list == NULL || *list == NULL || cb == NULL || key == NULL)
        return false;

    for (ListNode* node = (*list)->head; node != NULL; node = node->next) {
        if (cb((const void**) &node->value, key)) {
            ListNode* prev = node->prev;
            ListNode* next = node->next;

            if (node == (*list)->head)
                (*list)->head = next;

            if (prev != NULL)
                prev->next = next;

            if (next != NULL)
                next->prev = prev;

            list_node_free(&node, (*list)->destroy);
            
            decrease_list_size(list);
            
            return true;
        }
    }

    return false;
}

List* list_map(List** list, void* (*cb)(void**), void (*new_destructor)(void**)) {
    if (list == NULL || *list == NULL || cb == NULL)
        return NULL;

    void (*destroy)(void**) = new_destructor != NULL ? new_destructor : (*list)->destroy;

    List* new_list = list_new(destroy);
    if (new_list == NULL) {
        return NULL;
    }

    for (ListNode* node = (*list)->head; node != NULL; node = node->next) {
        list_insert_last(&new_list, cb(&node->value));
    }

    return new_list;
}

char* list_join_str(List** list, const char* delimiter, char* (*to_string)(const void**), void (*free_str)(char**)) {
    if (list == NULL || *list == NULL || delimiter == NULL || to_string == NULL) {
        return NULL;
    }

    List* objects_as_strings = list_new((void (*)(void**)) free_str);
    if (objects_as_strings == NULL) {
        return NULL;
    }

    size_t str_len = 0;
    for (ListNode* node = (*list)->head; node != NULL; node = node->next) {
        char* object_as_string = to_string((const void**) &node->value);
        if (object_as_string == NULL) {
            list_free(&objects_as_strings);
            return NULL;
        }

        list_insert_last(&objects_as_strings, object_as_string);
        str_len += strlen(object_as_string);
    }

    size_t delimiter_len = strlen(delimiter);
    if (list_size(&objects_as_strings) > 1) {
        str_len += (list_size(&objects_as_strings) - 1) * delimiter_len;
    }

    char* result = safe_malloc((str_len + 1), NULL);
    if (result == NULL) {
        list_free(&objects_as_strings);
        return NULL;
    }

    size_t i = 0;
    for (ListNode* node = objects_as_strings->head; node != NULL; node = node->next) {
        strcpy(result + i, (const char*) node->value);
        i += strlen((const char*) node->value);

        if (node->next != NULL) {
            strcpy(result + i, delimiter);
            i += delimiter_len;
        }
    }
    result[str_len] = '\0';

    list_free(&objects_as_strings);

    return result;
}
