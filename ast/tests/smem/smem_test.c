#include "smem_test.h"

#include <stdio.h>
#include <assert.h>

#include "../../src/smem.h"


static void test_safe_malloc(void) {
    void* ptr = safe_malloc(10, NULL);
    assert(ptr != NULL);

    safe_free(&ptr);
    assert(ptr == NULL);

    void* ptr2 = safe_malloc(0, NULL);
    assert(ptr2 == NULL);
}

static void test_safe_calloc(void) {
    void* ptr = safe_calloc(10, sizeof(int), NULL);
    assert(ptr != NULL);

    int* p = (int*) ptr;
    for (int i = 0; i < 10; i++) {
        assert(p[i] == 0);
    }

    safe_free(&ptr);
    assert(ptr == NULL);

    void* ptr2 = safe_calloc(0, sizeof(int), NULL);
    assert(ptr2 == NULL);
}

static void test_safe_realloc(void) {
    void* ptr = safe_malloc(10, NULL);
    void* new_ptr = safe_realloc(&ptr, 20, NULL);
    assert(ptr == NULL);
    assert(new_ptr != NULL);

    safe_free(&new_ptr);
    assert(new_ptr == NULL);

    void* ptr2 = safe_realloc(NULL, 20, NULL);
    assert(ptr2 == NULL);
}

static void test_safe_free(void) {
    void* ptr = safe_malloc(10, NULL);
    safe_free(&ptr);
    assert(ptr == NULL);

    void* ptr2 = NULL;
    safe_free(&ptr2);
}

void run_smem_tests(void) {
    test_safe_malloc();
    test_safe_calloc();
    test_safe_realloc();
    test_safe_free();

    printf("%s: All tests passed successfully!\n", __FILE__);
}
