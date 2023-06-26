#include "object.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "buffer.h"
#include "context.h"
#include "interpreter.h"
#include "list.h"
#include "smem.h"
#include "types.h"
#include "utils.h"


Object* object_new(ObjectType type, void* object,
    Type* (*get_type)(void*),
    void* (*copy)(void*),
    bool (*equals)(void*, void*),
    void (*to_string)(ByteBuffer*, void**),
    void (*destroy)(void**))
{
    Object* new_object = NULL;
    new_object = safe_malloc(sizeof(Object), NULL);
    if (new_object == NULL) {
        if (destroy != NULL) {
            destroy(&object);
        }
        return NULL;
    }

    *new_object = (Object) {
        .type = type,
        .object = object,
        .get_type = get_type,
        .copy = copy,
        .equals = equals,
        .to_string = to_string,
        .destroy = destroy
    };

    return new_object;
}

Type* object_get_type(Object* object) {
    if (object == NULL || object->get_type == NULL)
        return NULL;

    return object->get_type(object->object);
}

Object* object_copy(Object* self) {
    if (self == NULL || self->copy == NULL)
        return NULL;

    void* objectCopy = self->copy(self->object);

    return object_new(
        self->type,
        objectCopy,
        self->get_type,
        self->copy,
        self->equals,
        self->to_string,
        self->destroy
    );
}

bool object_equals(Object* self, Object* other) {
    if (self == NULL || self->object == NULL || other == NULL || other->object == NULL || self->equals == NULL)
        return false;

    return self->equals(self->object, other);
}

void object_to_string(ByteBuffer* byteBuffer, Object** object) {
    if (byteBuffer == NULL || object == NULL || *object == NULL)
        return;

    if ((*object)->to_string != NULL) {
        (*object)->to_string(byteBuffer, &(*object)->object);
    } else {
        byte_buffer_appendf(byteBuffer, "[Object: %p]", *object);
    }
}

void object_free(Object** object) {
    if (object == NULL || *object == NULL)
        return;

    if ((*object)->destroy != NULL) {
        (*object)->destroy(&(*object)->object);
    }

    safe_free((void**) object);
}

Error* error_new(ErrorType type, char* message) {
    Error* new_error = NULL;
    new_error = safe_malloc(sizeof(Error), NULL);
    if (new_error == NULL) {
        return NULL;
    }

    *new_error = (Error) {
        .type = type,
        .message = str_dup(message)
    };

    return new_error;
}

Error* error_copy(Error* self) {
    if (self == NULL)
        return NULL;

    return error_new(self->type, self->message);
}

ErrorType error_get_type(Error* self) {
    if (self == NULL)
        return RUNTIME_ERROR;

    return self->type;
}

bool error_equals(Error* self, Object* other) {
    if (self == NULL || other == NULL)
        return false;

    if (other->type != OBJ_ERROR)
        return false;

    Error* otherError = (Error*) other->object;

    if (self->type != otherError->type)
        return false;

    return strcmp(self->message, otherError->message) == 0;
}

void error_to_string(ByteBuffer* byteBuffer, Error** error) {
    if (byteBuffer == NULL || error == NULL || *error == NULL)
        return;

    byte_buffer_appendf(byteBuffer, "%s", (*error)->message);
}

void error_free(Error** errorObject) {
    if (errorObject == NULL || *errorObject == NULL)
        return;

    safe_free((void**) &(*errorObject)->message);
    safe_free((void**) errorObject);
}

IdentObject* ident_object_new(Type* type, char* name, Object* value) {
    IdentObject* new_ident = NULL;
    new_ident = safe_malloc(sizeof(IdentObject), NULL);
    if (new_ident == NULL) {
        type_free(&type);
        object_free(&value);
        return NULL;
    }

    *new_ident = (IdentObject) {
        .type = type,
        .name = str_dup(name),
        .value = value
    };

    return new_ident;
}

Type* ident_object_get_type(IdentObject* identObject) {
    if (identObject == NULL)
        return NULL;

    return identObject->type;
}

