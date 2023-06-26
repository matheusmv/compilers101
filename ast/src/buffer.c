#include "buffer.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "smem.h"


ByteBuffer* byte_buffer_new(void) {
    return byte_buffer_new_with_capacity(DEFAULT_BB_CAPACITY);
}

ByteBuffer* byte_buffer_new_with_capacity(size_t capacity) {
    ByteBuffer* buffer = NULL;
    buffer = safe_malloc(capacity, NULL);
    if (buffer == NULL) {
        return NULL;
    }

    char* bytes = safe_calloc(1, capacity, NULL);
    if (bytes == NULL) {
        safe_free((void**) &buffer);
        return NULL;
    }

    *buffer = (ByteBuffer) {
        .bytes = bytes,
        .size = 0,
        .capacity = capacity
    };

    return buffer;
}

void byte_buffer_free(ByteBuffer** byteBuffer) {
    if (byteBuffer == NULL || *byteBuffer == NULL)
        return;

    safe_free((void**) &(*byteBuffer)->bytes);
    safe_free((void**) byteBuffer);
}

size_t byte_buffer_size(ByteBuffer* byteBuffer) {
    if (byteBuffer == NULL)
        return 0;

    return byteBuffer->size;
}

size_t byte_buffer_capacity(ByteBuffer* byteBuffer) {
    if (byteBuffer == NULL)
        return 0;

    return byteBuffer->capacity;
}

void byte_buffer_clear(ByteBuffer* byteBuffer) {
    if (byteBuffer == NULL || byteBuffer->bytes == NULL)
        return;

    byteBuffer->bytes[0] = '\0';
    byteBuffer->size = 0;
}

static bool byte_buffer_has_storage_available(ByteBuffer* byteBufer, size_t desiredAmountOfSpace) {
    if (byteBufer == NULL)
        return false;

    size_t availableStorage = byteBufer->capacity - byteBufer->size;

    return desiredAmountOfSpace <= availableStorage;
}

static bool byte_buffer_increase_storage_capacity(ByteBuffer* byteBuffer, size_t totalBytesToIncrease) {
    if (byteBuffer == NULL || totalBytesToIncrease <= 0)
        return false;

    size_t newCapacity = byteBuffer->capacity < totalBytesToIncrease
        ? totalBytesToIncrease * 2
        : byteBuffer->capacity * 2;

    char* newByteBuffer = safe_realloc((void**) &byteBuffer->bytes, newCapacity * sizeof(char), NULL);

    *byteBuffer = (ByteBuffer) {
        .bytes = newByteBuffer,
        .size = byteBuffer->size,
        .capacity = newCapacity,
    };

    return newByteBuffer != NULL;
}

size_t byte_buffer_append(ByteBuffer* byteBuffer, const char* content, size_t contentSize) {
    if (byteBuffer == NULL || content == NULL || contentSize <= 0)
        return 0;

    size_t safeSize = contentSize + 1;

    if (!byte_buffer_has_storage_available(byteBuffer, safeSize)) {
        if (!byte_buffer_increase_storage_capacity(byteBuffer, safeSize)) {
            return 0;
        }
    }

    strncat(byteBuffer->bytes, content, contentSize);

    byteBuffer->size += contentSize;

    return contentSize;
}

size_t byte_buffer_appendf(ByteBuffer* byteBuffer, const char* format, ...) {
    if (byteBuffer == NULL || format == NULL)
        return 0;

    va_list args;
    va_start(args, format);

    va_list args_copy;
    va_copy(args_copy, args);
    int contentSize = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (contentSize < 0) {
        va_end(args);
        return 0;
    }

    char* content = safe_malloc(contentSize + 1, NULL);
    if (content == NULL) {
        va_end(args);
        return 0;
    }

    vsprintf(content, format, args);

    va_end(args);

    size_t bytesUsed = byte_buffer_append(byteBuffer, content, contentSize);

    safe_free((void**) &content);

    return bytesUsed;
}

size_t byte_buffer_nappendf(ByteBuffer* byteBuffer, size_t maxSize, const char* format, ...) {
    if (byteBuffer == NULL || maxSize <= 0 || format == NULL)
        return 0;

    size_t safeSize = maxSize + 1;

    char* content = safe_calloc(1, safeSize, NULL);
    if (content == NULL) {
        return 0;
    }

    va_list args;
    va_start(args, format);

    int contentSize = vsnprintf(content, safeSize, format, args);
    if (contentSize < 0) {
        safe_free((void**) &content);
        va_end(args);
        return 0;
    }

    va_end(args);

    size_t bytesUsed = byte_buffer_append(byteBuffer, content, maxSize);

    safe_free((void**) &content);

    return bytesUsed;
}

char* byte_buffer_to_string(ByteBuffer* byteBuffer) {
    if (byteBuffer == NULL)
        return "";

    size_t bufferSize = byteBuffer->size;

    char* stringBuffer = safe_calloc(1, bufferSize + 1, NULL);
    if (stringBuffer == NULL) {
        return "";
    }

    memmove(stringBuffer, byteBuffer->bytes, bufferSize);
    stringBuffer[bufferSize] = '\0';

    return stringBuffer;
}
