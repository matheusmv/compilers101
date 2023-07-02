#include "interpreter.h"

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "buffer.h"
#include "context.h"
#include "list.h"
#include "literal-type.h"
#include "map.h"
#include "object.h"
#include "smem.h"
#include "token.h"
#include "type-checker.h"
#include "types.h"


static TypeChecker* types = NULL;

static const Context* globalEnv = NULL;

static const Object* TRUE_OBJECT     = NULL;
static const Object* FALSE_OBJECT    = NULL;
static const Object* NIL_OBJECT      = NULL;
static const Object* BREAK_OBJECT    = NULL;
static const Object* CONTINUE_OBJECT = NULL;

static Object* eval_binary_expr(Interpreter* interpreter, Type* type, Object* left, Token* operation, Object* right);
static Object* eval_assign_expr(Interpreter* interpreter, Token* op, char* ident, Object* value);
static Object* eval_literal_expr(Interpreter* interpreter, LiteralExpr* literalExpr);


static bool isInteger(const char* str) {
    char* endptr;
    strtol(str, &endptr, 10);
    return (*endptr == '\0');
}

static bool isFloat(const char* str) {
    char* endptr;
    strtod(str, &endptr);
    return (*endptr == '\0');
}

static bool isChar(const char* str) {
    return (strlen(str) == 1);
}

static bool isBool(const char* str) {
    bool isTrue = strcmp(str, "true") == 0;
    bool isFalse = strcmp(str, "false") == 0;
    return isTrue || isFalse;
}

static bool is_error(Interpreter* interpreter, Object* object);
static void log_error(Error* error);

static Object* eval_to_bool_obj(bool input);
static bool is_thruthy(Object* object);

static Object* eval_binary_op(ObjectType returnType, Token* op, double left, double right);

static Type* get_operation_type(Token* operation, Object* left, Object* right);

static bool entry_cmp(const MapEntry** entry, char** key) {
    return strcmp((*entry)->key, *key) == 0;
}

static Interpreter* interpreter_init(void) {
    Interpreter* interpreter = NULL;
    interpreter = safe_malloc(sizeof(Interpreter), NULL);
    if (interpreter == NULL) {
        return NULL;
    }

    *interpreter = (Interpreter) {
        .env = NULL,
        .exitCode = INTERPRETER_SUCCESS
    };

    return interpreter;
}

static void interpreter_free(Interpreter** interpreter) {
    if (interpreter == NULL || *interpreter == NULL)
        return;

    context_free(&(*interpreter)->env);

    safe_free((void**) interpreter);
}

InterpreterStatus eval(List* declarations) {
    if (declarations == NULL)
        return INTERPRETER_SUCCESS;

    if (init_and_check(&types, declarations) == TYPE_CHECKER_FAILURE) {
        type_checker_destroy(&types);
        return INTERPRETER_FAILURE;
    }

    globalEnv = context_new(MAP_NEW(32, entry_cmp, NULL, NULL));

    context_define((Context*) globalEnv, "print", NEW_PRINT_FUNC());
    context_define((Context*) globalEnv, "println", NEW_PRINTLN_FUNC());
    context_define((Context*) globalEnv, "input", NEW_INPUT_FUNC());
    context_define((Context*) globalEnv, "len", NEW_LEN_FUNC());

    TRUE_OBJECT     = NEW_BOOLEAN_OBJECT(true);
    FALSE_OBJECT    = NEW_BOOLEAN_OBJECT(false);
    NIL_OBJECT      = NEW_NIL_OBJECT();
    BREAK_OBJECT    = NEW_BREAK_OBJECT();
    CONTINUE_OBJECT = NEW_CONTINUE_OBJECT();

    Interpreter* interpreter = interpreter_init();
    interpreter->env = (Context*) globalEnv;

    // ByteBuffer* bb = byte_buffer_new();
    // char* str_out = NULL;

    list_foreach(declaration, declarations) {
        Object* res = eval_decl(interpreter, declaration->value);
        if (res != NULL && is_error(interpreter, res)) {
            log_error(res->object);
            continue;
        }

        // if (res == NULL)
        //     continue;

        // object_to_string(bb, &res);
        // str_out = byte_buffer_to_string(bb);

        // decl_to_string((Decl**) &declaration->value);
        // printf(" ---> %s\n", str_out);

        // if (str_out != NULL) {
        //     safe_free((void**) &str_out);
        //     byte_buffer_clear(bb);
        // }
    }

    // byte_buffer_free(&bb);

    object_free((Object**) &TRUE_OBJECT);
    object_free((Object**) &FALSE_OBJECT);
    object_free((Object**) &NIL_OBJECT);
    object_free((Object**) &BREAK_OBJECT);
    object_free((Object**) &CONTINUE_OBJECT);

    InterpreterStatus status = interpreter->exitCode;

    interpreter_free(&interpreter);

    // type_checker_destroy(&types);

    return status;
}

static Context* extend_function_env(FunctionObject* functionObject, List* arguments) {
    Context* functionEnv = context_enclosed_new(
        functionObject->env,
        MAP_NEW(32, entry_cmp, NULL, NULL)
    );

    size_t paramIndex = 0;
    list_foreach(parameter, functionObject->parameters) {
        FieldDecl* parameterDecl = ((Decl*) parameter->value)->decl;
        char* parameterName = parameterDecl->name->literal;

        context_define(
            functionEnv,
            parameterName,
            list_get_at(&arguments, paramIndex)
        );

        paramIndex++;
    }

    return functionEnv;
}