IdentObject* ident_object_copy(IdentObject* self) {
    if (self == NULL)
        return NULL;

    return ident_object_new(
        type_copy((const Type**) &self->type),
        self->name,
        object_copy(self->value)
    );
}

bool ident_object_equals(IdentObject* self, Object* other) {
    if (self == NULL || other == NULL)
        return false;

    if (other->type != OBJ_IDENT)
        return false;

    IdentObject* otherIdentObject = other->object;

    return type_equals(&self->type, &otherIdentObject->type) &&
        object_equals(self->value, otherIdentObject->value);
}

void ident_object_to_string(ByteBuffer* byteBuffer, IdentObject** identObject) {
    if (byteBuffer == NULL || identObject == NULL || *identObject == NULL)
        return;

    object_to_string(byteBuffer, &(*identObject)->value);
}

void ident_object_free(IdentObject** identObject) {
    if (identObject == NULL || *identObject == NULL)
        return;

    type_free(&(*identObject)->type);
    safe_free((void**) &(*identObject)->name);
    object_free(&(*identObject)->value);

    safe_free((void**) identObject);
}

IntegerObject* integer_object_new(Type* type, int value) {
    IntegerObject* new_integer_object = NULL;
    new_integer_object = safe_malloc(sizeof(IntegerObject), NULL);
    if (new_integer_object == NULL) {
        return NULL;
    }

    *new_integer_object = (IntegerObject) {
        .type = type,
        .value = value
    };

    return new_integer_object;
}

Type* integer_object_get_type(IntegerObject* integerObject) {
    if (integerObject == NULL)
        return NULL;

    return integerObject->type;
}

IntegerObject* integer_object_copy(IntegerObject* self) {
    if (self == NULL)
        return NULL;

    return integer_object_new(type_copy((const Type**) &self->type), self->value);
}

bool integer_object_equals(IntegerObject* self, Object* other) {
    if (self == NULL || other == NULL)
        return false;

    if (other->type != OBJ_INTEGER)
        return false;

    IntegerObject* otherIntegerObject = (IntegerObject*)other->object;

    return self->value == otherIntegerObject->value;
}

void integer_object_to_string(ByteBuffer* byteBuffer, IntegerObject** integerObject) {
    if (byteBuffer == NULL || integerObject == NULL || *integerObject == NULL)
        return;

    byte_buffer_appendf(byteBuffer, "%d", (*integerObject)->value);
}

void integer_object_free(IntegerObject** integerObject) {
    if (integerObject == NULL || *integerObject == NULL)
        return;

    type_free(&(*integerObject)->type);

    safe_free((void**) integerObject);
}

FloatObject* float_object_new(Type* type, double value) {
    FloatObject* new_float_object = safe_malloc(sizeof(FloatObject), NULL);
    if (new_float_object == NULL) {
        return NULL;
    }

    *new_float_object = (FloatObject) {
        .type = type,
        .value = value
    };

    return new_float_object;
}

Type* float_object_get_type(FloatObject* floatObject) {
    if (floatObject == NULL)
        return NULL;

    return floatObject->type;
}

FloatObject* float_object_copy(FloatObject* self) {
    if (self == NULL)
        return NULL;

    return float_object_new(type_copy((const Type**) &self->type), self->value);
}

static bool compare_double(double a, double b) {
    return memcmp(&a, &b, sizeof(double)) == 0;
}

bool float_object_equals(FloatObject* self, Object* other) {
    if (self == NULL || other == NULL)
        return false;

    if (other->type != OBJ_FLOAT)
        return false;

    FloatObject* otherFloatObject = (FloatObject*)other->object;

    return compare_double(self->value, otherFloatObject->value);
}

void float_object_to_string(ByteBuffer* byteBuffer, FloatObject** floatObject) {
    if (byteBuffer == NULL || floatObject == NULL || *floatObject == NULL)
        return;

    byte_buffer_appendf(byteBuffer, "%f", (*floatObject)->value);
}

void float_object_free(FloatObject** floatObject) {
    if (floatObject == NULL || *floatObject == NULL)
        return;

    type_free(&(*floatObject)->type);

    safe_free((void**) floatObject);
}

