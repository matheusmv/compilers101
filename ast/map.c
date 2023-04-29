#include "map.h"
#include "list.h"

#include <stddef.h>
#include <stdlib.h>


MapEntry* map_entry_new(void* key, void* value,
    void (*destroy_key)(void**), void (*destroy_value)(void**)) {
    MapEntry* entry = NULL;
    entry = malloc(sizeof(MapEntry));
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

    free(*mapEntry);
    *mapEntry = NULL;
}

Map* map_new(size_t size, bool (*cmp)(const void**, void**),
    void (*destroy_key)(void**), void (*destroy_value)(void**)) {
    Map* map = NULL;
    map = malloc(sizeof(Map));
    if (map == NULL) {
        return NULL;
    }

    List** buckets = NULL;
    buckets = calloc(size, sizeof(List*));
    if (buckets == NULL) {
        free(map);
        return NULL;
    }

    for (size_t i = 0; i < size; i++) {
        buckets[i] = list_new((void (*)(void**)) map_entry_free);
    }

    *map = (Map) {
        .size = size,
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

    for (int i = 0; i < (*map)->size; i++) {
        list_free(&((*map)->buckets[i]));
    }

    free((*map)->buckets);
    free(*map);
    *map = NULL;
}

size_t hash(const char* key, size_t size) {
    size_t hash = 5381;

    unsigned char c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;

    return hash % size;
}

inline static void increment_map_total_entries(Map* map) {
    map->total_entries += 1;
}

inline static void decrement_map_total_entries(Map* map) {
    map->total_entries -= 1;
}

void map_put(Map* map, void* key, void* value) {
    size_t index = hash(key, map->size);

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
        list_remove_at(&(map->buckets[index]), object_index, NULL);
        list_insert_at(&(map->buckets[index]), object_index,
            map_entry_new(
                key, value,
                map->destroy_key, map->destroy_value
            )
        );
    }
}

void* map_get(Map* map, void* key) {
    size_t index = hash(key, map->size);

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
    size_t index = hash(key, map->size);

    int object_index = list_find_first(
        &(map->buckets[index]), map->cmp,
        &key,
        NULL
    );

    if (object_index != -1) {
        list_remove_at(
            &(map->buckets[index]),
            object_index,
            NULL
        );
        decrement_map_total_entries(map);
    }
}

bool map_contains(Map* map, void* key) {
    size_t index = hash(key, map->size);

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

    for (size_t i = 0; i < map->size; i++) {
        list_clear(&map->buckets[i]);
    }

    map->total_entries = 0;
}

void map_iterate(Map* map, void (*cb)(const void**)) {
    if (map_size(map) == 0)
        return;

    for (size_t i = 0; i < map->size; i++) {
        list_for_each(&map->buckets[i], cb);
    }
}
