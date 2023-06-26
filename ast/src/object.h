#pragma once

#include "ast.h"
#include "buffer.h"
#include "context.h"
#include "interpreter.h"
#include "list.h"
#include "types.h"


typedef enum ObjectType {
    OBJ_ERROR,

    OBJ_INTEGER,
    OBJ_FLOAT,
    OBJ_CHARACTER,
    OBJ_STRING,
    OBJ_BOOLEAN,

    OBJ_NIL,

    OBJ_RETURN,
    OBJ_BREAK,
    OBJ_CONTINUE,

    OBJ_FUNCTION,
    OBJ_STRUCT,
    OBJ_ARRAY,

    OBJ_IDENT,
    OBJ_CALLABLE
} ObjectType;

typedef struct Object {
    ObjectType type;
    void* object;
    Type* (*get_type)(void*);
    void* (*copy)(void*);
    bool (*equals)(void*, void*);
    void (*to_string)(ByteBuffer*, void**);
    void (*destroy)(void**);
} Object;

Object* object_new(ObjectType type, void* object,
    Type* (*get_type)(void*),
    void* (*copy)(void*),
    bool (*equals)(void*, void*),
    void (*to_string)(ByteBuffer*, void**),
    void (*destroy)(void**));
Type* object_get_type(Object* object);
Object* object_copy(Object* self);
bool object_equals(Object* self, Object* other);
void object_to_string(ByteBuffer* byteBuffer, Object** object);
void object_free(Object** object);

typedef enum ErrorType {
    RUNTIME_ERROR,
    DIVISION_BY_ZERO_ERROR
} ErrorType;

typedef struct Error {
    ErrorType type;
    char* message;
} Error;

Error* error_new(ErrorType type, char* message);
Error* error_copy(Error* self);
ErrorType error_get_type(Error* self);
bool error_equals(Error* self, Object* other);
void error_to_string(ByteBuffer* byteBuffer, Error** error);
void error_free(Error** errorObject);

typedef struct IdentObject {
    Type* type;
    char* name;
    Object* value;
} IdentObject;

IdentObject* ident_object_new(Type* type, char* name, Object* value);
Type* ident_object_get_type(IdentObject* identObject);
IdentObject* ident_object_copy(IdentObject* self);
bool ident_object_equals(IdentObject* self, Object* other);
void ident_object_to_string(ByteBuffer* byteBuffer, IdentObject** identObject);
void ident_object_free(IdentObject** identObject);

typedef struct IntegerObject {
    Type* type;
    int value;
} IntegerObject;

IntegerObject* integer_object_new(Type* type, int value);
Type* integer_object_get_type(IntegerObject* integerObject);
IntegerObject* integer_object_copy(IntegerObject* self);
bool integer_object_equals(IntegerObject* self, Object* other);
void integer_object_to_string(ByteBuffer* byteBuffer, IntegerObject** integerObject);
void integer_object_free(IntegerObject** integerObject);

typedef struct FloatObject {
    Type* type;
    double value;
} FloatObject;

FloatObject* float_object_new(Type* type, double value);
Type* float_object_get_type(FloatObject* floatObject);
FloatObject* float_object_copy(FloatObject* self);
bool float_object_equals(FloatObject* self, Object* other);
void float_object_to_string(ByteBuffer* byteBuffer, FloatObject** floatObject);
void float_object_free(FloatObject** floatObject);

typedef struct CharacterObject {
    Type* type;
    char value;
} CharacterObject;

CharacterObject* character_object_new(Type* type, char value);
Type* character_object_get_type(CharacterObject* charObject);
CharacterObject* character_object_copy(CharacterObject* self);
bool character_object_equals(CharacterObject* self, Object* other);
void character_object_to_string(ByteBuffer* byteBuffer, CharacterObject** characterObject);
void character_object_free(CharacterObject** characterObject);

typedef struct StringObject {
    Type* type;
    char* value;
} StringObject;