Object* eval_decl(Interpreter* interpreter, Decl* declaration) {
    if (interpreter == NULL || declaration == NULL)
        return NULL;

    switch (declaration->type) {
    case LET_DECL: {
        LetDecl* letDecl = declaration->decl;
        char* varName = letDecl->name->literal;

        if (context_exists(interpreter->env, varName)) {
            ByteBuffer* bb = byte_buffer_new();

            byte_buffer_appendf(bb, "%s: already defined", varName);
            char* error_message = byte_buffer_to_string(bb);
            byte_buffer_free(&bb);

            Object* error = NEW_ERROR_OBJECT(RUNTIME_ERROR, error_message);
            safe_free((void**) &error_message);

            return error;
        }

        Type* identType = letDecl->type;
        Object* identValue = eval_expr(interpreter, letDecl->expression);

        // Object* newIdentObject = NEW_IDENT_OBJECT(identType, varName, identValue);

        context_define(interpreter->env, varName, identValue);

        Object* ok = context_get(interpreter->env, varName);

        return identValue;
    }
    case CONST_DECL: {
        return NULL;
    }
    case FIELD_DECL: {
        return NULL;
    }
    case FUNC_DECL: {
        FunctionDecl* functionDecl = declaration->decl;
        char* functionName = functionDecl->name->literal;

        if (context_exists(interpreter->env, functionName)) {
            ByteBuffer* bb = byte_buffer_new();

            byte_buffer_appendf(bb, "%s: already defined", functionName);
            char* error_message = byte_buffer_to_string(bb);
            byte_buffer_free(&bb);

            Object* error = NEW_ERROR_OBJECT(RUNTIME_ERROR, error_message);
            safe_free((void**) &error_message);

            return error;
        }

        Type* functionType = get_decl_type(types, declaration);
        Context* functionEnv = interpreter->env;
        List* functionParameters = functionDecl->parameters;
        Stmt* functionBody = functionDecl->body;

        Object* functionObject = NEW_FUNCTION_OBJECT(functionType, functionEnv, functionParameters, functionBody);
        Object* callableFunction = NEW_CALLABLE_OBJECT(functionObject);

        context_define(interpreter->env, functionName, callableFunction);

        return functionObject;
    }
    case STRUCT_DECL: {
        return NULL;
    }
    case STMT_DECL: {
        StmtDecl* stmtDecl = declaration->decl;

        return eval_stmt(interpreter, stmtDecl->stmt);
    }
    default:
        return NULL;
    }
}

Object* eval_stmt(Interpreter* interpreter, Stmt* statement) {
    if (interpreter == NULL || statement == NULL)
        return NULL;

    switch (statement->type) {
    case BLOCK_STMT: {
        Context* previous = interpreter->env;

        interpreter->env = context_enclosed_new(
            previous,
            MAP_NEW(32, entry_cmp, NULL, NULL)
        );

        BlockStmt* blockStmt = statement->stmt;
        Object* result = NULL;

        bool isContinue = false;

        list_foreach(declaration, blockStmt->declarations) {
            if (!isContinue) {
                result = eval_decl(interpreter, declaration->value);
            } else {
                isContinue = false;
            }

            if (result != NULL && result->type == OBJ_RETURN) {
                return result;
            }

            if (result != NULL && result->type == OBJ_BREAK) {
                return result;
            }

            if (result != NULL && result->type == OBJ_ERROR) {
                return result;
            }

            isContinue = result != NULL && result->type == OBJ_CONTINUE;
        }

        context_free(&interpreter->env);

        interpreter->env = previous;

        return result;
    }
    case RETURN_STMT: {
        ReturnStmt* returnStmt = statement->stmt;

        Object* result = eval_expr(interpreter, returnStmt->expression);

        if (is_error(interpreter, result)) {
            log_error(result->object);
            return result;
        }

        return NEW_RETURN_OBJECT(result);
    }
    case BREAK_STMT: {
        return BREAK_OBJECT;
    }
    case CONTINUE_STMT: {
        return CONTINUE_OBJECT;
    }
    case IF_STMT: {
        IfStmt* ifStmt = statement->stmt;

        Object* condition = eval_expr(interpreter, ifStmt->condition);
        if (is_error(interpreter, condition)) {
            log_error(condition->object);
            return condition;
        }

        Context* previous = interpreter->env;

        interpreter->env = context_enclosed_new(
            previous,
            MAP_NEW(32, entry_cmp, NULL, NULL)
        );

        Object* result = NULL;

        if (is_thruthy(condition)) {
            result = eval_stmt(interpreter, ifStmt->thenBranch);
        } else if (ifStmt->elseBranch) {
            result = eval_stmt(interpreter, ifStmt->elseBranch);
        }

        interpreter->env = previous;

        if (is_error(interpreter, result)) {
            log_error(result->object);
            return result;
        }

        return result;
    }
    case WHILE_STMT: {
        WhileStmt* whileStmt = statement->stmt;

        Object* result = NULL;

        while (is_thruthy(eval_expr(interpreter, whileStmt->condition))) {
            result = eval_stmt(interpreter, whileStmt->body);

            if (result != NULL && result->type == OBJ_RETURN) {
                return result;
            }

            if (result != NULL && result->type == OBJ_BREAK) {
                return result;
            }
        }

        return NIL_OBJECT;
    }
    case FOR_STMT: {
        ForStmt* forStmt = statement->stmt;

        Context* previous = interpreter->env;

        interpreter->env = context_enclosed_new(
            previous,
            MAP_NEW(32, entry_cmp, NULL, NULL)
        );

        Object* init = eval_decl(interpreter, forStmt->initialization);
        if (is_error(interpreter, init)) {
            log_error(init->object);
            return init;
        }

        Object* result = NULL;

        while(is_thruthy(eval_expr(interpreter, forStmt->condition))) {
            result = eval_stmt(interpreter, forStmt->body);
            if (is_error(interpreter, result)) {
                log_error(result->object);
                return result;
            }

            if (result != NULL && result->type == OBJ_RETURN) {
                break;
            }

            if (result != NULL && result->type == OBJ_BREAK) {
                break;
            }

            if (forStmt->action) {
                result = eval_expr(interpreter, forStmt->action);
                if (is_error(interpreter, result)) {
                    log_error(result->object);
                    interpreter->env = previous;
                    return result;
                }
            }
        }

        interpreter->env = previous;

        return result;
    }
    case EXPRESSION_STMT: {
        ExpressionStmt* exprStmt = statement->stmt;

        return eval_expr(interpreter, exprStmt->expression);
    }
    default:
        return NULL;
    }
}

