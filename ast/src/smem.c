#include "smem.h"

#include <stdlib.h>


static void invoke_error_callback_if_not_null(error_callback cb, char* message) {
    if (cb != NULL) {
        cb(message);
    }
}

void* safe_malloc(size_t size, error_callback cb) {
    if (size < 1) {
        invoke_error_callback_if_not_null(cb, "invalid size");
        return NULL;
    }

    void* ptr = NULL;
    ptr = malloc(size);
    if (ptr == NULL) {
        invoke_error_callback_if_not_null(cb, "failed to allocate memory");
        return NULL;
    }

    return ptr;
}

void* safe_calloc(size_t nmemb, size_t size, error_callback cb) {
    if (nmemb < 1) {
        invoke_error_callback_if_not_null(cb, "invalid nmemb");
        return NULL;
    }

    if (size < 1) {
        invoke_error_callback_if_not_null(cb, "invalid size");
        return NULL;
    }

    void* ptr = NULL;
    ptr = calloc(nmemb, size);
    if (ptr == NULL) {
        invoke_error_callback_if_not_null(cb, "failed to allocate memory");
        return NULL;
    }

    return ptr;
}

void* safe_realloc(void** ptr, size_t size, error_callback cb) {
    if (ptr == NULL || *ptr == NULL) {
        invoke_error_callback_if_not_null(cb, "invalid ptr");
        return NULL;
    }

    if (size < 1) {
        invoke_error_callback_if_not_null(cb, "invalid size");
        return NULL;
    }

    void* new_ptr = NULL;
    new_ptr = realloc(*ptr, size);
    if (new_ptr == NULL) {
        invoke_error_callback_if_not_null(cb, "failed to reallocate memory");
        return NULL;
    }

    *ptr = NULL;

    return new_ptr;
}

void safe_free(void** ptr) {
    if (ptr == NULL || *ptr == NULL)
        return;

    free(*ptr);
    *ptr = NULL;
}
