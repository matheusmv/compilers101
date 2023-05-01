#pragma once

#include <stddef.h>


typedef void (*error_callback)(const char*);

void* safe_malloc(size_t size, error_callback cb);
void* safe_calloc(size_t nmemb, size_t size, error_callback cb);
void* safe_realloc(void* ptr, size_t size, error_callback cb);
void safe_free(void** ptr);