Object* eval_expr(Interpreter* interpreter, Expr* expression) {
    if (interpreter == NULL || expression == NULL)
        return NULL;

    switch (expression->type) {
    case BINARY_EXPR: {
        BinaryExpr* binaryExpr = expression->expr;

        Object* left = eval_expr(interpreter, binaryExpr->left);
        if (is_error(interpreter, left)) {
            log_error(left->object);
            return left;
        }

        Object* right = eval_expr(interpreter, binaryExpr->right);
        if (is_error(interpreter, right)) {
            log_error(right->object);
            return right;
        }

        // Context* out = types->env;
        // types->env = interpreter->env;

        // Type* resultType = get_expr_type(types, expression);

        // if (resultType == NULL) {
            Type* resultType = resultType = get_operation_type(binaryExpr->op, left, right);
        // }

        // types->env = out;

        Token* operation = binaryExpr->op;

        Object* result = eval_binary_expr(interpreter, resultType, left, operation, right);
        if (is_error(interpreter, result)) {
            log_error(result->object);
            return result;
        }

        return result;
    }
    case GROUP_EXPR: {
        GroupExpr* groupExpr = expression->expr;

        Object* result = eval_expr(interpreter, groupExpr->expression);
        if (is_error(interpreter, result)) {
            log_error(result->object);
            return result;
        }

        return result;
    }
    case ASSIGN_EXPR: {
        AssignExpr* assignExpr = expression->expr;

        if (assignExpr != NULL && assignExpr->identifier != NULL && assignExpr->identifier->type == ARRAY_MEMBER_EXPR) {
            ArrayMemberExpr* arrayMember = assignExpr->identifier->expr;

            Object* array = eval_expr(interpreter, arrayMember->object);
            if (is_error(interpreter, array)) {
                log_error(array->object);
                return array;
            }

            Object* ident = array;

            list_foreach(level, arrayMember->levelOfAccess) {
                Object* index = eval_expr(interpreter, level->value);

                if (ident != NULL && ident->type == OBJ_ARRAY) {
                    ident = array_object_get_object_at(ident->object, ((IntegerObject*) index->object)->value);
                }
            }

            if (is_error(interpreter, ident)) {
                log_error(ident->object);
                return ident;
            }

            Object* value = eval_expr(interpreter, assignExpr->expression);
            if (is_error(interpreter, value)) {
                log_error(value->object);
                return value;
            }

            Type* identType = object_get_type(ident);
            Type* valueType = object_get_type(value);
            if (!type_equals(&identType, &valueType)) {
                type_to_string(&identType);
                printf("\n");
                type_to_string(&valueType);
                printf("\n");
                return NEW_ERROR_OBJECT(RUNTIME_ERROR, "invalid assign: type mismatch");
            }

            // TODO: implement remaining operators
            ident->object = value->object;

            return ident;
        }

        Object* ident = eval_expr(interpreter, assignExpr->identifier);
        if (is_error(interpreter, ident)) {
            log_error(ident->object);
            return ident;
        }

        Object* value = eval_expr(interpreter, assignExpr->expression);
        if (is_error(interpreter, value)) {
            log_error(value->object);
            return value;
        }

        char* identName = ((IdentLiteral*) ((Expr*) assignExpr->identifier->expr)->expr)->value;

        Object* result = eval_assign_expr(
            interpreter,
            assignExpr->op,
            identName,
            value
        );
        if (is_error(interpreter, result)) {
            log_error(result->object);
            return result;
        }

        return result;
    }
    case CALL_EXPR: {
        CallExpr* callExpr = expression->expr;

        Object* callable = eval_expr(interpreter, callExpr->callee);
        if (is_error(interpreter, callable)) {
            log_error(callable->object);
            return callable;
        }

        List* arguments = list_new(NULL);

        list_foreach(argument, callExpr->arguments) {
            Object* value = eval_expr(interpreter, argument->value);
            if (is_error(interpreter, value)) {
                log_error(value->object);
                list_free(&arguments);
                return value;
            }

            list_insert_last(&arguments, value);
        }

        Object* result = callable_run(interpreter, callable, arguments);

        list_free(&arguments);

        return result;
    }
    case LOGICAL_EXPR: {
        LogicalExpr* logicalExpr = expression->expr;

        Object* left = eval_expr(interpreter, logicalExpr->left);
        if (is_error(interpreter, left)) {
            log_error(left->object);
            return left;
        }

        Object* right = eval_expr(interpreter, logicalExpr->right);
        if (is_error(interpreter, right)) {
            log_error(right->object);
            return right;
        }

        Object* result = NULL;

        if (logicalExpr->op->type == TOKEN_LAND) {
            result = eval_to_bool_obj(is_thruthy(left) && is_thruthy(right));
        }

        if (logicalExpr->op->type == TOKEN_LOR) {
            result = eval_to_bool_obj(is_thruthy(left) || is_thruthy(right));
        }

        if (is_error(interpreter, result)) {
            log_error(result->object);
            return result;
        }

        return result;
    }
    case UNARY_EXPR: {
        UnaryExpr* unaryExpr = expression->expr;

        Object* right = eval_expr(interpreter, unaryExpr->expression);
        if (is_error(interpreter, right)) {
            log_error(right->object);
            return right;
        }

        TokenType operationType = unaryExpr->op->type;

        if (operationType == TOKEN_ADD) {
            if (right->type == OBJ_INTEGER) {
                IntegerObject* intObj = (IntegerObject*)right->object;
                return NEW_INTEGER_OBJECT(intObj->value);
            }
            if (right->type == OBJ_FLOAT) {
                FloatObject* floatObj = (FloatObject*)right->object;
                return NEW_FLOAT_OBJECT(floatObj->value);
            }
        }

        if (operationType == TOKEN_SUB) {
            if (right->type == OBJ_INTEGER) {
                IntegerObject* intObj = (IntegerObject*)right->object;
                return NEW_INTEGER_OBJECT(-intObj->value);
            }
            if (right->type == OBJ_FLOAT) {
                FloatObject* floatObj = (FloatObject*)right->object;
                return NEW_FLOAT_OBJECT(-floatObj->value);
            }
        }

        if (operationType == TOKEN_TILDE) {
            if (right->type == OBJ_INTEGER) {
                IntegerObject* intObj = (IntegerObject*)right->object;
                return NEW_INTEGER_OBJECT(~intObj->value);
            }
        }

        if (operationType == TOKEN_NOT) {
            return eval_to_bool_obj(!is_thruthy(right));
        }

        ByteBuffer* bb = byte_buffer_new();
        byte_buffer_append(bb, "invalid operation: ", strlen("invalid operation: "));
        byte_buffer_appendf(bb, "%s ", unaryExpr->op->literal);
        object_to_string(bb, &right);
        char* error_message = byte_buffer_to_string(bb);
        byte_buffer_free(&bb);

        Object* error = NEW_ERROR_OBJECT(RUNTIME_ERROR, error_message);

        safe_free((void**) &error_message);

        return error;
    }
    case UPDATE_EXPR: {
        UpdateExpr* updateExpr = expression->expr;

        Object* identValue = eval_expr(interpreter, updateExpr->expression);
        if (is_error(interpreter, identValue)) {
            log_error(identValue->object);
            return identValue;
        }

        char* identName = ((IdentLiteral*) ((Expr*) updateExpr->expression->expr)->expr)->value;
        TokenType operationType = updateExpr->op->type;

        if (operationType == TOKEN_INC) {
            if (identValue->type == OBJ_INTEGER) {
                IntegerObject* intObj = identValue->object;
                return NEW_INTEGER_OBJECT(intObj->value++);
            }
            if (identValue->type == OBJ_FLOAT) {
                FloatObject* floatObj = identValue->object;
                return NEW_FLOAT_OBJECT(floatObj->value++);
            }
        }

        if (operationType == TOKEN_DEC) {
            if (identValue->type == OBJ_INTEGER) {
                IntegerObject* intObj = identValue->object;
                return NEW_INTEGER_OBJECT(intObj->value--);
            }
            if (identValue->type == OBJ_FLOAT) {
                FloatObject* floatObj = identValue->object;
                return NEW_FLOAT_OBJECT(floatObj->value--);
            }
        }

        ByteBuffer* bb = byte_buffer_new();
        byte_buffer_append(bb, "invalid operation: ", strlen("invalid operation: "));
        object_to_string(bb, &identValue);
        byte_buffer_appendf(bb, "%s", updateExpr->op->literal);
        char* error_message = byte_buffer_to_string(bb);
        byte_buffer_free(&bb);

        Object* error = NEW_ERROR_OBJECT(RUNTIME_ERROR, error_message);

        safe_free((void**) &error_message);

        return error;
    }
    case FIELD_INIT_EXPR: {
        return NULL;
    }
    case STRUCT_INIT_EXPR: {
        return NULL;
    }
    case STRUCT_INLINE_EXPR: {
        return NULL;
    }
    case ARRAY_INIT_EXPR: {
        ArrayInitExpr* arrayInitExpr = expression->expr;

        List* objects = list_new(NULL);
        Object* result = NULL;
        list_foreach(element, arrayInitExpr->elements) {
            result = eval_expr(interpreter, element->value);
            if (is_error(interpreter, result)) {
                log_error(result->object);
                return result;
            }

            list_insert_last(&objects, result);
        }

        return NEW_ARRAY_OBJECT(arrayInitExpr->type, objects);
    }
    case FUNC_EXPR: {
        FunctionExpr* functionExpr = expression->expr;

        Type* functionType = get_expr_type(types, expression);
        Context* functionEnv = interpreter->env;
        List* functionParameters = functionExpr->parameters;
        Stmt* functionBody = functionExpr->body;

        Object* functionObject = NEW_FUNCTION_OBJECT(functionType, functionEnv, functionParameters, functionBody);
        Object* callableFunction = NEW_CALLABLE_OBJECT(functionObject);

        return callableFunction;
    }
    case CONDITIONAL_EXPR: {
        ConditionalExpr* conditionalExpr = expression->expr;

        Object* result = NULL;

        if (is_thruthy(eval_expr(interpreter, conditionalExpr->condition))) {
            result = eval_expr(interpreter, conditionalExpr->isTrue);
        } else if (conditionalExpr->isFalse) {
            result = eval_expr(interpreter, conditionalExpr->isFalse);
        }

        if (is_error(interpreter, result)) {
            log_error(result->object);
            return result;
        }

        return result;
    }
    case MEMBER_EXPR: {
        return NULL;
    }
    case ARRAY_MEMBER_EXPR: {
        ArrayMemberExpr* arrayMemberExpr = expression->expr;

        Object* array = eval_expr(interpreter, arrayMemberExpr->object);
        if (is_error(interpreter, array)) {
            log_error(array->object);
            return array;
        }

        if (array_object_get_dimensions(array->object) < list_size(&arrayMemberExpr->levelOfAccess)) {
            return NEW_ERROR_OBJECT(RUNTIME_ERROR, "invalid array access");
        }

        Object* result = array;
        Object* index = NULL;

        list_foreach(level, arrayMemberExpr->levelOfAccess) {
            index = eval_expr(interpreter, level->value);

            if (is_error(interpreter, index)) {
                log_error(index->object);
                return index;
            }

            result = array_object_get_object_at(result->object, ((IntegerObject*) index->object)->value);

            if (is_error(interpreter, result)) {
                log_error(result->object);
                return index;
            }
        }

        return result;
    }
    case CAST_EXPR: {
        CastExpr* castExpr = expression->expr;

        Object* targetValue = eval_expr(interpreter, castExpr->target);
        if (is_error(interpreter, targetValue)) {
            log_error(targetValue->object);
            return targetValue;
        }

        Type* targetType = object_get_type(targetValue);

        Object* result = NULL;

        if (targetValue != NULL && targetType != NULL && targetType->typeId == STRING_TYPE) {
            StringObject* strObj = targetValue->object;

            if (castExpr->type->typeId == STRING_TYPE) {
                return NEW_STRING_OBJECT(strObj->value);
            }

            if (castExpr->type->typeId == INT_TYPE && isInteger(strObj->value)) {
                return NEW_INTEGER_OBJECT(atoi(strObj->value));
            }

            if (castExpr->type->typeId == FLOAT_TYPE && isInteger(strObj->value) || isFloat(strObj->value)) {
                return NEW_FLOAT_OBJECT(atof(strObj->value));
            }

            if (castExpr->type->typeId == CHAR_TYPE && isChar(strObj->value)) {
                return NEW_CHARACTER_OBJECT(strObj->value[0]);
            }

            if (castExpr->type->typeId == BOOL_TYPE && isBool(strObj->value)) {
                return NEW_BOOLEAN_OBJECT(strcmp(strObj->value, "true") == 0);
            }
        }

        if (targetValue != NULL && targetType != NULL && targetType->typeId == INT_TYPE) {
            IntegerObject* intObj = targetValue->object;

            if (castExpr->type->typeId == INT_TYPE) {
                return NEW_INTEGER_OBJECT(intObj->value);
            }

            if (castExpr->type->typeId == FLOAT_TYPE) {
                return NEW_FLOAT_OBJECT(intObj->value);
            }
        }

        if (targetValue != NULL && targetType != NULL && targetType->typeId == FLOAT_TYPE) {
            FloatObject* floatObj = targetValue->object;

            if (castExpr->type->typeId == FLOAT_TYPE) {
                return NEW_INTEGER_OBJECT(floatObj->value);
            }

            if (castExpr->type->typeId == INT_TYPE) {
                return NEW_INTEGER_OBJECT((int)floatObj->value);
            }
        }

        if (targetValue != NULL && targetType != NULL && targetType->typeId == CHAR_TYPE) {
            CharacterObject* charObj = targetValue->object;

            if (castExpr->type->typeId == INT_TYPE) {
                return NEW_INTEGER_OBJECT(charObj->value);
            }

            if (castExpr->type->typeId == CHAR_TYPE) {
                return NEW_CHARACTER_OBJECT(charObj->value);
            }

            if (castExpr->type->typeId == STRING_TYPE) {
                return NEW_STRING_OBJECT(&charObj->value);
            }
        }

        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "invalid cast");
    }
    case LITERAL_EXPR: {
        LiteralExpr* literalExpr = expression->expr;

        Object* value = eval_literal_expr(interpreter, literalExpr);

        if (is_error(interpreter, value)) {
            log_error(value->object);
            return value;
        }

        return value;
    }
    default:
        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "cannot eval expression");
    }
}

