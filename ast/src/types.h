#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "list.h"


typedef enum TypeID {
    _atomic_start,
    INT_TYPE,
    FLOAT_TYPE,
    CHAR_TYPE,
    STRING_TYPE,
    BOOL_TYPE,
    VOID_TYPE,
    NIL_TYPE,
    _atomic_end,

    CUSTOM_TYPE,

    NAMED_TYPE,
    STRUCT_TYPE,
    ARRAY_DIMENSION_TYPE,
    ARRAY_TYPE,
    FUNC_TYPE
} TypeID;

typedef struct Type {
    TypeID typeId;
    void* type;
    void* (*copy)(const void**);
    bool (*equals)(void**, void**);
    void (*to_string)(void**);
    void (*destroy)(void**);
} Type;

Type* type_new(TypeID typeID, void* type,
    void* (*copy)(const void**),
    bool (*equals)(void**, void**),
    void (*to_string)(void**),
    void (*destroy)(void**));
Type* type_copy(const Type** self);
bool type_equals(Type** self, Type** other);
void type_to_string(Type** type);
void type_free(Type** type);

typedef struct AtomicType {
    size_t size;
    char* name;
} AtomicType;

AtomicType* atomic_type_new(size_t size, char* name);
AtomicType* atomic_type_copy(const AtomicType** self);
bool atomic_type_equals(AtomicType** self, Type** other);
void atomic_type_to_string(AtomicType** atomicType);
void atomic_type_free(AtomicType** atomicType);

typedef struct NamedType {
    char* name;
    Type* type;
} NamedType;

NamedType* named_type_new(char* name, Type* type);
NamedType* named_type_copy(const NamedType** self);
bool named_type_equals(NamedType** self, Type** other);
void named_type_to_string(NamedType** namedType);
void named_type_free(NamedType** namedType);

typedef struct StructType {
    size_t size;
    char* name;
    List* fields; /* List of (NamedType*) */
} StructType;

StructType* struct_type_new(size_t size, char* name, List* fields);
void struct_type_add_field(StructType** structType, Type* field);
StructType* struct_type_copy(const StructType** self);
bool struct_type_equals(StructType** self, Type** other);
void struct_type_to_string(StructType** structType);
void struct_type_free(StructType** structType);

typedef struct ArrayDimension {
    size_t size;
} ArrayDimension;

ArrayDimension* array_dimension_new(size_t size);
ArrayDimension* array_dimension_copy(const ArrayDimension** self);
bool array_dimension_equals(ArrayDimension** self, Type** other);
void array_dimension_to_string(ArrayDimension** arrayDimension);
void array_dimension_free(ArrayDimension** arrayDimension);

typedef struct ArrayType {
    List* dimensions;
    Type* type;
} ArrayType;

ArrayType* array_type_new(List* dimensions, Type* type);
void array_type_add_dimension(ArrayType** arrayType, Type* dimension);
ArrayType* array_type_copy(const ArrayType** self);
bool array_type_equals(ArrayType** self, Type** other);
void array_type_to_string(ArrayType** arrayType);
void array_type_free(ArrayType** arrayType);

typedef struct FunctionType {
    List* parameterTypes;
    Type* returnType;
} FunctionType;

FunctionType* function_type_new(List* parameterTypes, Type* returnType);
void function_type_add_parameter(FunctionType** functionType, Type* parameter);
FunctionType* function_type_copy(const FunctionType** self);
bool function_type_equals(FunctionType** self, Type** other);
void function_type_to_string(FunctionType** functionType);
void function_type_free(FunctionType** functionType);

#define NEW_INT_TYPE()                                                         \
    type_new(INT_TYPE,                                                         \
        atomic_type_new(sizeof(int), "int"),                                   \
        (void* (*)(const void**)) atomic_type_copy,                            \
        (bool (*)(void**, void**)) atomic_type_equals,                         \
        (void (*)(void**)) atomic_type_to_string,                              \
        (void (*)(void**)) atomic_type_free)