CharacterObject* character_object_new(Type* type, char value) {
    CharacterObject* new_char_object = NULL;
    new_char_object = safe_malloc(sizeof(CharacterObject), NULL);
    if (new_char_object == NULL) {
        return NULL;
    }

    *new_char_object = (CharacterObject) {
        .type = type,
        .value = value
    };

    return new_char_object;
}

Type* character_object_get_type(CharacterObject* charObject) {
    if (charObject == NULL)
        return NULL;

    return charObject->type;
}

CharacterObject* character_object_copy(CharacterObject* self) {
    if (self == NULL)
        return NULL;

    return character_object_new(type_copy((const Type**) &self->type), self->value);
}

bool character_object_equals(CharacterObject* self, Object* other) {
    if (self == NULL || other == NULL)
        return false;

    if (other->type != OBJ_CHARACTER)
        return false;

    CharacterObject* otherCharObject = (CharacterObject*) other->object;

    return self->value == otherCharObject->value;
}

void character_object_to_string(ByteBuffer* byteBuffer, CharacterObject** characterObject) {
    if (byteBuffer == NULL || characterObject == NULL || *characterObject == NULL)
        return;

    byte_buffer_appendf(byteBuffer, "%c", (*characterObject)->value);
}

void character_object_free(CharacterObject** characterObject) {
    if (characterObject == NULL || *characterObject == NULL)
        return;

    type_free(&(*characterObject)->type);

    safe_free((void**) characterObject);
}

StringObject* string_object_new(Type* type, char* value) {
    StringObject* new_string_object = NULL;
    new_string_object = safe_malloc(sizeof(StringObject), NULL);
    if (new_string_object == NULL) {
        return NULL;
    }

    *new_string_object = (StringObject) {
        .type = type,
        .value = str_dup(value)
    };

    return new_string_object;
}

Type* string_object_get_type(StringObject* stringObject) {
    if (stringObject == NULL)
        return NULL;

    return stringObject->type;
}

StringObject* string_object_copy(StringObject* self) {
    if (self == NULL)
        return NULL;

    return string_object_new(type_copy((const Type**) &self->type), self->value);
}

bool string_object_equals(StringObject* self, Object* other) {
    if (self == NULL || other == NULL)
        return false;

    if (other->type != OBJ_STRING)
        return false;

    StringObject* otherStringObject = (StringObject*) other->object;

    return strcmp(self->value, otherStringObject->value) == 0;
}

void string_object_to_string(ByteBuffer* byteBuffer, StringObject** stringObject) {
    if (byteBuffer == NULL || stringObject == NULL || *stringObject == NULL)
        return;


    const char* str = (*stringObject)->value;
    size_t len = strlen(str);

    for (size_t i = 0; i < len; i++) {
        char c = str[i];

        switch (c) {
        case '\\':
            if (i + 1 < len) {
                char next = str[i + 1];
                switch (next) {
                case 'n':
                    byte_buffer_appendf(byteBuffer, "\n", "");
                    i++;
                    break;
                case 't':
                    byte_buffer_appendf(byteBuffer, "\t", "");
                    i++;
                    break;
                case '\"':
                    byte_buffer_appendf(byteBuffer, "\"", "");
                    i++;
                    break;
                case '\'':
                    byte_buffer_appendf(byteBuffer, "\'", "");
                    i++;
                    break;
                case '\\':
                    byte_buffer_appendf(byteBuffer, "\\", "");
                    i++;
                    break;
                default:
                    byte_buffer_appendf(byteBuffer, "%c", c);
                    break;
                }
            } else {
                byte_buffer_appendf(byteBuffer, "%c", c);
            }
            break;
        default:
            byte_buffer_appendf(byteBuffer, "%c", c);
            break;
        }
    }
}

void string_object_free(StringObject** stringObject) {
    if (stringObject == NULL || *stringObject == NULL)
        return;

    type_free(&(*stringObject)->type);
    safe_free((void**) &(*stringObject)->value);

    safe_free((void**) stringObject);
}

