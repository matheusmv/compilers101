#include "utils_test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../../src/utils.h"


static void test_str_dup(void) {
    char* str1 = "Hello, world!";
    char* str2 = str_dup(str1);

    assert(str1 != str2);
    assert(strcmp(str1, str2) == 0);

    free(str2);
}

static void test_hash_int(void) {
    int value = 123456789;
    unsigned long expected_hash = 77274515265433293;
    unsigned long hash = hash_int(value);

    assert(hash == expected_hash);
}

static void test_hash_float(void) {
    float value = 3.1415926535f;
    unsigned long expected_hash = 673759060557230911;
    unsigned long hash = hash_float(value);

    assert(hash == expected_hash);
}

static void test_hash_double(void) {
    double value = 3.1415926535;
    unsigned long expected_hash = 3574695756856218044;
    unsigned long hash = hash_double(value);

    assert(hash == expected_hash);
}

static void test_hash_char(void) {
    char value = 'A';
    unsigned long expected_hash = 475871767249088;
    unsigned long hash = hash_char(value);

    assert(hash == expected_hash);
}

static void test_hash_string(void) {
    char* value = "Hello, world!";
    unsigned long expected_hash = 5904905661493778094;
    unsigned long hash = hash_string(value);

    assert(hash == expected_hash);
}

void run_utils_tests(void) {
    test_str_dup();
    test_hash_int();
    test_hash_float();
    test_hash_double();
    test_hash_char();
    test_hash_string();

    printf("%s: All tests passed successfully!\n", __FILE__);
}
