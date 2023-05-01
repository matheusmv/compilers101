#include "smem.h"

#include <stdlib.h>


void* safe_malloc(size_t size, error_callback cb) {
    void* ptr = NULL;
    ptr = malloc(size);
    if (ptr == NULL) {
        if (cb != NULL) {
            cb("Failed to allocate memory");
        }

        return NULL;
    }

    return ptr;
}

void* safe_calloc(size_t nmemb, size_t size, error_callback cb) {
    void* ptr = NULL;
    ptr = calloc(nmemb, size);
    if (ptr == NULL) {
        if (cb != NULL) {
            cb("Failed to allocate memory");
        }

        return NULL;
    }

    return ptr;
}

void* safe_realloc(void* ptr, size_t size, error_callback cb) {
    void* new_ptr = NULL;
    new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        if (cb != NULL) {
            cb("Failed to reallocate memory");
        }

        return NULL;
    }

    return new_ptr;
}

void safe_free(void** ptr) {
    if (ptr == NULL || *ptr == NULL)
        return;

    free(*ptr);
    *ptr = NULL;
}
