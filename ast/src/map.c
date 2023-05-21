#include "map.h"

#include <stdbool.h>
#include <stddef.h>

#include "list.h"
#include "smem.h"
#include "utils.h"


MapEntry* map_entry_new(void* key, void* value,
    void (*destroy_key)(void**), void (*destroy_value)(void**)) {
    MapEntry* entry = NULL;
    entry = safe_malloc(sizeof(MapEntry), NULL);
    if (entry == NULL) {
        if (destroy_key != NULL)
            destroy_key(&key);
        if (destroy_value != NULL)
            destroy_value(&value);
        return NULL;
    }

    *entry = (MapEntry) {
        .key = key,
        .value = value,
        .destroy_key = destroy_key,
        .destroy_value = destroy_value
    };

    return entry;
}

void map_entry_free(MapEntry** mapEntry) {
    if (mapEntry == NULL || *mapEntry == NULL)
        return;

    if ((*mapEntry)->destroy_key != NULL)
        (*mapEntry)->destroy_key(&(*mapEntry)->key);

    if ((*mapEntry)->destroy_value != NULL)
        (*mapEntry)->destroy_value(&(*mapEntry)->value);

    safe_free((void**) mapEntry);
}

Map* map_new(size_t number_of_buckets, bool (*cmp)(const void**, void**),
    void (*destroy_key)(void**), void (*destroy_value)(void**)) {
    if (cmp == NULL) {
        return NULL;
    }

    Map* map = NULL;
    map = safe_malloc(sizeof(Map), NULL);
    if (map == NULL) {
        return NULL;
    }

    List** buckets = NULL;
    buckets = safe_calloc(number_of_buckets, sizeof(List*), NULL);
    if (buckets == NULL) {
        safe_free((void**) &map);
        return NULL;
    }

    for (size_t i = 0; i < number_of_buckets; i++) {
        buckets[i] = list_new((void (*)(void**)) map_entry_free);
    }

    *map = (Map) {
        .total_entries = 0,
        .number_of_buckets = number_of_buckets,
        .buckets = buckets,
        .cmp = cmp,
        .destroy_key = destroy_key,
        .destroy_value = destroy_value
    };

    return map;
}

void map_free(Map** map) {
    if (map == NULL || *map == NULL)
        return;

    for (size_t i = 0; i < (*map)->number_of_buckets; i++) {
        list_free(&((*map)->buckets[i]));
    }

    safe_free((void**) &(*map)->buckets);
    safe_free((void**) map);
}

size_t get_index(const char* key, size_t number_of_buckets) {
    return hash_string(key) % number_of_buckets;
}

inline static void increment_map_total_entries(Map* map) {
    map->total_entries += 1;
}

inline static void decrement_map_total_entries(Map* map) {
    map->total_entries -= 1;
}

void map_put(Map* map, void* key, void* value) {
    size_t index = get_index(key, map->number_of_buckets);

    ListNode* object = NULL;
    int object_index = list_find_first(
        &(map->buckets[index]), map->cmp,
        &key,
        (void**) &object
    );

    if (object == NULL) {
        list_insert_last(&(map->buckets[index]),
            map_entry_new(
                key, value,
                map->destroy_key, map->destroy_value
            )
        );
        increment_map_total_entries(map);
    } else {
        list_replace_at(&(map->buckets[index]), object_index,
            map_entry_new(
                key, value,
                map->destroy_key, map->destroy_value
            )
        );
    }
}

void* map_get(Map* map, void* key) {
    size_t index = get_index(key, map->number_of_buckets);

    MapEntry* entry = NULL;
    int object_index = list_find_first(
        &(map->buckets[index]), map->cmp,
        &key,
        (void**) &entry
    );

    if (object_index != -1) {
        return entry->value;
    }

    return NULL;
}

void map_remove(Map* map, void* key) {
    size_t index = get_index(key, map->number_of_buckets);

    bool ok = list_find_and_remove(&(map->buckets[index]), map->cmp, &key);
    if (ok) {
        decrement_map_total_entries(map);
    }
}

bool map_contains(Map* map, void* key) {
    size_t index = get_index(key, map->number_of_buckets);

    void* entry = NULL;
    list_find_first(&(map->buckets[index]), map->cmp, &key, &entry);
    return entry != NULL;
}

size_t map_size(Map* map) {
    return map->total_entries;
}

void map_clear(Map* map) {
    if (map_size(map) == 0)
        return;

    for (size_t i = 0; i < map->number_of_buckets; i++) {
        list_clear(&map->buckets[i]);
    }

    map->total_entries = 0;
}

void map_iterate(Map* map, void (*cb)(const void**)) {
    if (map_size(map) == 0)
        return;

    for (size_t i = 0; i < map->number_of_buckets; i++) {
        list_for_each(&map->buckets[i], cb);
    }
}

MapIterator* map_iterator_new(const Map* map) {
    if (map == NULL) {
        return NULL;
    }

    MapIterator* iterator = NULL;
    iterator = safe_malloc(sizeof(MapIterator), NULL);
    if (iterator == NULL) {
        return NULL;
    }

    *iterator = (MapIterator) {
        .map = map,
        .bucket_index = 0,
        .current_node = NULL
    };

    return iterator;
}

void map_iterator_free(MapIterator** iterator) {
    if (iterator == NULL || *iterator == NULL)
        return;

    safe_free((void**) iterator);
}

bool map_iterator_has_next(MapIterator* iterator) {
    while (iterator->bucket_index < iterator->map->number_of_buckets) {
        List* bucket = iterator->map->buckets[iterator->bucket_index];

        if (iterator->current_node == NULL && !list_is_empty(&bucket)) {
            return true;
        }

        if (iterator->current_node != NULL && iterator->current_node->next != NULL) {
            return true;
        }

        iterator->bucket_index += 1;
        iterator->current_node = NULL;
    }

    return false;
}

MapEntry* map_iterator_next(MapIterator* iterator) {
    if (!map_iterator_has_next(iterator)) {
        return NULL;
    }

    List* bucket = iterator->map->buckets[iterator->bucket_index];

    if (iterator->current_node == NULL) {
        iterator->current_node = bucket->head;
    } else {
        iterator->current_node = iterator->current_node->next;
    }

    return (MapEntry*) iterator->current_node->value;
}
