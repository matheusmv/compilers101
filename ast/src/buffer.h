#pragma once

#include <stddef.h>


#define DEFAULT_BB_CAPACITY 256

typedef struct ByteBuffer {
    char* bytes;
    size_t size;
    size_t capacity;
} ByteBuffer;

ByteBuffer* byte_buffer_new(void);
ByteBuffer* byte_buffer_new_with_capacity(size_t capacity);
void byte_buffer_free(ByteBuffer** byteBuffer);

size_t byte_buffer_size(ByteBuffer* byteBuffer);
size_t byte_buffer_capacity(ByteBuffer* byteBuffer);

void byte_buffer_clear(ByteBuffer* byteBuffer);

size_t byte_buffer_append(ByteBuffer* byteBuffer, const char* content, size_t contentSize);
size_t byte_buffer_appendf(ByteBuffer* byteBuffer, const char* format, ...);
size_t byte_buffer_nappendf(ByteBuffer* byteBuffer, size_t maxSize, const char* format, ...);

char* byte_buffer_to_string(ByteBuffer* byteBuffer);
