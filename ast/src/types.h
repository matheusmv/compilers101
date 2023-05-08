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
    ARRAY_DIMENSION_TYPE,
    ARRAY_TYPE,
    NAMED_TYPE,
    CUSTOM_TYPE
} TypeID;

typedef struct Type {
    TypeID typeId;
    void* type;
    void (*to_string)(void**);
    void (*destroy)(void**);
} Type;

Type* type_new(TypeID typeID, void* type, void (*to_string)(void**), void (*destroy)(void**));
void type_to_string(Type** type);
void type_free(Type** type);

typedef struct CommonType {
    size_t size;
    char* name;
} CommonType;

CommonType* common_type_new(size_t size, char* name);
void common_type_to_string(CommonType** commonType);
void common_type_free(CommonType** commonType);

typedef struct NamedType {
    char* name;
    Type* type;
} NamedType;

NamedType* named_type_new(char* name, Type* type);
void named_type_to_string(NamedType** namedType);
void named_type_free(NamedType** namedType);

typedef struct StructType {
    size_t size;
    char* name;
    List* fields; /* List of (NamedType*) */
} StructType;

StructType* struct_type_new(size_t size, char* name, List* fields);
void struct_type_add_field(StructType** structType, Type* field);
void struct_type_to_string(StructType** structType);
void struct_type_free(StructType** structType);

typedef struct ArrayDimension {
    size_t size;
} ArrayDimension;

ArrayDimension* array_dimension_new(size_t size);
void array_dimension_to_string(ArrayDimension** arrayDimension);
void array_dimension_free(ArrayDimension** arrayDimension);

typedef struct ArrayType {
    List* dimensions;
    Type* type;
} ArrayType;

ArrayType* array_type_new(List* dimensions, Type* type);
void array_type_add_dimension(ArrayType** arrayType, Type* dimension);
void array_type_to_string(ArrayType** arrayType);
void array_type_free(ArrayType** arrayType);

#define NEW_INT_TYPE()                                                         \
    type_new(INT_TYPE,                                                         \
        common_type_new(sizeof(int), "int"),                                   \
        (void (*)(void**)) common_type_to_string,                              \
        (void (*)(void**)) common_type_free)

#define NEW_FLOAT_TYPE()                                                       \
    type_new(FLOAT_TYPE,                                                       \
        common_type_new(sizeof(double), "float"),                              \
        (void (*)(void**)) common_type_to_string,                              \
        (void (*)(void**)) common_type_free)

#define NEW_CHAR_TYPE()                                                        \
    type_new(CHAR_TYPE,                                                        \
        common_type_new(sizeof(char), "char"),                                 \
        (void (*)(void**)) common_type_to_string,                              \
        (void (*)(void**)) common_type_free)

#define NEW_STRING_TYPE()                                                      \
    type_new(STRING_TYPE,                                                      \
        common_type_new(sizeof(char*), "string"),                              \
        (void (*)(void**)) common_type_to_string,                              \
        (void (*)(void**)) common_type_free)

#define NEW_BOOL_TYPE()                                                        \
    type_new(BOOL_TYPE,                                                        \
        common_type_new(sizeof(bool), "bool"),                                 \
        (void (*)(void**)) common_type_to_string,                              \
        (void (*)(void**)) common_type_free)

#define NEW_VOID_TYPE()                                                        \
    type_new(VOID_TYPE,                                                        \
        common_type_new(0, "void"),                                            \
        (void (*)(void**)) common_type_to_string,                              \
        (void (*)(void**)) common_type_free)

#define NEW_NIL_TYPE()                                                         \
    type_new(NIL_TYPE,                                                         \
        common_type_new(0, "nil"),                                             \
        (void (*)(void**)) common_type_to_string,                              \
        (void (*)(void**)) common_type_free)

#define NEW_NAMED_TYPE(name, type)                                             \
    type_new(NAMED_TYPE,                                                       \
        named_type_new((name), (type)),                                        \
        (void (*)(void**)) named_type_to_string,                               \
        (void (*)(void**)) named_type_free)

#define NEW_STRUCT_TYPE(size)                                                  \
    type_new(STRUCT_TYPE,                                                      \
        struct_type_new((size), "struct",                                      \
            (list_new((void (*)(void **)) type_free))),                        \
        (void (*)(void**)) struct_type_to_string,                              \
        (void (*)(void**)) struct_type_free)

#define NEW_STRUCT_TYPE_WITH_FIELDS(size, fields)                              \
    type_new(STRUCT_TYPE,                                                      \
        struct_type_new((size), "struct", (fields)),                           \
        (void (*)(void**)) struct_type_to_string,                              \
        (void (*)(void**)) struct_type_free)

#define STRUCT_TYPE_ADD_FIELD(struct_type, field)                              \
    struct_type_add_field(                                                     \
        (StructType**) (&(struct_type)->type), field)

#define STRUCT_TYPE_ADD_FIELDS(struct_type, ...)                               \
    do {                                                                       \
        Type* types[] = { __VA_ARGS__ };                                       \
        size_t n_types = sizeof(types) / sizeof(types[0]);                     \
        for (size_t i = 0; i < n_types; i++) {                                 \
            STRUCT_TYPE_ADD_FIELD((struct_type), types[i]);                    \
        }                                                                      \
    } while(0)

#define NEW_CUSTOM_TYPE(size, name)                                            \
    type_new(CUSTOM_TYPE,                                                      \
        common_type_new((size), (name)),                                       \
        (void (*)(void**)) common_type_to_string,                              \
        (void (*)(void**)) common_type_free)

#define NEW_ARRAY_DIMENSION(size)                                              \
    type_new(ARRAY_DIMENSION_TYPE,                                             \
        array_dimension_new((size)),                                           \
        (void (*)(void**)) array_dimension_to_string,                          \
        (void (*)(void**)) array_dimension_free)

#define NEW_ARRAY_UNDEFINED_DIMENSION()                                        \
    type_new(ARRAY_DIMENSION_TYPE,                                             \
        array_dimension_new(0),                                                \
        (void (*)(void**)) array_dimension_to_string,                          \
        (void (*)(void**)) array_dimension_free)

#define NEW_ARRAY_TYPE(type)                                                   \
    type_new(ARRAY_TYPE,                                                       \
        array_type_new((list_new((void (*)(void **)) type_free)), (type)),     \
        (void (*)(void**)) array_type_to_string,                               \
        (void (*)(void**)) array_type_free)

#define NEW_ARRAY_TYPE_WITH_DIMENSION(dimension, type)                         \
    type_new(ARRAY_TYPE,                                                       \
        array_type_new((dimension), (type)),                                   \
        (void (*)(void**)) array_type_to_string,                               \
        (void (*)(void**)) array_type_free)

#define ARRAY_TYPE_ADD_DIMENSION(array_type, dimension)                        \
    array_type_add_dimension((ArrayType**) (&(array_type)->type), dimension)

#define ARRAY_TYPE_ADD_DIMENSIONS(array_type, ...)                             \
    do {                                                                       \
        Type* dimensions[] = { __VA_ARGS__ };                                  \
        size_t n_dimensions = sizeof(dimensions) / sizeof(dimensions[0]);      \
        for (size_t i = 0; i < n_dimensions; i++) {                            \
            ARRAY_TYPE_ADD_DIMENSION((array_type), dimensions[i]);             \
        }                                                                      \
    } while(0)

#define TYPE_PRINT_AND_FREE(type)                                              \
    type_to_string((&(type)));                                                 \
    printf("\n");                                                              \
    type_free((&(type)))