static Object* eval_literal_expr(Interpreter* interpreter, LiteralExpr* literalExpr) {
    if (interpreter == NULL || literalExpr == NULL) {
        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "cannot determine value of expression");
    }

    switch (literalExpr->type) {
    case IDENT_LITERAL: {
        IdentLiteral* identLiteral = (IdentLiteral*) literalExpr->value;

        void* found_obj = NULL;
        found_obj = context_get(interpreter->env, identLiteral->value);
        if (found_obj == NULL) {
            ByteBuffer* bb = byte_buffer_new();

            byte_buffer_appendf(bb, "undefined: %s", identLiteral->value);

            char* error_message = byte_buffer_to_string(bb);

            byte_buffer_free(&bb);

            Object* error = NEW_ERROR_OBJECT(RUNTIME_ERROR, error_message);

            safe_free((void**) &error_message);

            return error;
        }

        return found_obj;
    }
    case INT_LITERAL: {
        IntLiteral* intLiteral = literalExpr->value;

        return NEW_INTEGER_OBJECT(intLiteral->value);
    }
    case FLOAT_LITERAL: {
        FloatLiteral* floatLiteral = literalExpr->value;

        return NEW_FLOAT_OBJECT(floatLiteral->value);
    }
    case CHAR_LITERAL: {
        CharLiteral* charLiteral = literalExpr->value;

        return NEW_CHARACTER_OBJECT(charLiteral->value);
    }
    case STRING_LITERAL: {
        StringLiteral* stringLiteral = literalExpr->value;

        return NEW_STRING_OBJECT(stringLiteral->value);
    }
    case BOOL_LITERAL: {
        BoolLiteral* boolLiteral = literalExpr->value;

        return (Object*) (boolLiteral->value ? TRUE_OBJECT : FALSE_OBJECT);
    }
    case NIL_LITERAL: {
        return (Object*) NIL_OBJECT;
    }
    default:
        printf("Error: \n\t");
        literal_expr_to_string(&literalExpr);
        printf("\n");

        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "cannot determine value of expression");
    }
}