StringObject* string_object_new(Type* type, char* value);
Type* string_object_get_type(StringObject* stringObject);
StringObject* string_object_copy(StringObject* self);
bool string_object_equals(StringObject* self, Object* other);
void string_object_to_string(ByteBuffer* byteBuffer, StringObject** stringObject);
void string_object_free(StringObject** stringObject);

typedef struct BooleanObject {
    Type* type;
    bool value;
} BooleanObject;

BooleanObject* boolean_object_new(Type* type, bool value);
Type* boolean_object_get_type(BooleanObject* booleanObject);
BooleanObject* boolean_object_copy(BooleanObject* self);
bool boolean_object_equals(BooleanObject* self, Object* other);
void boolean_object_to_string(ByteBuffer* byteBuffer, BooleanObject** booleanObject);
void boolean_object_free(BooleanObject** booleanObject);

typedef struct NilObject {
    Type* type;
} NilObject;

NilObject* nil_object_new(Type* type);
Type* nil_object_get_type(NilObject* nilObject);
NilObject* nil_object_copy(NilObject* self);
bool nil_object_equals(NilObject* self, Object* other);
void nil_object_to_string(ByteBuffer* byteBuffer, NilObject** nilObject);
void nil_object_free(NilObject** nilObject);

typedef struct ReturnObject {
    Object* value;
} ReturnObject;

ReturnObject* return_object_new(Object* value);
ReturnObject* return_object_copy(ReturnObject* self);
bool return_object_equals(ReturnObject* self, Object* other);
void return_object_to_string(ByteBuffer* byteBuffer, ReturnObject** returnObject);
void return_object_free(ReturnObject** returnObject);

typedef struct BreakObject {
    void* _;
} BreakObject;

BreakObject* break_object_new(void);
BreakObject* break_object_copy(BreakObject* self);
void break_object_free(BreakObject** breakObject);

typedef struct ContinueObject {
    void* _;
} ContinueObject;

ContinueObject* continue_object_new(void);
ContinueObject* continue_object_copy(ContinueObject* self);
void continue_object_free(ContinueObject** continueObject);

typedef struct FunctionObject {
    Type* type;
    Context* env;
    List* parameters;
    Stmt* body;
} FunctionObject;

FunctionObject* function_object_new(Type* type, Context* env, List* parameters, Stmt* body);
Type* function_object_get_type(FunctionObject* self);
bool function_object_equals(FunctionObject* self, Object* other);
void function_object_to_string(ByteBuffer* byteBuffer, FunctionObject** functionObject);
void function_object_free(FunctionObject** functionObject);
Object* function_object_run(struct Interpreter* interpreter, FunctionObject* functionObject, List* arguments);

#define NEW_FUNCTION_OBJECT(function_type, env, parameters, body)              \
    object_new(OBJ_FUNCTION,                                                   \
            function_object_new((function_type), (env), (parameters), (body)), \
        (Type* (*)(void*)) function_object_get_type,                           \
        (void* (*)(void*)) NULL,                                               \
        (bool (*)(void*, void*)) function_object_equals,                       \
        (void (*)(ByteBuffer*, void **)) function_object_to_string,            \
        (void (*)(void **)) function_object_free)

#define NEW_ERROR_OBJECT(error_type, message)                                  \
    object_new(OBJ_ERROR, error_new((error_type), (message)),                  \
        (Type* (*)(void*)) NULL,                                               \
        (void* (*)(void*)) error_copy,                                         \
        (bool (*)(void*, void*)) error_equals,                                 \
        (void (*)(ByteBuffer*, void **)) error_to_string,                      \
        (void (*)(void **)) error_free)

#define NEW_IDENT_OBJECT(varType, varName, varValue)                           \
    object_new(OBJ_IDENT, ident_object_new((varType), (varName), (varValue)),  \
        (Type* (*)(void*)) ident_object_get_type,                              \
        (void* (*)(void*)) ident_object_copy,                                  \
        (bool (*)(void*, void*)) ident_object_equals,                          \
        (void (*)(ByteBuffer*, void**)) ident_object_to_string,                \
        (void (*)(void**)) ident_object_free)