BooleanObject* boolean_object_new(Type* type, bool value) {
    BooleanObject* new_boolean_object = NULL;
    new_boolean_object = safe_malloc(sizeof(BooleanObject), NULL);
    if (new_boolean_object == NULL) {
        return NULL;
    }

    *new_boolean_object = (BooleanObject) {
        .type = type,
        .value = value
    };

    return new_boolean_object;
}

Type* boolean_object_get_type(BooleanObject* booleanObject) {
    if (booleanObject == NULL)
        return NULL;

    return booleanObject->type;
}

BooleanObject* boolean_object_copy(BooleanObject* self) {
    if (self == NULL)
        return NULL;

    return boolean_object_new(type_copy((const Type**) &self->type), self->value);
}

bool boolean_object_equals(BooleanObject* self, Object* other) {
    if (self == NULL || other == NULL)
        return false;

    if (other->type != OBJ_BOOLEAN)
        return false;

    BooleanObject* otherBooleanObject = (BooleanObject*) other->object;

    return self->value == otherBooleanObject->value;
}

void boolean_object_to_string(ByteBuffer* byteBuffer, BooleanObject** booleanObject) {
    if (byteBuffer == NULL || booleanObject == NULL || *booleanObject == NULL)
        return;

    const char* value_str = (*booleanObject)->value ? "true" : "false";

    byte_buffer_appendf(byteBuffer, "%s", value_str);
}

void boolean_object_free(BooleanObject** booleanObject) {
    if (booleanObject == NULL || *booleanObject == NULL)
        return;

    type_free(&(*booleanObject)->type);

    safe_free((void**) booleanObject);
}

NilObject* nil_object_new(Type* type) {
    NilObject* new_nil_object = NULL;
    new_nil_object = safe_malloc(sizeof(NilObject), NULL);
    if (new_nil_object == NULL) {
        return NULL;
    }

    *new_nil_object = (NilObject) {
        .type = type
    };

    return new_nil_object;
}

Type* nil_object_get_type(NilObject* nilObject) {
    if (nilObject == NULL)
        return NULL;

    return nilObject->type;
}

NilObject* nil_object_copy(NilObject* self) {
    if (self == NULL)
        return NULL;

    return nil_object_new(type_copy((const Type**) &self->type));
}

bool nil_object_equals(NilObject* self, Object* other) {
    if (self == NULL || other == NULL)
        return false;

    return other->type == OBJ_NIL;
}

void nil_object_to_string(ByteBuffer* byteBuffer, NilObject** nilObject) {
    if (byteBuffer == NULL || nilObject == NULL || *nilObject == NULL)
        return;

    byte_buffer_appendf(byteBuffer, "%s", "nil");
}

void nil_object_free(NilObject** nilObject) {
    if (nilObject == NULL || *nilObject == NULL)
        return;

    type_free(&(*nilObject)->type);

    safe_free((void**) nilObject);
}

ReturnObject* return_object_new(Object* value) {
    ReturnObject* new_return_object = NULL;
    new_return_object = safe_malloc(sizeof(ReturnObject), NULL);
    if (new_return_object == NULL) {
        return NULL;
    }

    *new_return_object = (ReturnObject) {
        .value = value
    };

    return new_return_object;
}

ReturnObject* return_object_copy(ReturnObject* self) {
    if (self == NULL)
        return NULL;

    Object* value_copy = object_copy(self->value);
    if (value_copy == NULL) {
        return NULL;
    }

    return return_object_new(value_copy);
}

bool return_object_equals(ReturnObject* self, Object* other) {
    if (self == NULL || other == NULL)
        return false;

    if (other->type != OBJ_RETURN)
        return false;

    ReturnObject* otherReturnObject = (ReturnObject*) other->object;

    return object_equals(self->value, otherReturnObject->value);
}

void return_object_to_string(ByteBuffer* byteBuffer, ReturnObject** returnObject) {
    if (byteBuffer == NULL || returnObject == NULL || *returnObject == NULL)
        return;

    object_to_string(byteBuffer, &(*returnObject)->value);
}

