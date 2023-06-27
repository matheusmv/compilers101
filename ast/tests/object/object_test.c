#include "object_test.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/object.h"


static void test_error_object(void) {
    Object* error_obj = NEW_ERROR_OBJECT(DIVISION_BY_ZERO_ERROR, "invalid operation: division by zero");

    assert(error_obj->type == OBJ_ERROR);
    assert(error_get_type(error_obj->object) == DIVISION_BY_ZERO_ERROR);

    ByteBuffer* bb = byte_buffer_new();

    object_to_string(bb, &error_obj);

    char* str = byte_buffer_to_string(bb);

    assert(strcmp(str, "invalid operation: division by zero") == 0);

    free(str);

    byte_buffer_free(&bb);

    Object* error_obj_copy = object_copy(error_obj);

    assert(error_obj != error_obj_copy);
    assert(error_get_type(error_obj->object) == error_get_type(error_obj_copy->object));
    assert(object_equals(error_obj, error_obj_copy) == true);

    object_free(&error_obj);
    object_free(&error_obj_copy);

    assert(error_obj == NULL);
    assert(error_obj_copy == NULL);
}

static void test_integer_object(void) {
    Object* integer = NEW_INTEGER_OBJECT(42);
    assert(integer->type == OBJ_INTEGER);

    assert(object_get_type(integer) != NULL);
    assert(object_get_type(integer)->typeId == INT_TYPE);

    Object* integer_copy = object_copy(integer);
    assert(integer_copy != NULL);
    assert(integer_copy != integer);
    assert(integer_copy->type == integer->type);

    assert(object_equals(integer, integer_copy) == true);

    ByteBuffer* bb = byte_buffer_new();
    object_to_string(bb, &integer);
    char* str = byte_buffer_to_string(bb);
    assert(strcmp(str, "42") == 0);
    free(str);
    byte_buffer_free(&bb);

    object_free(&integer);
    object_free(&integer_copy);
    assert(integer == NULL);
    assert(integer_copy == NULL);
}

static void test_float_object(void) {
    Object* float_obj = NEW_FLOAT_OBJECT(3.14);
    assert(float_obj->type == OBJ_FLOAT);

    assert(object_get_type(float_obj) != NULL);
    assert(object_get_type(float_obj)->typeId == FLOAT_TYPE);

    Object* float_obj_copy = object_copy(float_obj);
    assert(float_obj_copy != NULL);
    assert(float_obj_copy != float_obj);
    assert(float_obj_copy->type == float_obj->type);

    assert(object_equals(float_obj, float_obj_copy) == true);

    ByteBuffer* buffer = byte_buffer_new();
    object_to_string(buffer, &float_obj);
    char* str = byte_buffer_to_string(buffer);
    assert(strcmp(str, "3.140000") == 0);
    free(str);
    byte_buffer_free(&buffer);

    object_free(&float_obj);
    object_free(&float_obj_copy);
    assert(float_obj == NULL);
    assert(float_obj_copy == NULL);
}

static void test_character_object(void) {
    Object* character = NEW_CHARACTER_OBJECT('A');
    assert(character->type == OBJ_CHARACTER);

    assert(object_get_type(character) != NULL);
    assert(object_get_type(character)->typeId == CHAR_TYPE);

    Object* character_copy = object_copy(character);
    assert(character_copy != NULL);
    assert(character_copy != character);
    assert(character_copy->type == character->type);

    assert(object_equals(character, character_copy) == true);

    ByteBuffer* buffer = byte_buffer_new();
    object_to_string(buffer, &character);
    char* str = byte_buffer_to_string(buffer);
    assert(strcmp(str, "A") == 0);
    free(str);
    byte_buffer_free(&buffer);

    object_free(&character);
    object_free(&character_copy);
    assert(character == NULL);
    assert(character_copy == NULL);
}