static bool is_error(Interpreter* interpreter, Object* object) {
    if (object == NULL)
        return false;

    if (object->type == OBJ_ERROR) {
        interpreter->exitCode = INTERPRETER_FAILURE;
        return true;
    }

    return false;
}

static void log_error(Error* error) {
    if (error == NULL)
        return;

    ByteBuffer* bb = byte_buffer_new();

    error_to_string(bb, &error);

    char* error_message = byte_buffer_to_string(bb);

    byte_buffer_free(&bb);

    fprintf(stderr, "%s\n", error_message);

    safe_free((void**) &error_message);
}

static Object* eval_to_bool_obj(bool input) {
    if (input) {
        return (Object*) TRUE_OBJECT;
    }

    return (Object*) FALSE_OBJECT;
}

static bool is_thruthy(Object* object) {
    if (object == NULL)
        return false;

    if (object->type == OBJ_NIL) {
        return false;
    }

    if (object->type == OBJ_BOOLEAN) {
        return ((BooleanObject*) object->object)->value;
    }

    if (object->type == OBJ_INTEGER) {
        return ((IntegerObject*) object->object)->value == 0 ? false : true;
    }

    if (object->type == OBJ_FLOAT) {
        return ((FloatObject*) object->object)->value == 0.0 ? false : true;
    }

    return true;
}

