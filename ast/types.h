#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "list.h"


typedef enum TypeID {
    INT_TYPE,
    FLOAT_TYPE,
    CHAR_TYPE,
    STRING_TYPE,
    BOOL_TYPE,
    VOID_TYPE,
    NIL_TYPE,
    STRUCT_TYPE,
    CUSTOM_TYPE
} TypeID;

typedef struct Type {
    TypeID id;
    size_t size;
    char* name;
    List* fields; /* List of (Type*) */
} Type;

Type* type_new(TypeID id, size_t size, char* name, List* fields);
void type_add_field(Type** type, Type* field);
void type_to_string(Type** type);
void type_free(Type** type);

#define NEW_INT_TYPE() type_new(INT_TYPE, sizeof(int), "int", NULL)
#define NEW_FLOAT_TYPE() type_new(FLOAT_TYPE, sizeof(double), "float", NULL)
#define NEW_CHAR_TYPE() type_new(CHAR_TYPE, sizeof(char), "char", NULL)
#define NEW_STRING_TYPE() type_new(STRING_TYPE, sizeof(char*), "string", NULL)
#define NEW_BOOL_TYPE() type_new(BOOL_TYPE, sizeof(bool), "bool", NULL)
#define NEW_VOID_TYPE() type_new(VOID_TYPE, 0, "void", NULL)
#define NEW_NIL_TYPE() type_new(NIL_TYPE, 0, "nil", NULL)

#define NEW_STRUCT_TYPE(size)                                                  \
    type_new(STRUCT_TYPE, (size), "struct",                                    \
        (list_new((void (*)(void **)) type_free)))

#define NEW_CUSTOM_TYPE(size, name)                                            \
    type_new(CUSTOM_TYPE, (size), (name), NULL)

#define NEW_CUSTOM_TYPE_WITH_FIELDS(size, name)                                \
    type_new(CUSTOM_TYPE, (size), (name),                                      \
        (list_new((void (*)(void **)) type_free)))

#define CUSTOM_TYPE_ADD_FIELDS(custom_type, ...)                               \
    do {                                                                       \
        Type* types[] = { __VA_ARGS__ };                                       \
        size_t n_types = sizeof(types) / sizeof(types[0]);                     \
        for (size_t i = 0; i < n_types; i++) {                                 \
            type_add_field(&(custom_type), types[i]);                          \
        }                                                                      \
    } while(0)