#define NEW_FLOAT_TYPE()                                                       \
    type_new(FLOAT_TYPE,                                                       \
        atomic_type_new(sizeof(double), "float"),                              \
        (void* (*)(const void**)) atomic_type_copy,                            \
        (bool (*)(void**, void**)) atomic_type_equals,                         \
        (void (*)(void**)) atomic_type_to_string,                              \
        (void (*)(void**)) atomic_type_free)

#define NEW_CHAR_TYPE()                                                        \
    type_new(CHAR_TYPE,                                                        \
        atomic_type_new(sizeof(char), "char"),                                 \
        (void* (*)(const void**)) atomic_type_copy,                            \
        (bool (*)(void**, void**)) atomic_type_equals,                         \
        (void (*)(void**)) atomic_type_to_string,                              \
        (void (*)(void**)) atomic_type_free)

#define NEW_STRING_TYPE()                                                      \
    type_new(STRING_TYPE,                                                      \
        atomic_type_new(sizeof(char*), "string"),                              \
        (void* (*)(const void**)) atomic_type_copy,                            \
        (bool (*)(void**, void**)) atomic_type_equals,                         \
        (void (*)(void**)) atomic_type_to_string,                              \
        (void (*)(void**)) atomic_type_free)

#define NEW_BOOL_TYPE()                                                        \
    type_new(BOOL_TYPE,                                                        \
        atomic_type_new(sizeof(bool), "bool"),                                 \
        (void* (*)(const void**)) atomic_type_copy,                            \
        (bool (*)(void**, void**)) atomic_type_equals,                         \
        (void (*)(void**)) atomic_type_to_string,                              \
        (void (*)(void**)) atomic_type_free)

#define NEW_VOID_TYPE()                                                        \
    type_new(VOID_TYPE,                                                        \
        atomic_type_new(0, "void"),                                            \
        (void* (*)(const void**)) atomic_type_copy,                            \
        (bool (*)(void**, void**)) atomic_type_equals,                         \
        (void (*)(void**)) atomic_type_to_string,                              \
        (void (*)(void**)) atomic_type_free)

#define NEW_NIL_TYPE()                                                         \
    type_new(NIL_TYPE,                                                         \
        atomic_type_new(0, "nil"),                                             \
        (void* (*)(const void**)) atomic_type_copy,                            \
        (bool (*)(void**, void**)) atomic_type_equals,                         \
        (void (*)(void**)) atomic_type_to_string,                              \
        (void (*)(void**)) atomic_type_free)

#define NEW_CUSTOM_TYPE(size, name)                                            \
    type_new(CUSTOM_TYPE,                                                      \
        atomic_type_new((size), (name)),                                       \
        (void* (*)(const void**)) atomic_type_copy,                            \
        (bool (*)(void**, void**)) atomic_type_equals,                         \
        (void (*)(void**)) atomic_type_to_string,                              \
        (void (*)(void**)) atomic_type_free)

#define NEW_NAMED_TYPE(name, type)                                             \
    type_new(NAMED_TYPE,                                                       \
        named_type_new((name), (type)),                                        \
        (void* (*)(const void**)) named_type_copy,                             \
        (bool (*)(void**, void**)) named_type_equals,                          \
        (void (*)(void**)) named_type_to_string,                               \
        (void (*)(void**)) named_type_free)

#define NEW_STRUCT_TYPE(size)                                                  \
    type_new(STRUCT_TYPE,                                                      \
        struct_type_new((size), "struct",                                      \
            (list_new((void (*)(void **)) type_free))),                        \
        (void* (*)(const void**)) struct_type_copy,                            \
        (bool (*)(void**, void**)) struct_type_equals,                         \
        (void (*)(void**)) struct_type_to_string,                              \
        (void (*)(void**)) struct_type_free)