static Object* eval_binary_expr(Interpreter* interpreter, Type* type, Object* left, Token* operation, Object* right) {
    if (interpreter == NULL || type == NULL || left == NULL || operation == NULL || right == NULL) {
        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "eval_binary_expr: invalid operation");
    }

    if (type->typeId == INT_TYPE || type->typeId == FLOAT_TYPE || type->typeId == BOOL_TYPE) {
        if ((left->type == OBJ_STRING || left->type == OBJ_CHARACTER)
            && (right->type == OBJ_STRING || right->type == OBJ_CHARACTER)) {

            if (operation->type == TOKEN_EQL) {
                return NEW_BOOLEAN_OBJECT(object_equals(left, right));
            }

            if (operation->type == TOKEN_NEQ) {
                return NEW_BOOLEAN_OBJECT(!object_equals(left, right));
            }

            return NEW_ERROR_OBJECT(RUNTIME_ERROR, "eval_binary_expr: invalid operation");
        }

        double left_value = 0;
        double right_value = 0;
        ObjectType returnType = OBJ_BOOLEAN;

        if (left->type == right->type) {
            returnType = left->type;
        } else {
            returnType = OBJ_FLOAT;
        }

        if (left->type == OBJ_INTEGER) {
            left_value = ((IntegerObject*)left->object)->value;
        } else if (left->type == OBJ_FLOAT) {
            left_value = ((FloatObject*)left->object)->value;
        } else {
            return NEW_ERROR_OBJECT(RUNTIME_ERROR, "eval_binary_expr: invalid left operand type");
        }

        if (right->type == OBJ_INTEGER) {
            right_value = ((IntegerObject*)right->object)->value;
        } else if (right->type == OBJ_FLOAT) {
            right_value = ((FloatObject*)right->object)->value;
        } else {
            return NEW_ERROR_OBJECT(RUNTIME_ERROR, "eval_binary_expr: invalid right operand type");
        }

        Object* result = eval_binary_op(returnType, operation, left_value, right_value);
        if (is_error(interpreter, result)) {
            log_error(result->object);
            return result;
        }

        return result;
    }

    if (type->typeId == STRING_TYPE) {
        ByteBuffer* bb = byte_buffer_new();
        object_to_string(bb, &left);
        object_to_string(bb, &right);
        char* str = byte_buffer_to_string(bb);
        byte_buffer_free(&bb);

        Object* result = NEW_STRING_OBJECT(str);

        safe_free((void**) &str);

        return result;
    }

    ByteBuffer* bb = byte_buffer_new();
    byte_buffer_append(bb, "invalid operation: ", strlen("invalid operation: "));
    object_to_string(bb, &left);
    byte_buffer_appendf(bb, " %s ", operation->literal);
    object_to_string(bb, &right);
    char* error_message = byte_buffer_to_string(bb);
    byte_buffer_free(&bb);

    Object* error = NEW_ERROR_OBJECT(RUNTIME_ERROR, error_message);

    safe_free((void**) &error_message);

    return error;
}

