#pragma once

#include <stdbool.h>
#include <stddef.h>


typedef enum TypeID {
    INT_TYPE,
    FLOAT_TYPE,
    CHAR_TYPE,
    STRING_TYPE,
    BOOL_TYPE,
    VOID_TYPE,
    NIL_TYPE,
    CUSTOM_TYPE
} TypeID;

typedef struct Type {
    TypeID id;
    size_t size;
    char* name;
} Type;

Type* type_new(TypeID id, size_t size, char* name);
void type_to_string(Type** type);
void type_free(Type** type);

#define NEW_INT_TYPE() type_new(INT_TYPE, sizeof(int), "int")
#define NEW_FLOAT_TYPE() type_new(FLOAT_TYPE, sizeof(double), "float")
#define NEW_CHAR_TYPE() type_new(CHAR_TYPE, sizeof(char), "char")
#define NEW_STIRNG_TYPE() type_new(STRING_TYPE, sizeof(char*), "string")
#define NEW_BOOL_TYPE() type_new(BOOL_TYPE, sizeof(bool), "bool")
#define NEW_VOID_TYPE() type_new(VOID_TYPE, 0, "void")
#define NEW_NIL_TYPE() type_new(NIL_TYPE, 0, "nil")
#define NEW_CUSTOM_TYPE(size, name) type_new(CUSTOM_TYPE, (size), (name))