static void test_string_object(void) {
    Object* string_obj = NEW_STRING_OBJECT("Hello, world!");
    assert(string_obj->type == OBJ_STRING);

    assert(object_get_type(string_obj) != NULL);
    assert(object_get_type(string_obj)->typeId == STRING_TYPE);

    Object* string_copy = object_copy(string_obj);
    assert(string_copy != NULL);
    assert(string_copy != string_obj);
    assert(string_copy->type == string_obj->type);

    assert(object_equals(string_obj, string_copy) == true);

    ByteBuffer* buffer = byte_buffer_new();
    object_to_string(buffer, &string_obj);
    char* str = byte_buffer_to_string(buffer);
    assert(strcmp(str, "Hello, world!") == 0);
    free(str);
    byte_buffer_free(&buffer);

    object_free(&string_obj);
    object_free(&string_copy);
    assert(string_obj == NULL);
    assert(string_copy == NULL);
}

static void test_bool_object(void) {
    Object* boolean = NEW_BOOLEAN_OBJECT(true);
    assert(boolean->type == OBJ_BOOLEAN);

    assert(object_get_type(boolean) != NULL);
    assert(object_get_type(boolean)->typeId == BOOL_TYPE);

    Object* boolean_copy = object_copy(boolean);
    assert(boolean_copy != NULL);
    assert(boolean_copy != boolean);
    assert(boolean_copy->type == boolean->type);

    assert(object_equals(boolean, boolean_copy) == true);

    ByteBuffer* buffer = byte_buffer_new();
    object_to_string(buffer, &boolean);
    char* str = byte_buffer_to_string(buffer);
    assert(strcmp(str, "true") == 0);
    free(str);
    byte_buffer_free(&buffer);

    object_free(&boolean);
    object_free(&boolean_copy);
    assert(boolean == NULL);
    assert(boolean_copy == NULL);
}

static void test_nil_object(void) {
    Object* nil = NEW_NIL_OBJECT();
    assert(nil->type == OBJ_NIL);

    assert(object_get_type(nil) != NULL);
    assert(object_get_type(nil)->typeId == NIL_TYPE);

    Object* nil_copy = object_copy(nil);
    assert(nil_copy != NULL);
    assert(nil_copy != nil);
    assert(nil_copy->type == nil->type);

    assert(object_equals(nil, nil_copy) == true);

    ByteBuffer* buffer = byte_buffer_new();
    object_to_string(buffer, &nil);
    char* str = byte_buffer_to_string(buffer);
    assert(strcmp(str, "nil") == 0);
    free(str);
    byte_buffer_free(&buffer);

    object_free(&nil);
    object_free(&nil_copy);
    assert(nil == NULL);
    assert(nil_copy == NULL);
}

static void test_return_object(void) {
    Object* inner_obj = NEW_INTEGER_OBJECT(1);
    Object* return_obj = NEW_RETURN_OBJECT(inner_obj);

    assert(return_obj->type == OBJ_RETURN);

    assert(object_get_type(return_obj) == NULL);

    Object* return_obj_copy = object_copy(return_obj);
    assert(return_obj_copy != NULL);
    assert(return_obj_copy != return_obj);
    assert(return_obj_copy->type == return_obj->type);

    assert(object_equals(return_obj, return_obj_copy) == true);

    ByteBuffer* buffer = byte_buffer_new();
    object_to_string(buffer, &return_obj);
    char* str = byte_buffer_to_string(buffer);
    assert(strcmp(str, "1") == 0);
    free(str);
    byte_buffer_free(&buffer);

    object_free(&return_obj);
    object_free(&return_obj_copy);
    assert(return_obj == NULL);
    assert(return_obj_copy == NULL);
}

static void test_break_object(void) {
    Object* break_obj = NEW_BREAK_OBJECT();
    assert(break_obj->type == OBJ_BREAK);

    object_free(&break_obj);
}

static void test_continue_object(void) {
    Object* continue_obj = NEW_CONTINUE_OBJECT();
    assert(continue_obj->type == OBJ_CONTINUE);

    object_free(&continue_obj);
}

void run_object_tests(void) {
    test_error_object();
    test_integer_object();
    test_float_object();
    test_character_object();
    test_string_object();
    test_bool_object();
    test_nil_object();
    test_return_object();
    test_break_object();
    test_continue_object();

    printf("%s: All tests passed successfully!\n", __FILE__);
}