static Object* eval_assign_expr(Interpreter* interpreter, Token* op, char* ident, Object* value) {
    if (op == NULL)
        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "invalid operation");

    Object* identValue = context_get(interpreter->env, ident);
    if (is_error(interpreter, identValue)) {
        log_error(identValue->object);
        return identValue;
    }

    if (op->type == TOKEN_ASSIGN) {
        context_assign(interpreter->env, ident, value);
        return context_get(interpreter->env, ident);
    }

    double left_value = 0;
    double right_value = 0;
    ObjectType returnType = OBJ_FLOAT;

    // lazy solution

    if (identValue != NULL && identValue->type == OBJ_STRING) {
        if (op->type != TOKEN_ADD_ASSIGN) {
            return NEW_ERROR_OBJECT(RUNTIME_ERROR, "invalid operation");
        }

        if (identValue->type == value->type) {
            returnType = identValue->type;
        } else if (value->type == OBJ_CHARACTER) {
            returnType = OBJ_STRING;
        } else {
            return NEW_ERROR_OBJECT(RUNTIME_ERROR, "invalid operation");
        }
    }

    if (identValue != NULL && identValue->type == OBJ_CHARACTER) {
        if (op->type != TOKEN_ADD_ASSIGN) {
            return NEW_ERROR_OBJECT(RUNTIME_ERROR, "invalid operation");
        }

        if (identValue->type == value->type) {
            returnType = identValue->type;
        } else if (value->type == OBJ_STRING) {
            returnType = OBJ_STRING;
        } else {
            return NEW_ERROR_OBJECT(RUNTIME_ERROR, "invalid operation");
        }
    }

    if (identValue != NULL && identValue->type == OBJ_INTEGER) {
        left_value = ((IntegerObject*)identValue->object)->value;

        if (identValue->type == value->type) {
            right_value = ((IntegerObject*) value->object)->value;
            returnType = identValue->type;
        } else if (value->type == OBJ_FLOAT) {
            right_value = ((FloatObject*) value->object)->value;
            returnType = OBJ_FLOAT;
        } else {
            return NEW_ERROR_OBJECT(RUNTIME_ERROR, "invalid operation");
        }
    }

    if (identValue != NULL && identValue->type == OBJ_FLOAT) {
        left_value = ((FloatObject*)identValue->object)->value;

        if (identValue->type == value->type) {
            right_value = ((FloatObject*) value->object)->value;
            returnType = identValue->type;
        } else if (value->type == OBJ_INTEGER) {
            right_value = ((IntegerObject*) value->object)->value;
            returnType = OBJ_FLOAT;
        } else {
            return NEW_ERROR_OBJECT(RUNTIME_ERROR, "invalid operation");
        }
    }

    Object* result = NULL;

    if (returnType == OBJ_INTEGER || returnType == OBJ_FLOAT) {
        switch (op->type) {
        case TOKEN_ADD_ASSIGN: {
            if (returnType == OBJ_FLOAT)
                result = NEW_FLOAT_OBJECT(left_value + right_value);
            else if (returnType == OBJ_INTEGER)
                result = NEW_INTEGER_OBJECT((int)(left_value + right_value));
            break;
        }
        case TOKEN_SUB_ASSIGN: {
            if (returnType == OBJ_FLOAT)
                result = NEW_FLOAT_OBJECT(left_value - right_value);
            else if (returnType == OBJ_INTEGER)
                result = NEW_INTEGER_OBJECT((int)(left_value - right_value));
            break;
        }
        case TOKEN_MUL_ASSIGN: {
            if (returnType == OBJ_FLOAT)
                result = NEW_FLOAT_OBJECT(left_value * right_value);
            else if (returnType == OBJ_INTEGER)
                result = NEW_INTEGER_OBJECT((int)(left_value * right_value));
            break;
        }
        case TOKEN_QUO_ASSIGN: {
            if (right_value == 0)
                return NEW_ERROR_OBJECT(DIVISION_BY_ZERO_ERROR, "division by zero");
            if (returnType == OBJ_FLOAT)
                result = NEW_FLOAT_OBJECT(left_value / right_value);
            else if (returnType == OBJ_INTEGER)
                result = NEW_INTEGER_OBJECT((int)(left_value / right_value));
            break;
        }
        case TOKEN_REM_ASSIGN: {
            if (right_value == 0)
                return NEW_ERROR_OBJECT(DIVISION_BY_ZERO_ERROR, "division by zero");
            if (returnType == OBJ_FLOAT)
                result = NEW_FLOAT_OBJECT(fmod(left_value, right_value));
            else if (returnType == OBJ_INTEGER)
                result = NEW_INTEGER_OBJECT((int)fmod(left_value, right_value));
            break;
        }
        case TOKEN_AND_ASSIGN: {
            if (returnType == OBJ_INTEGER)
                result = NEW_INTEGER_OBJECT((int)left_value & (int)right_value);
            break;
        }
        case TOKEN_OR_ASSIGN: {
            if (returnType == OBJ_INTEGER)
                result = NEW_INTEGER_OBJECT((int)left_value | (int)right_value);
            break;
        }
        case TOKEN_XOR_ASSIGN: {
            if (returnType == OBJ_INTEGER)
                result = NEW_INTEGER_OBJECT((int)left_value ^ (int)right_value);
            break;
        }
        case TOKEN_SHL_ASSIGN: {
            if (returnType == OBJ_INTEGER)
                result = NEW_INTEGER_OBJECT((int)left_value << (int)right_value);
            break;
        }
        case TOKEN_SHR_ASSIGN: {
            if (returnType == OBJ_INTEGER)
                result = NEW_INTEGER_OBJECT((int)left_value >> (int)right_value);
            break;
        }
        }
    } else {
        ByteBuffer* bb = byte_buffer_new();
        object_to_string(bb, &identValue);
        object_to_string(bb, &value);
        char* str = byte_buffer_to_string(bb);
        byte_buffer_free(&bb);

        result = NEW_STRING_OBJECT(str);

        safe_free((void**) &str);
    }

    if (result != NULL) {
        context_assign(interpreter->env, ident, result);
        return context_get(interpreter->env, ident);
    }

    return NEW_ERROR_OBJECT(RUNTIME_ERROR, "invalid operation");
}