#define NEW_INTEGER_OBJECT(value)                                              \
    object_new(OBJ_INTEGER, integer_object_new((NEW_INT_TYPE()), (value)),     \
        (Type* (*)(void*)) integer_object_get_type,                            \
        (void* (*)(void*)) integer_object_copy,                                \
        (bool (*)(void*, void*)) integer_object_equals,                        \
        (void (*)(ByteBuffer*, void**)) integer_object_to_string,              \
        (void (*)(void**)) integer_object_free)

#define NEW_FLOAT_OBJECT(value)                                                \
    object_new(OBJ_FLOAT, float_object_new((NEW_FLOAT_TYPE()), (value)),       \
        (Type* (*)(void*)) float_object_get_type,                              \
        (void* (*)(void*)) float_object_copy,                                  \
        (bool (*)(void*, void*)) float_object_equals,                          \
        (void (*)(ByteBuffer*, void**)) float_object_to_string,                \
        (void (*)(void**)) float_object_free)

#define NEW_CHARACTER_OBJECT(value)                                            \
    object_new(OBJ_CHARACTER, character_object_new((NEW_CHAR_TYPE()), (value)),\
        (Type* (*)(void*)) character_object_get_type,                          \
        (void* (*)(void*)) character_object_copy,                              \
        (bool (*)(void*, void*)) character_object_equals,                      \
        (void (*)(ByteBuffer*, void**)) character_object_to_string,            \
        (void (*)(void**)) character_object_free)

#define NEW_STRING_OBJECT(value)                                               \
    object_new(OBJ_STRING, string_object_new((NEW_STRING_TYPE()), (value)),    \
        (Type* (*)(void*)) string_object_get_type,                             \
        (void* (*)(void*)) string_object_copy,                                 \
        (bool (*)(void*, void*)) string_object_equals,                         \
        (void (*)(ByteBuffer*, void**)) string_object_to_string,               \
        (void (*)(void**)) string_object_free)

#define NEW_BOOLEAN_OBJECT(value)                                              \
    object_new(OBJ_BOOLEAN, boolean_object_new((NEW_BOOL_TYPE()), (value)),    \
        (Type* (*)(void*)) boolean_object_get_type,                            \
        (void* (*)(void*)) boolean_object_copy,                                \
        (bool (*)(void*, void*)) boolean_object_equals,                        \
        (void (*)(ByteBuffer*, void**)) boolean_object_to_string,              \
        (void (*)(void**)) boolean_object_free)

#define NEW_NIL_OBJECT()                                                       \
    object_new(OBJ_NIL, nil_object_new((NEW_NIL_TYPE())),                      \
        (Type* (*)(void*)) nil_object_get_type,                                \
        (void* (*)(void*)) nil_object_copy,                                    \
        (bool (*)(void*, void*)) nil_object_equals,                            \
        (void (*)(ByteBuffer*, void**)) nil_object_to_string,                  \
        (void (*)(void**)) nil_object_free)

#define NEW_RETURN_OBJECT(value)                                               \
    object_new(OBJ_RETURN, return_object_new((value)),                         \
        (Type* (*)(void*)) NULL,                                               \
        (void* (*)(void*)) return_object_copy,                                 \
        (bool (*)(void*, void*)) return_object_equals,                         \
        (void (*)(ByteBuffer*, void**)) return_object_to_string,               \
        (void (*)(void**)) return_object_free)

#define NEW_BREAK_OBJECT()                                                     \
    object_new(OBJ_BREAK, break_object_new(),                                  \
        (Type* (*)(void*)) NULL,                                               \
        (void* (*)(void*)) break_object_copy,                                  \
        (bool (*)(void*, void*)) NULL,                                         \
        (void (*)(ByteBuffer*, void**)) NULL,                                  \
        (void (*)(void**)) break_object_free)

