#include "buffer_test.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/buffer.h"


static void test_byte_buffer_new(void) {
    ByteBuffer* buffer = byte_buffer_new();

    assert(buffer != NULL);
    assert(buffer->bytes != NULL);
    assert(buffer->size == 0);
    assert(buffer->capacity == DEFAULT_BB_CAPACITY);

    byte_buffer_free(&buffer);
}

static void test_byte_buffer_new_with_capacity(void) {
    size_t capacity = 100;
    ByteBuffer* buffer = byte_buffer_new_with_capacity(capacity);

    assert(buffer != NULL);
    assert(buffer->bytes != NULL);
    assert(buffer->size == 0);
    assert(buffer->capacity == capacity);

    byte_buffer_free(&buffer);
}

static void test_byte_buffer_clear(void) {
    ByteBuffer* buffer = byte_buffer_new();

    assert(byte_buffer_append(buffer, "Hello, ", 7) == 7);
    assert(byte_buffer_append(buffer, "world!", 6) == 6);

    byte_buffer_clear(buffer);

    assert(byte_buffer_size(buffer) == 0);
    assert(buffer->bytes[0] == '\0');

    byte_buffer_free(&buffer);
}

static void test_byte_buffer_append(void) {
    ByteBuffer* buffer = byte_buffer_new();

    assert(byte_buffer_append(buffer, "Hello, ", 7) == 7);
    assert(byte_buffer_append(buffer, "world!", 6) == 6);

    assert(byte_buffer_size(buffer) == 13);

    char* content = byte_buffer_to_string(buffer);

    assert(strcmp(content, "Hello, world!") == 0);

    free(content);
    byte_buffer_free(&buffer);
}

static void test_byte_buffer_appendf(void) {
    ByteBuffer* buffer = byte_buffer_new();

    assert(byte_buffer_appendf(buffer, "%s, %s!", "Hello", "world") == 13);

    assert(byte_buffer_size(buffer) == 13);

    char* content = byte_buffer_to_string(buffer);

    assert(strcmp(content, "Hello, world!") == 0);

    free(content);
    byte_buffer_free(&buffer);
}

static void test_byte_buffer_nappendf(void) {
    ByteBuffer* buffer = byte_buffer_new();

    assert(byte_buffer_nappendf(buffer, 6, "%s, %s!", "Hello", "world") == 6);

    assert(byte_buffer_size(buffer) == 6);

    char* content = byte_buffer_to_string(buffer);

    assert(strcmp(content, "Hello,") == 0);

    free(content);
    byte_buffer_free(&buffer);
}

static void test_byte_buffer_to_string(void) {
    ByteBuffer* buffer = byte_buffer_new();

    byte_buffer_append(buffer, "Hello, ", 7);
    byte_buffer_append(buffer, "world!", 6);

    char* str = byte_buffer_to_string(buffer);

    assert(strcmp(str, "Hello, world!") == 0);

    free(str);

    byte_buffer_clear(buffer);

    int number = 42;
    byte_buffer_appendf(buffer, "The answer is %d", number);

    str = byte_buffer_to_string(buffer);

    assert(strcmp(str, "The answer is 42") == 0);

    free(str);

    byte_buffer_free(&buffer);
}

void run_buffer_tests(void) {
    test_byte_buffer_new();
    test_byte_buffer_new_with_capacity();
    test_byte_buffer_clear();
    test_byte_buffer_append();
    test_byte_buffer_appendf();
    test_byte_buffer_nappendf();
    test_byte_buffer_to_string();

    printf("%s: All tests passed successfully!\n", __FILE__);
}