void return_object_free(ReturnObject** returnObject) {
    if (returnObject == NULL || *returnObject == NULL)
        return;

    object_free(&(*returnObject)->value);
    safe_free((void**) returnObject);
}

BreakObject* break_object_new(void) {
    BreakObject* new_break_object = NULL;
    new_break_object = safe_malloc(sizeof(BreakObject), NULL);
    if (new_break_object == NULL) {
        return NULL;
    }

    *new_break_object = (BreakObject) {};

    return new_break_object;
}

BreakObject* break_object_copy(BreakObject* self) {
    return self;
}

void break_object_free(BreakObject** breakObject) {
    if (breakObject == NULL || *breakObject == NULL)
        return;

    safe_free((void**) breakObject);
}

ContinueObject* continue_object_new(void) {
    ContinueObject* new_continue_object = NULL;
    new_continue_object = safe_malloc(sizeof(ContinueObject), NULL);
    if (new_continue_object == NULL) {
        return NULL;
    }

    *new_continue_object = (ContinueObject) {};

    return new_continue_object;
}

ContinueObject* continue_object_copy(ContinueObject* self) {
    return self;
}

void continue_object_free(ContinueObject** continueObject) {
    if (continueObject == NULL || *continueObject == NULL)
        return;

    safe_free((void**) continueObject);
}

FunctionObject* function_object_new(Type* type, Context* env, List* parameters, Stmt* body) {
    FunctionObject* new_function_object = NULL;
    new_function_object = safe_malloc(sizeof(FunctionObject), NULL);
    if (new_function_object == NULL) {
        type_free(&type);
        context_free(&env);
        list_free(&parameters);
        stmt_free(&body);
        return NULL;
    }

    *new_function_object = (FunctionObject) {
        .type = type,
        .env = env,
        .parameters = parameters,
        .body = body
    };

    return new_function_object;
}

Type* function_object_get_type(FunctionObject* self) {
    if (self == NULL)
        return NULL;

    return self->type;
}

bool function_object_equals(FunctionObject* self, Object* other) {
    if (other == NULL || other->type != OBJ_FUNCTION)
        return false;

    FunctionObject* otherFunctionObject = other->object;

    return type_equals(&self->type, &otherFunctionObject->type);
}

void function_object_to_string(ByteBuffer* byteBuffer, FunctionObject** functionObject) {
    if (byteBuffer == NULL || functionObject == NULL || *functionObject == NULL)
        return;

    byte_buffer_appendf(byteBuffer, "[Function: %p]", *functionObject);
}

void function_object_free(FunctionObject** functionObject) {
    if (functionObject == NULL || *functionObject == NULL)
        return;

    type_free(&(*functionObject)->type);
    context_free(&(*functionObject)->env);
    list_free(&(*functionObject)->parameters);
    stmt_free(&(*functionObject)->body);

    safe_free((void**) functionObject);
}

static bool entry_cmp(const MapEntry** entry, char** key) {
    return strcmp((*entry)->key, *key) == 0;
}

static Context* extend_function_env(FunctionObject* functionObject, List* arguments) {
    Context* functionEnv = context_enclosed_new(
        functionObject->env,
        MAP_NEW(32, entry_cmp, NULL, NULL)
    );

    size_t paramIndex = 0;

    list_foreach(parameter, functionObject->parameters) {
        FieldDecl* paramDecl = ((Decl*) parameter->value)->decl;
        char* parameterName = paramDecl->name->literal;

        context_define(
            functionEnv,
            parameterName,
            list_get_at(&arguments, paramIndex)
        );

        paramIndex++;
    }

    return functionEnv;
}

static Object* unwrap_return_value(Object* object) {
    if (object->type == OBJ_RETURN) {
        ReturnObject* returnObject = object->object;
        Object* result = returnObject->value;
        returnObject->value = NULL;
        object_free(&object);
        return result;
    }

    return object;
}