#define NEW_CONTINUE_OBJECT()                                                  \
    object_new(OBJ_CONTINUE, continue_object_new(),                            \
        (Type* (*)(void*)) NULL,                                               \
        (void* (*)(void*)) continue_object_copy,                               \
        (bool (*)(void*, void*)) NULL,                                         \
        (void (*)(ByteBuffer*, void**)) NULL,                                  \
        (void (*)(void**)) continue_object_free)


typedef struct Callable {
    Object* functionObject;
    Object* (*function)(struct Interpreter*, FunctionObject*, List*);
    void (*to_string)(ByteBuffer*, void**);
    void (*destroy)(void**);
} Callable;

Callable* callable_new(
    Object* functionObject,
    Object* (*function)(struct Interpreter*, FunctionObject*, List*),
    void (*to_string)(ByteBuffer*, void**),
    void (*destroy)(void**));
Object* callable_run(struct Interpreter* interpreter, Object* callable, List* arguments);
void callable_to_string(ByteBuffer* byteBuffer, Callable** callable);
void callable_free(Callable** callable);

Object* print_function_run(struct Interpreter* interpreter, FunctionObject* functionObject, List* arguments);
Object* println_function_run(struct Interpreter* interpreter, FunctionObject* functionObject, List* arguments);
Object* input_function_run(struct Interpreter* interpreter, FunctionObject* functionObject, List* arguments);

#define NEW_CALLABLE(func_obj, func_executer, func_obj_to_str, func_obj_free)  \
    callable_new(                                                              \
        (func_obj),                                                            \
        (func_executer),                                                       \
        (func_obj_to_str),                                                     \
        (func_obj_free)                                                        \
    )

#define NEW_PRINT_FUNC()                                                       \
    object_new(OBJ_CALLABLE,                                                   \
            NEW_CALLABLE(                                                      \
                NULL,                                                          \
                print_function_run,                                            \
                NULL,                                                          \
                NULL                                                           \
            ),                                                                 \
        (Type* (*)(void*)) NULL,                                               \
        (void* (*)(void*)) NULL,                                               \
        (bool (*)(void*, void*)) NULL,                                         \
        (void (*)(ByteBuffer*, void **)) callable_to_string,                   \
        (void (*)(void **)) callable_free)

#define NEW_PRINTLN_FUNC()                                                       \
    object_new(OBJ_CALLABLE,                                                   \
            NEW_CALLABLE(                                                      \
                NULL,                                                          \
                println_function_run,                                          \
                NULL,                                                          \
                NULL                                                           \
            ),                                                                 \
        (Type* (*)(void*)) NULL,                                               \
        (void* (*)(void*)) NULL,                                               \
        (bool (*)(void*, void*)) NULL,                                         \
        (void (*)(ByteBuffer*, void **)) callable_to_string,                   \
        (void (*)(void **)) callable_free)

#define NEW_INPUT_FUNC()                                                       \
    object_new(OBJ_CALLABLE,                                                   \
            NEW_CALLABLE(                                                      \
                NULL,                                                          \
                input_function_run,                                            \
                NULL,                                                          \
                NULL                                                           \
            ),                                                                 \
        (Type* (*)(void*)) NULL,                                               \
        (void* (*)(void*)) NULL,                                               \
        (bool (*)(void*, void*)) NULL,                                         \
        (void (*)(ByteBuffer*, void **)) callable_to_string,                   \
        (void (*)(void **)) callable_free)

#define NEW_CALLABLE_OBJECT(func_obj)                                          \
    object_new(OBJ_CALLABLE,                                                   \
            NEW_CALLABLE(                                                      \
                (func_obj),                                                    \
                function_object_run,                                           \
                (void (*)(ByteBuffer*, void**)) function_object_to_string,     \
                (void (*)(void **)) function_object_free                       \
            ),                                                                 \
        (Type* (*)(void*)) NULL,                                               \
        (void* (*)(void*)) NULL,                                               \
        (bool (*)(void*, void*)) NULL,                                         \
        (void (*)(ByteBuffer*, void **)) callable_to_string,                   \
        (void (*)(void **)) callable_free)
