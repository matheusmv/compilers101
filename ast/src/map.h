#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "list.h"


typedef struct MapEntry {
    void* key;
    void* value;
    void (*destroy_key)(void**);
    void (*destroy_value)(void**);
} MapEntry;

MapEntry* map_entry_new(void* key, void* value,
    void (*destroy_key)(void**), void (*destroy_value)(void**));
void map_entry_free(MapEntry** mapEntry);

typedef struct Map {
    size_t total_entries;
    size_t number_of_buckets;
    List** buckets;
    bool (*cmp)(const void**, void**);
    void (*destroy_key)(void**);
    void (*destroy_value)(void**);
} Map;

Map* map_new(size_t number_of_buckets, bool (*cmp)(const void**, void**),
    void (*destroy_key)(void**), void (*destroy_value)(void**));
void map_free(Map** map);

size_t get_index(const char* key, size_t number_of_buckets);

void map_put(Map* map, void* key, void* value);
void* map_get(Map* map, void* key);
void map_remove(Map* map, void* key);
bool map_contains(Map* map, void* key);
size_t map_size(Map* map);
void map_clear(Map* map);
void map_iterate(Map* map, void (*cb)(const void**));


#define MAP_NEW(number_of_buckets, cmp_fn, free_key_fn, free_value_fn)         \
    map_new((number_of_buckets),                                               \
        ((bool (*)(const void**, void**)) cmp_fn),                             \
        ((void (*)(void**)) free_key_fn),                                      \
        ((void (*)(void**)) free_value_fn))


typedef struct MapIterator {
    const Map* map;
    size_t bucket_index;
    ListNode* current_node;
} MapIterator;

MapIterator* map_iterator_new(const Map* map);
void map_iterator_free(MapIterator** iterator);

bool map_iterator_has_next(MapIterator* iterator);
MapEntry* map_iterator_next(MapIterator* iterator);