Object* function_object_run(Interpreter* interpreter, FunctionObject* functionObject, List* arguments) {
    Context* previous = interpreter->env;

    Context* innerEnv = extend_function_env(functionObject, arguments);

    interpreter->env = innerEnv;

    Object* result = eval_stmt(interpreter, functionObject->body);

    interpreter->env = previous;

    return unwrap_return_value(result);
}

Callable* callable_new(Object* functionObject,
    Object* (*function)(struct Interpreter*, FunctionObject*, List*),
    void (*to_string)(ByteBuffer*, void**),
    void (*destroy)(void**))
{
    Callable* new_callable = NULL;
    new_callable = safe_malloc(sizeof(Callable), NULL);
    if (new_callable == NULL) {
        if (destroy != NULL) {
            destroy((void**) &functionObject);
        }
        return NULL;
    }

    *new_callable = (Callable) {
        .functionObject = functionObject,
        .function = function,
        .to_string = to_string,
        .destroy = destroy
    };

    return new_callable;
}

Object* callable_run(Interpreter* interpreter, Object* callable, List* arguments) {
    if (interpreter == NULL || callable == NULL || arguments == NULL)
        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "callable_run: cannot execute callable function");

    if (callable->type != OBJ_CALLABLE)
        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "callable_run: cannot execute callable function");

    Callable* callableObject = callable->object;

    if (callableObject->functionObject != NULL && callableObject->functionObject->type != OBJ_FUNCTION)
        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "callable_run: cannot execute callable function");

    FunctionObject* functionObject = NULL;
    if (callableObject->functionObject != NULL)
        functionObject = callableObject->functionObject->object;

    Object* result = callableObject->function(interpreter, functionObject, arguments);

    return result;
}

void callable_to_string(ByteBuffer* byteBuffer, Callable** callable) {
    if (byteBuffer == NULL || callable == NULL || *callable == NULL || (*callable)->to_string == NULL)
        return;

    (*callable)->to_string(byteBuffer, (void**) &(*callable)->functionObject);
}

void callable_free(Callable** callable) {
    if (callable == NULL || *callable == NULL)
        return;

    if ((*callable)->destroy != NULL) {
        (*callable)->destroy((void**) &(*callable)->functionObject);
    }

    safe_free((void**) callable);
}

Object* print_function_run(Interpreter* interpreter, FunctionObject* functionObject, List* arguments) {
    if (arguments == NULL)
        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "print_function_run: invalid arguments");

    ByteBuffer* bb = byte_buffer_new();
    char* str = NULL;

    list_foreach(argument, arguments) {
        Object* obj = argument->value;

        object_to_string(bb, &obj);
    }

    str = byte_buffer_to_string(bb);

    printf("%s", str);

    safe_free((void**) &str);

    byte_buffer_free(&bb);

    return NULL;
}

Object* println_function_run(Interpreter* interpreter, FunctionObject* functionObject, List* arguments) {
    if (arguments == NULL)
        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "print_function_run: invalid arguments");

    ByteBuffer* bb = byte_buffer_new();
    char* str = NULL;

    list_foreach(argument, arguments) {
        Object* obj = argument->value;

        object_to_string(bb, &obj);
    }

    str = byte_buffer_to_string(bb);

    printf("%s\n", str);

    safe_free((void**) &str);

    byte_buffer_free(&bb);

    return NULL;
}

Object* input_function_run(Interpreter* interpreter, FunctionObject* functionObject, List* arguments) {
    if (arguments == NULL)
        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "input_function_run: invalid arguments");

    ByteBuffer* bb = byte_buffer_new();
    char* str = NULL;

    list_foreach(argument, arguments) {
        Object* obj = argument->value;

        object_to_string(bb, &obj);
    }

    str = byte_buffer_to_string(bb);

    printf("%s", str);

    safe_free((void**) &str);

    byte_buffer_free(&bb);

    char* input = NULL;
    size_t size = 0;
    ssize_t read = getline(&input, &size, stdin);

    if (read == -1) {
        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "input_function_run: error while trying to read from stdin");
    }

    size_t len = strlen(input);

    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }

    Object* result = NEW_STRING_OBJECT(input);
    safe_free((void**) &input);

    return result;
}