#define NEW_STRUCT_TYPE_WITH_FIELDS(size, fields)                              \
    type_new(STRUCT_TYPE,                                                      \
        struct_type_new((size), "struct", (fields)),                           \
        (void* (*)(const void**)) struct_type_copy,                            \
        (bool (*)(void**, void**)) struct_type_equals,                         \
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

#define NEW_ARRAY_DIMENSION(size)                                              \
    type_new(ARRAY_DIMENSION_TYPE,                                             \
        array_dimension_new((size)),                                           \
        (void* (*)(const void**)) array_dimension_copy,                        \
        (bool (*)(void**, void**)) array_dimension_equals,                     \
        (void (*)(void**)) array_dimension_to_string,                          \
        (void (*)(void**)) array_dimension_free)

#define NEW_ARRAY_UNDEFINED_DIMENSION()                                        \
    type_new(ARRAY_DIMENSION_TYPE,                                             \
        array_dimension_new(0),                                                \
        (void* (*)(const void**)) array_dimension_copy,                        \
        (bool (*)(void**, void**)) array_dimension_equals,                     \
        (void (*)(void**)) array_dimension_to_string,                          \
        (void (*)(void**)) array_dimension_free)

#define NEW_ARRAY_TYPE(type)                                                   \
    type_new(ARRAY_TYPE,                                                       \
        array_type_new((list_new((void (*)(void **)) type_free)), (type)),     \
        (void* (*)(const void**)) array_type_copy,                             \
        (bool (*)(void**, void**)) array_type_equals,                          \
        (void (*)(void**)) array_type_to_string,                               \
        (void (*)(void**)) array_type_free)

#define NEW_ARRAY_TYPE_WITH_DIMENSION(dimension, type)                         \
    type_new(ARRAY_TYPE,                                                       \
        array_type_new((dimension), (type)),                                   \
        (void* (*)(const void**)) array_type_copy,                             \
        (bool (*)(void**, void**)) array_type_equals,                          \
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

#define NEW_FUNCTION_TYPE()                                                    \
    type_new(FUNC_TYPE,                                                        \
        function_type_new(                                                     \
            (list_new((void (*)(void **)) type_free)),                         \
            NULL),                                                             \
        (void* (*)(const void**)) function_type_copy,                          \
        (bool (*)(void**, void**)) function_type_equals,                       \
        (void (*)(void**)) function_type_to_string,                            \
        (void (*)(void**)) function_type_free)

#define NEW_FUNCTION_TYPE_WITH_PARAMS(parameters)                              \
    type_new(FUNC_TYPE,                                                        \
        function_type_new(                                                     \
            (parameters), NULL),                                               \
        (void* (*)(const void**)) function_type_copy,                          \
        (bool (*)(void**, void**)) function_type_equals,                       \
        (void (*)(void**)) function_type_to_string,                            \
        (void (*)(void**)) function_type_free)

#define NEW_FUNCTION_TYPE_WITH_RETURN(retrn)                                   \
    type_new(FUNC_TYPE,                                                        \
        function_type_new(                                                     \
            (list_new((void (*)(void **)) type_free)), (retrn)),               \
        (void* (*)(const void**)) function_type_copy,                          \
        (bool (*)(void**, void**)) function_type_equals,                       \
        (void (*)(void**)) function_type_to_string,                            \
        (void (*)(void**)) function_type_free)

#define NEW_FUNCTION_TYPE_WITH_PARAMS_AND_RETURN(parameters, retrn)            \
    type_new(FUNC_TYPE,                                                        \
        function_type_new((parameters), (retrn)),                              \
        (void* (*)(const void**)) function_type_copy,                          \
        (bool (*)(void**, void**)) function_type_equals,                       \
        (void (*)(void**)) function_type_to_string,                            \
        (void (*)(void**)) function_type_free)

#define FUNCTION_TYPE_ADD_PARAM(func_type, param)                              \
    function_type_add_parameter((FunctionType**) (&(func_type)->type), param)

#define FUNCTION_TYPE_ADD_PARAMS(func_type, ...)                               \
    do {                                                                       \
        Type* params[] = { __VA_ARGS__ };                                      \
        size_t n_params = sizeof(params) / sizeof(params[0]);                  \
        for (size_t i = 0; i < n_params; i++) {                                \
            FUNCTION_TYPE_ADD_PARAM((func_type), params[i]);                   \
        }                                                                      \
    } while(0)

#define TYPE_PRINT_AND_FREE(type)                                              \
    type_to_string((&(type)));                                                 \
    printf("\n");                                                              \
    type_free((&(type)))