static Object* eval_binary_op(ObjectType returnType, Token* op, double left, double right) {
    if (op == NULL)
        return NEW_ERROR_OBJECT(RUNTIME_ERROR, "invalid operation");

    switch (op->type) {
    case TOKEN_ADD:
        if (returnType == OBJ_FLOAT)
            return NEW_FLOAT_OBJECT(left + right);
        else if (returnType == OBJ_INTEGER)
            return NEW_INTEGER_OBJECT((int)(left + right));
        break;
    case TOKEN_SUB:
        if (returnType == OBJ_FLOAT)
            return NEW_FLOAT_OBJECT(left - right);
        else if (returnType == OBJ_INTEGER)
            return NEW_INTEGER_OBJECT((int)(left - right));
        break;
    case TOKEN_MUL:
        if (returnType == OBJ_FLOAT)
            return NEW_FLOAT_OBJECT(left * right);
        else if (returnType == OBJ_INTEGER)
            return NEW_INTEGER_OBJECT((int)(left * right));
        break;
    case TOKEN_QUO:
        if (right == 0)
            return NEW_ERROR_OBJECT(DIVISION_BY_ZERO_ERROR, "division by zero");
        if (returnType == OBJ_FLOAT)
            return NEW_FLOAT_OBJECT(left / right);
        else if (returnType == OBJ_INTEGER)
            return NEW_INTEGER_OBJECT((int)(left / right));
        break;
    case TOKEN_REM:
        if (right == 0)
            return NEW_ERROR_OBJECT(DIVISION_BY_ZERO_ERROR, "division by zero");
        if (returnType == OBJ_FLOAT)
            return NEW_FLOAT_OBJECT(fmod(left, right));
        else if (returnType == OBJ_INTEGER)
            return NEW_INTEGER_OBJECT((int)fmod(left, right));
        break;
    case TOKEN_AND:
        if (returnType == OBJ_INTEGER)
            return NEW_INTEGER_OBJECT((int)left & (int)right);
        break;
    case TOKEN_OR:
        if (returnType == OBJ_INTEGER)
            return NEW_INTEGER_OBJECT((int)left | (int)right);
        break;
    case TOKEN_XOR:
        if (returnType == OBJ_INTEGER)
            return NEW_INTEGER_OBJECT((int)left ^ (int)right);
        break;
    case TOKEN_SHL:
        if (returnType == OBJ_INTEGER)
            return NEW_INTEGER_OBJECT((int)left << (int)right);
        break;
    case TOKEN_SHR:
        if (returnType == OBJ_INTEGER)
            return NEW_INTEGER_OBJECT((int)left >> (int)right);
        break;
    case TOKEN_EQL:
        return eval_to_bool_obj(left == right);
    case TOKEN_LSS:
        return eval_to_bool_obj(left < right);
    case TOKEN_GTR:
        return eval_to_bool_obj(left > right);
    case TOKEN_NEQ:
        return eval_to_bool_obj(left != right);
    case TOKEN_LEQ:
        return eval_to_bool_obj(left <= right);
    case TOKEN_GEQ:
        return eval_to_bool_obj(left >= right);
    }

    return NEW_ERROR_OBJECT(RUNTIME_ERROR, "invalid operation");
}

static Type* get_operation_type(Token* operation, Object* left, Object* right) {
    if (operation == NULL || left == NULL || right == NULL) {
        return NEW_NIL_TYPE();
    }

    switch (operation->type) {
    case TOKEN_ADD: {
        if (left->type == OBJ_CHARACTER || right->type == OBJ_CHARACTER) {
            return NEW_STRING_TYPE();
        }

        if (left->type == OBJ_STRING || right->type == OBJ_STRING) {
            return NEW_STRING_TYPE();
        }

        if (left->type == OBJ_FLOAT || right->type == OBJ_FLOAT) {
            return NEW_FLOAT_TYPE();
        }

        return NEW_INT_TYPE();
    }

    case TOKEN_SUB:
    case TOKEN_MUL:
    case TOKEN_QUO:
    case TOKEN_REM:
    case TOKEN_AND:
    case TOKEN_OR:
    case TOKEN_XOR:
    case TOKEN_SHL:
    case TOKEN_SHR: {
        if (left->type == OBJ_FLOAT || right->type == OBJ_FLOAT) {
            return NEW_FLOAT_TYPE();
        }

        return NEW_INT_TYPE();
    }

    case TOKEN_LOR:
    case TOKEN_LAND:
    case TOKEN_EQL:
    case TOKEN_LSS:
    case TOKEN_GTR:
    case TOKEN_NOT:
    case TOKEN_NEQ:
    case TOKEN_LEQ:
    case TOKEN_GEQ: {
        return NEW_BOOL_TYPE();
    }
    }

    return NEW_NIL_TYPE();
}
