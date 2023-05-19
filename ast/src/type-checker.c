#include "type-checker.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "context.h"
#include "list.h"
#include "literal-type.h"
#include "map.h"
#include "smem.h"
#include "token.h"
#include "types.h"


static Type* type_lookup[] = {
    [INT_TYPE]    = NULL,
    [FLOAT_TYPE]  = NULL,
    [CHAR_TYPE]   = NULL,
    [STRING_TYPE] = NULL,
    [BOOL_TYPE]   = NULL,
    [VOID_TYPE]   = NULL,
    [NIL_TYPE]    = NULL
};

static void init_type_lookup(void) {
    type_lookup[INT_TYPE] = NEW_INT_TYPE();
    type_lookup[FLOAT_TYPE] = NEW_FLOAT_TYPE();
    type_lookup[CHAR_TYPE] = NEW_CHAR_TYPE();
    type_lookup[STRING_TYPE] = NEW_STRING_TYPE();
    type_lookup[BOOL_TYPE] = NEW_BOOL_TYPE();
    type_lookup[VOID_TYPE] = NEW_VOID_TYPE();
    type_lookup[NIL_TYPE] = NEW_NIL_TYPE();
}

static void free_type_lookup(void) {
    for (size_t i = _atomic_start + 1; i < _atomic_end; i++) {
        type_free((Type**) &type_lookup[i]);
    }
}

static Type* get_type_for(TypeID typeId) {
    if (_atomic_start < typeId && typeId < _atomic_end)
        return type_lookup[typeId];

    return NULL;
}

static bool entry_cmp(const MapEntry** entry, char** key) {
    return strcmp((*entry)->key, *key) == 0;
}

static bool equals(Type* self, Type* other) {
    return type_equals(&self, &other);
}

static bool cmp_namedType_fieldName(const Type** namedType, char** fieldName) {
    NamedType* nt = (NamedType*) (*namedType)->type;
    return strcmp(nt->name, *fieldName) == 0;
}

static Type* check_decl(TypeChecker* typeChecker, Decl* declaration);
static Type* check_stmt(TypeChecker* typeChecker, Stmt* statement);
static Type* check_expr(TypeChecker* typeChecker, Expr* expression);
static Type* check_literal(TypeChecker* typeChecker, LiteralExpr* literalExpr);
static Expr* get_zero_value(Type* type);
static Type* lookup(TypeChecker* typeChecker, Expr* ident, Expr* name);

static TypeChecker* type_checker_init(void) {
    TypeChecker* type_checker = NULL;
    type_checker = safe_malloc(sizeof(TypeChecker), NULL);
    if (type_checker == NULL) {
        return NULL;
    }

    *type_checker = (TypeChecker) {
        .env = context_new(MAP_NEW(32, entry_cmp, NULL, NULL)),
        .currentFunctionReturnType = NULL,
        .hasCurrentFunctionReturned = false,
        .hasFunctionTypeToDefine = false,
        .currentStatus = TYPE_CHECKER_SUCCESS
    };

    return type_checker;
}

static void type_checker_free(TypeChecker** typeChecker) {
    if (typeChecker == NULL || *typeChecker == NULL)
        return;

    context_free(&(*typeChecker)->env);

    safe_free((void**) typeChecker);
}

TypeCheckerStatus check(List* declarations) {
    init_type_lookup();

    TypeChecker* typeChecker = type_checker_init();

    Type* result = NULL;

    list_foreach(declaration, declarations) {
        result = check_decl(typeChecker, declaration->value);
    }

    TypeCheckerStatus status = typeChecker->currentStatus;

    type_checker_free(&typeChecker);

    free_type_lookup();

    return status;
}

static Type* check_decl(TypeChecker* typeChecker, Decl* declaration) {
    switch (declaration->type) {
    case LET_DECL: {
        LetDecl* letDecl = (LetDecl*) declaration->decl;

        Type* declaredType = letDecl->type;
        Type* initializerType = NULL;

        if (declaredType == NULL && letDecl->expression == NULL) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid LetDecl:\n\t");
            decl_to_string(&declaration);
            printf("?\n");
            return NULL;
        }

        if (declaredType != NULL && letDecl->expression == NULL) {
            letDecl->expression = get_zero_value(declaredType);
            initializerType = check_expr(typeChecker, letDecl->expression);
        }

        initializerType = check_expr(typeChecker, letDecl->expression);

        if (declaredType != NULL) {
            bool ok = false;

            if (declaredType->typeId == CUSTOM_TYPE && initializerType != NULL && initializerType->typeId != CUSTOM_TYPE) {
                Type* declaredTypeDetails = context_get(typeChecker->env, ((AtomicType*) declaredType->type)->name);
                ok = equals(declaredTypeDetails, initializerType);
            } else if (declaredType->typeId == CUSTOM_TYPE && initializerType != NULL && initializerType->typeId == CUSTOM_TYPE) {
                Type* declaredTypeDetails = context_get(typeChecker->env, ((AtomicType*) declaredType->type)->name);
                Type* initializerTypeDetails = context_get(typeChecker->env, ((AtomicType*) initializerType->type)->name);
                ok = equals(declaredTypeDetails, initializerTypeDetails);
            } else {
                ok = equals(declaredType, initializerType);
            }

            if (!ok) {
                typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                fprintf(stderr, "incompatible type in LetDecl.");
                fprintf(stderr, "\n\trequired: ");
                type_to_string(&declaredType);
                printf("\n");
                fprintf(stderr, "\tgot: ");
                type_to_string(&initializerType);
                printf("\n");
                return NULL;
            }
        }

        if (typeChecker->hasFunctionTypeToDefine) {
            context_define(typeChecker->env, letDecl->name->literal, initializerType);
            check_expr(typeChecker, letDecl->expression);
        } else {
            context_define(typeChecker->env, letDecl->name->literal, initializerType);
        }

        return context_get(typeChecker->env, letDecl->name->literal);
    }
    case CONST_DECL: {
        ConstDecl* constDecl = (ConstDecl*) declaration->decl;

        Type* declaredType = constDecl->type;
        Type* initializerType = NULL;

        if (declaredType == NULL && constDecl->expression == NULL) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid ConstDecl:\n\t");
            decl_to_string(&declaration);
            printf("?\n");
            return NULL;
        }

        if (declaredType != NULL && constDecl->expression == NULL) {
            constDecl->expression = get_zero_value(declaredType);
            initializerType = check_expr(typeChecker, constDecl->expression);
        }

        initializerType = check_expr(typeChecker, constDecl->expression);

        if (declaredType == NULL && initializerType == NULL) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid ConstDecl: undefined type\n\t");
            decl_to_string(&declaration);
            printf("\n");
            return NULL;
        }

        if (declaredType != NULL) {
            bool ok = false;

            if (declaredType->typeId == CUSTOM_TYPE && initializerType != NULL && initializerType->typeId != CUSTOM_TYPE) {
                Type* declaredTypeDetails = context_get(typeChecker->env, ((AtomicType*) declaredType->type)->name);
                ok = equals(declaredTypeDetails, initializerType);
            } else if (declaredType->typeId == CUSTOM_TYPE && initializerType != NULL && initializerType->typeId == CUSTOM_TYPE) {
                Type* declaredTypeDetails = context_get(typeChecker->env, ((AtomicType*) declaredType->type)->name);
                Type* initializerTypeDetails = context_get(typeChecker->env, ((AtomicType*) initializerType->type)->name);
                ok = equals(declaredTypeDetails, initializerTypeDetails);
            } else {
                ok = equals(declaredType, initializerType);
            }

            if (!ok) {
                typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                fprintf(stderr, "incompatible type in ConstDecl.");
                fprintf(stderr, "\n\trequired: ");
                type_to_string(&declaredType);
                printf("\n");
                fprintf(stderr, "\tgot: ");
                type_to_string(&initializerType);
                printf("\n");
                return NULL;
            }
        }

        if (typeChecker->hasFunctionTypeToDefine) {
            context_define(typeChecker->env, constDecl->name->literal, initializerType);
            check_expr(typeChecker, constDecl->expression);
        } else {
            context_define(typeChecker->env, constDecl->name->literal, initializerType);
        }

        return context_get(typeChecker->env, constDecl->name->literal);
    }
    case FIELD_DECL: {
        FieldDecl* fieldDecl = (FieldDecl*) declaration->decl;
        return fieldDecl->type;
    }
    case FUNC_DECL: {
        FunctionDecl* funcDecl = (FunctionDecl*) declaration->decl;

        List* paramTypes = list_new(NULL);
        list_foreach(param, funcDecl->parameters) {
            list_insert_last(&paramTypes, check_decl(typeChecker, param->value));
        }

        List* retrnTypes = list_new(NULL);
        list_foreach(retrn, funcDecl->returnTypes) {
            Type* type = (Type*) retrn->value;
            list_insert_last(&retrnTypes, type);
        }

        Type* funcType = NEW_FUNCTION_TYPE_WITH_PARAMS_AND_RETURNS(paramTypes, retrnTypes);

        context_define(typeChecker->env, funcDecl->name->literal, funcType);

        Context* previous = typeChecker->env;
        typeChecker->env = context_enclosed_new(previous, MAP_NEW(32, entry_cmp, NULL, NULL));

        typeChecker->hasCurrentFunctionReturned = false;

        List* previousFuncType = typeChecker->currentFunctionReturnType;

        typeChecker->currentFunctionReturnType = retrnTypes;

        list_foreach(param, funcDecl->parameters) {
            Decl* decl = (Decl*) param->value;
            FieldDecl* field = (FieldDecl*) decl->decl;

            Type* paramType = check_decl(typeChecker, decl);

            context_define(typeChecker->env, field->name->literal, paramType);
        }

        check_stmt(typeChecker, funcDecl->body);

        context_free(&typeChecker->env);

        typeChecker->env = previous;

        typeChecker->currentFunctionReturnType = previousFuncType;

        return funcType;
    }
    case STRUCT_DECL: {
        StructDecl* structDecl = (StructDecl*) declaration->decl;

        List* listOfFieldTypes = list_new((void (*)(void **)) NULL);
        list_foreach(field, structDecl->fields) {
            list_insert_last(&listOfFieldTypes, field->value);
        }

        Type* structType = NEW_STRUCT_TYPE_WITH_FIELDS(0, listOfFieldTypes);

        context_define(typeChecker->env, structDecl->name->literal, structType);

        return context_get(typeChecker->env, structDecl->name->literal);
    }
    case STMT_DECL: {
        StmtDecl* stmtDecl = (StmtDecl*) declaration->decl;
        return check_stmt(typeChecker, stmtDecl->stmt);
    }
    default:
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        fprintf(stderr, "unexpected declaration type\n");
    }

    return NULL;
}

static Type* check_stmt(TypeChecker* typeChecker, Stmt* statement) {
    switch (statement->type) {
    case BLOCK_STMT: {
        Context* previous = typeChecker->env;

        typeChecker->env = context_enclosed_new(previous, MAP_NEW(32, entry_cmp, NULL, NULL));

        BlockStmt* blockStmt = (BlockStmt*) statement->stmt;

        list_foreach(declaration, blockStmt->declarations) {
            check_decl(typeChecker, declaration->value);
        }

        context_free(&typeChecker->env);

        typeChecker->env = previous;
        break;
    }
    case RETURN_STMT: {
        ReturnStmt* returnStmt = (ReturnStmt*) statement->stmt;
        if (returnStmt->expression != NULL
            && !list_is_empty(&typeChecker->currentFunctionReturnType)
            && list_size(&typeChecker->currentFunctionReturnType) == 1)
        {
            list_foreach(retrnType, typeChecker->currentFunctionReturnType) {
                if (equals(retrnType->value, get_type_for(VOID_TYPE))) {
                    typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                    fprintf(stderr, "not expecting any return value\n");
                    return NULL;
                }
            }
        }

        Type* returnType =  check_expr(typeChecker, returnStmt->expression);

        if (!list_is_empty(&typeChecker->currentFunctionReturnType)) {
            bool matchAnyReturn = false;

            list_foreach(retrnType, typeChecker->currentFunctionReturnType) {
                Type* funcReturn = (Type*) retrnType->value;
                if (funcReturn->typeId == CUSTOM_TYPE) {
                    funcReturn = context_get(typeChecker->env, ((AtomicType*) funcReturn->type)->name);
                }

                if (equals(funcReturn, returnType)) {
                    matchAnyReturn = true;
                    break;
                }
            }

            if (!matchAnyReturn) {
                typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                fprintf(stderr, "invalid FunctionReturn:");
                fprintf(stderr, "\n\trequired: ");
                list_foreach(retrnType, typeChecker->currentFunctionReturnType) {
                    type_to_string((Type**) &retrnType->value);
                    if (retrnType->next != NULL) {
                        printf(" | ");
                    }
                }
                printf("\n");
                fprintf(stderr, "\tgot: ");
                type_to_string(&returnType);
                printf("\n");
                return NULL;
            }
        }

        typeChecker->hasCurrentFunctionReturned = true;

        return returnType;
    }
    case BREAK_STMT: break;
    case CONTINUE_STMT: break;
    case IF_STMT: {
        IfStmt* ifStmt = (IfStmt*) statement->stmt;

        Type* conditionType = check_expr(typeChecker, ifStmt->condition);
        if (!equals(conditionType, get_type_for(BOOL_TYPE))) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid IfStmt: incompatible condition type");
            fprintf(stderr, "\n\trequired: bool\n\t");
            fprintf(stderr, "got: ");
            type_to_string(&conditionType);
            printf("\n\t");
            expr_to_string(&ifStmt->condition);
            printf("\n");
            return NULL;
        }

        check_stmt(typeChecker, ifStmt->thenBranch);

        if (ifStmt->elseBranch != NULL) {
            check_stmt(typeChecker, ifStmt->elseBranch);
        }

        return get_type_for(VOID_TYPE);
    }
    case WHILE_STMT: {
        WhileStmt* whileStmt = (WhileStmt*) statement->stmt;

        Type* conditionType = check_expr(typeChecker, whileStmt->condition);
        if (!equals(conditionType, get_type_for(BOOL_TYPE))) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid IfStmt: incompatible condition type");
            fprintf(stderr, "\n\trequired: bool\n\t");
            fprintf(stderr, "got: ");
            type_to_string(&conditionType);
            printf("\n\t");
            expr_to_string(&whileStmt->condition);
            printf("\n");
            return NULL;
        }

        check_stmt(typeChecker, whileStmt->body);

        return get_type_for(VOID_TYPE);
    }
    case FOR_STMT: break;
    case EXPRESSION_STMT: {
        ExpressionStmt* exprStmt = (ExpressionStmt*) statement->stmt;
        return check_expr(typeChecker, exprStmt->expression);
    }
    default:
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        fprintf(stderr, "unexpected statement type\n");
    }

    return NULL;
}

static bool expect_expr(Type* exprType, size_t n_elements, ...) {
    va_list args;

    va_start(args, n_elements);

    for (size_t i = 0; i < n_elements; i++) {
        if (equals(exprType, get_type_for(va_arg(args, TypeID)))) {
            va_end(args);
            return true;
        }
    }

    va_end(args);

    return false;
}

static Type* check_expr(TypeChecker* typeChecker, Expr* expression) {
    switch (expression->type) {
    case BINARY_EXPR: {
        BinaryExpr* binaryExpr = (BinaryExpr*) expression->expr;

        Type* leftType = check_expr(typeChecker, binaryExpr->left);
        Type* rightType = check_expr(typeChecker, binaryExpr->right);

        bool leftIsOk = false;

        switch (binaryExpr->op->type) {
        case TOKEN_ADD: {
            leftIsOk = expect_expr(leftType, 4, INT_TYPE, FLOAT_TYPE, CHAR_TYPE, STRING_TYPE);
            break;
        }
        case TOKEN_SUB:
        case TOKEN_MUL:
        case TOKEN_QUO:
        case TOKEN_REM:
        case TOKEN_LSS:
        case TOKEN_GTR:
        case TOKEN_LEQ:
        case TOKEN_GEQ:{
            leftIsOk = expect_expr(leftType, 2, INT_TYPE, FLOAT_TYPE);
            break;
        }
        case TOKEN_EQL:
        case TOKEN_NEQ: {
            leftIsOk = expect_expr(leftType, 6, INT_TYPE, FLOAT_TYPE, CHAR_TYPE, STRING_TYPE, BOOL_TYPE, NIL_TYPE);
            break;
        }
        case TOKEN_AND:
        case TOKEN_OR:
        case TOKEN_XOR:
        case TOKEN_SHL:
        case TOKEN_SHR: {
            leftIsOk = expect_expr(leftType, 1, INT_TYPE);
            if (leftIsOk && !equals(leftType, rightType)) {
                typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                fprintf(stderr, "invalid BinaryExpr: left type must be equals to right type\n\t");
                expr_to_string(&expression);
                printf("\n");
                return NULL;
            }
        }
        }

        if (!leftIsOk) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "unexpected left type in BinaryExpr\n\t");
            expr_to_string(&expression);
            printf("\n");
            return NULL;
        }

        if ((
                equals(leftType, get_type_for(INT_TYPE)) &&
                equals(rightType, get_type_for(FLOAT_TYPE))
            ) || (
                equals(leftType, get_type_for(FLOAT_TYPE)) &&
                equals(rightType, get_type_for(INT_TYPE))
            )
        ) {
            switch (binaryExpr->op->type) {
            case TOKEN_LSS:
            case TOKEN_GTR:
            case TOKEN_LEQ:
            case TOKEN_GEQ:
            case TOKEN_EQL:
            case TOKEN_NEQ:
                return get_type_for(BOOL_TYPE);
            default:
                return get_type_for(FLOAT_TYPE);
            }
        } else if (!equals(leftType, rightType)) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid BinaryExpr: left type must be equals to right type\n\t");
            expr_to_string(&expression);
            printf("\n");
            return NULL;
        }

        switch (binaryExpr->op->type) {
        case TOKEN_LSS:
        case TOKEN_GTR:
        case TOKEN_LEQ:
        case TOKEN_GEQ:
        case TOKEN_EQL:
        case TOKEN_NEQ:
            return get_type_for(BOOL_TYPE);
        default:
            return get_type_for(INT_TYPE);
        }
    }
    case GROUP_EXPR: {
        GroupExpr* groupExpr = (GroupExpr*) expression->expr;

        return check_expr(typeChecker, groupExpr->expression);
    }
    case ASSIGN_EXPR: {
        AssignExpr* assignExpr = (AssignExpr*) expression->expr;

        Type* valueType = check_expr(typeChecker, assignExpr->expression);
        Type* varType = check_expr(typeChecker, assignExpr->identifier);

        if (!equals(valueType, varType)) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid AssignExpr: incompatible types");
            fprintf(stderr, "\n\trequired: ");
            type_to_string(&varType);
            printf("\n");
            fprintf(stderr, "\tgot: ");
            type_to_string(&valueType);
            printf("\n\t");
            expr_to_string(&expression);
            printf("\n");
            return NULL;
        }

        return valueType;
    }
    case CALL_EXPR: {
        CallExpr* callExpr = (CallExpr*) expression->expr;

        Type* calleType = check_expr(typeChecker, callExpr->callee);
        if (calleType == NULL) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid CallExpr: function not defined.\n\t");
            fprintf(stderr, "---> ");
            expr_to_string(&expression);
            printf("\n");
            return NULL;
        }

        if (calleType->typeId != FUNC_TYPE) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            expr_to_string(&callExpr->callee);
            fprintf(stderr, ": not a function.\n");
            return NULL;
        }

        FunctionType* funcType = calleType->type;

        size_t nArgs = list_size(&callExpr->arguments);
        size_t nParam = list_size(&funcType->parameterTypes);

        if (nArgs > nParam) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid CallExpr: to many arguments");
            fprintf(stderr, "\n\trequired: %ld\n", nParam);
            fprintf(stderr, "\tgot: %ld\n", nArgs);
            return NULL;
        } else if (nArgs < nParam) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid CallExpr: insufficient number of arguments");
            fprintf(stderr, "\n\trequired: %ld\n", nParam);
            fprintf(stderr, "\tgot: %ld\n", nArgs);
            return NULL;
        }

        size_t index = 0;
        list_foreach(arg, callExpr->arguments) {
            Type* argType = check_expr(typeChecker, arg->value);
            Type* expectedType = list_get_at(&funcType->parameterTypes, index);

            if (!equals(argType, expectedType)) {
                typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                fprintf(stderr, "invalid CallExpr: argument has invalid type.");
                fprintf(stderr, "\n\trequired: ");
                type_to_string(&expectedType);
                printf("\n");
                fprintf(stderr, "\tgot: ");
                type_to_string(&argType);
                printf("\n");
                return NULL;
            }

            index++;
        }

        return funcType->returnTypes->head->value;
    }
    case LOGICAL_EXPR: {
        LogicalExpr* logicalExpr = (LogicalExpr*) expression->expr;

        Type* leftType = check_expr(typeChecker, logicalExpr->left);
        Type* rightType = check_expr(typeChecker, logicalExpr->right);

        if (!expect_expr(leftType, 1, BOOL_TYPE)) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid LogicalExpr: invalid left type\n\t");
            expr_to_string(&expression);
            printf("\n");
            return NULL;
        }

        if (!expect_expr(rightType, 1, BOOL_TYPE)) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid LogicalExpr: invalid right type\n\t");
            expr_to_string(&expression);
            printf("\n");
            return NULL;
        }

        return leftType;
    }
    case UNARY_EXPR: {
        UnaryExpr* unaryExpr = (UnaryExpr*) expression->expr;

        Type* rightType = check_expr(typeChecker, unaryExpr->expression);

        switch (unaryExpr->op->type) {
        case TOKEN_ADD:
        case TOKEN_SUB: {
            if (!expect_expr(rightType, 2, INT_TYPE, FLOAT_TYPE)) {
                typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                fprintf(stderr, "invalid UnaryExpr: invalid right type\n\t");
                expr_to_string(&expression);
                printf("\n");
                return NULL;
            }
            break;
        }
        case TOKEN_NOT: {
            if (!expect_expr(rightType, 1, BOOL_TYPE)) {
                typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                fprintf(stderr, "invalid UnaryExpr: invalid right type\n\t");
                expr_to_string(&expression);
                printf("\n");
                return NULL;
            }
            break;
        }
        case TOKEN_TILDE: {
            if (!expect_expr(rightType, 1, INT_TYPE)) {
                typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                fprintf(stderr, "invalid UnaryExpr: invalid right type\n\t");
                expr_to_string(&expression);
                printf("\n");
                return NULL;
            }
            break;
        }
        }

        return rightType;
    }
    case UPDATE_EXPR: {
        UpdateExpr* updateExpr = (UpdateExpr*) expression->expr;

        Type* leftType = check_expr(typeChecker, updateExpr->expression);

        if (!expect_expr(leftType, 2, INT_TYPE, FLOAT_TYPE)) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid UpdatedExpr: invalid left type\n\t");
            expr_to_string(&expression);
            printf("\n");
            return NULL;
        }

        return leftType;
    }
    case FIELD_INIT_EXPR: {
        FieldInitExpr* fieldInitExpr = (FieldInitExpr*) expression->expr;

        return check_expr(typeChecker, fieldInitExpr->value);
    }
    case STRUCT_INIT_EXPR: {
        StructInitExpr* structInitExpr = (StructInitExpr*) expression->expr;

        Type* structType = context_get(typeChecker->env, structInitExpr->name->literal);
        if (structType == NULL) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            fprintf(stderr, "invalid StructInitExpr: undefined struct.\n\t");
            expr_to_string(&expression);
            printf("\n");
            return NULL;
        }

        StructType* requiredStructType = (StructType*) structType->type;

        // TODO: get zero value from omitted fields

        list_foreach(field, structInitExpr->fields) {
            FieldInitExpr* fieldInitExpr = (FieldInitExpr*) ((Expr*) field->value)->expr;
            Type* namedType = NULL;

            list_find_first(
                &requiredStructType->fields,
                (bool (*)(const void **, void **)) cmp_namedType_fieldName,
                (void**) &fieldInitExpr->name->literal,
                (void**) &namedType
            );

            if (namedType == NULL) {
                typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                fprintf(stderr, "invalid StructInitExpr: undeclared field\n\t");
                expr_to_string((Expr**) &field->value);
                printf("\n");
                fprintf(stderr, "\tin: ");
                expr_to_string(&expression);
                printf("\n");
                return NULL;
            } else {
                Type* requiredType = ((NamedType*) namedType->type)->type;
                Type* fieldType = check_expr(typeChecker, field->value);

                if (!equals(requiredType, fieldType)) {
                    typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                    fprintf(stderr, "invalid StructInitExpr: type not match\n\t");
                    fprintf(stderr, "required: ");
                    type_to_string(&namedType);
                    printf("\n");
                    fprintf(stderr, "\tgot: ");
                    field_init_expr_to_string(&fieldInitExpr);
                    printf(" (");
                    type_to_string(&fieldType);
                    printf(")");
                    printf("\n");
                    fprintf(stderr, "\tin: ");
                    expr_to_string(&expression);
                    printf("\n");
                    return NULL;
                }
            }
        }

        return structType;
    }
    case STRUCT_INLINE_EXPR: {
        StructInlineExpr* structInlineExpr = (StructInlineExpr*) expression->expr;
        StructType* inlineStructTypeDefinition = (StructType*) ((Type*) structInlineExpr->type)->type;

        // TODO: get zero value from omitted fields

        list_foreach(initExpr, structInlineExpr->fields) {
            FieldInitExpr* fieldInitExpr = (FieldInitExpr*) ((Expr*) initExpr->value)->expr;
            Type* namedType = NULL;

            list_find_first(
                &inlineStructTypeDefinition->fields,
                (bool (*)(const void **, void **)) cmp_namedType_fieldName,
                (void**) &fieldInitExpr->name->literal,
                (void**) &namedType
            );

            if (namedType == NULL) {
                typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                fprintf(stderr, "invalid StructInlineExpr: undeclared field\n\t");
                expr_to_string((Expr**) &initExpr->value);
                printf("\n");
                fprintf(stderr, "\tin: ");
                expr_to_string(&expression);
                printf("\n");
                return NULL;
            } else {
                Type* requiredType = ((NamedType*) namedType->type)->type;
                Type* fieldInitExprType = check_expr(typeChecker, fieldInitExpr->value);

                if (!equals(requiredType, fieldInitExprType)) {
                    typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                    fprintf(stderr, "invalid StructInlineExpr: type not match\n\t");
                    fprintf(stderr, "required: ");
                    type_to_string(&namedType);
                    printf("\n");
                    fprintf(stderr, "\tgot: ");
                    field_init_expr_to_string(&fieldInitExpr);
                    printf(" (");
                    type_to_string(&fieldInitExprType);
                    printf(")");
                    printf("\n");
                    fprintf(stderr, "\tin: ");
                    expr_to_string(&expression);
                    printf("\n");
                    return NULL;
                }
            }
        }

        return structInlineExpr->type;
    }
    case ARRAY_INIT_EXPR: {
        ArrayInitExpr* arrayInitExpr = (ArrayInitExpr*) expression->expr;

        Type* firstElementType = check_expr(typeChecker, arrayInitExpr->elements->head->value);

        list_foreach(element, arrayInitExpr->elements) {
            Type* elementType = check_expr(typeChecker, element->value);
            if (!equals(firstElementType, elementType)) {
                typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                fprintf(stderr, "invalid ArrayInitExpr: elements have diferent types");
                fprintf(stderr, "\n\tgot: ");
                type_to_string(&elementType);
                printf("\n");
                fprintf(stderr, "\tin: ");
                expr_to_string(&expression);
                printf("\n");
                return NULL;
            }
        }

        ArrayType* arrayInitExprType = (ArrayType*) arrayInitExpr->type->type;

        Type* arrayType = NEW_ARRAY_TYPE(firstElementType);

        list_foreach(dimension, arrayInitExprType->dimensions) {
            ArrayDimension* dim = (ArrayDimension*) ((Type*) dimension->value)->type;

            ARRAY_TYPE_ADD_DIMENSION(arrayType, NEW_ARRAY_DIMENSION(dim->size));
        }

        return arrayType;
    }
    case FUNC_EXPR: {
        FunctionExpr* funcExpr = (FunctionExpr*) expression->expr;

        List* paramTypes = list_new(NULL);
        list_foreach(param, funcExpr->parameters) {
            list_insert_last(&paramTypes, check_decl(typeChecker, param->value));
        }

        List* retrnTypes = list_new(NULL);
        list_foreach(retrn, funcExpr->returnTypes) {
            Type* type = (Type*) retrn->value;
            list_insert_last(&retrnTypes, type);
        }

        Type* funcType = NEW_FUNCTION_TYPE_WITH_PARAMS_AND_RETURNS(paramTypes, retrnTypes);

        if (!typeChecker->hasFunctionTypeToDefine) {
            typeChecker->hasFunctionTypeToDefine = true;
            return funcType;
        }

        Context* previous = typeChecker->env;
        typeChecker->env = context_enclosed_new(previous, MAP_NEW(32, entry_cmp, NULL, NULL));

        typeChecker->hasFunctionTypeToDefine = false;
        typeChecker->hasCurrentFunctionReturned = false;

        List* previousFuncType = typeChecker->currentFunctionReturnType;

        typeChecker->currentFunctionReturnType = retrnTypes;

        list_foreach(param, funcExpr->parameters) {
            Decl* decl = (Decl*) param->value;
            FieldDecl* field = (FieldDecl*) decl->decl;

            Type* paramType = check_decl(typeChecker, decl);

            context_define(typeChecker->env, field->name->literal, paramType);
        }

        check_stmt(typeChecker, funcExpr->body);

        context_free(&typeChecker->env);

        typeChecker->env = previous;

        typeChecker->currentFunctionReturnType = previousFuncType;

        return funcType;
    }
    case CONDITIONAL_EXPR: break;
    case MEMBER_EXPR: {
        MemberExpr* memberExpr = (MemberExpr*) expression->expr;

        return lookup(typeChecker, memberExpr->object, expression);
    }
    case ARRAY_MEMBER_EXPR: {
        ArrayMemberExpr* arrayMemberExpr = (ArrayMemberExpr*) expression->expr;
        Type* memberType = NULL;

        list_foreach(level, arrayMemberExpr->levelOfAccess) {
            memberType = lookup(typeChecker, arrayMemberExpr->object, level->value);
        }

        return memberType;
    }
    case CAST_EXPR: break;
    case LITERAL_EXPR: {
        LiteralExpr* literalExpr = (LiteralExpr*) expression->expr;
        return check_literal(typeChecker, literalExpr);
    }
    default:
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        fprintf(stderr, "unexpected expression type\n");
    }

    return NULL;
}

static Type* check_literal(TypeChecker* typeChecker, LiteralExpr* literalExpr) {
    switch (literalExpr->type) {
    case IDENT_LITERAL: {
        IdentLiteral* identLiteral = (IdentLiteral*) literalExpr->value;
        return context_get(typeChecker->env, identLiteral->value);
    }
    case INT_LITERAL:
        return get_type_for(INT_TYPE);
    case FLOAT_LITERAL:
        return get_type_for(FLOAT_TYPE);
    case CHAR_LITERAL:
        return get_type_for(CHAR_TYPE);
    case STRING_LITERAL:
        return get_type_for(STRING_TYPE);
    case BOOL_LITERAL:
        return get_type_for(BOOL_TYPE);
    case VOID_LITERAL:
        return get_type_for(VOID_TYPE);
    case NIL_LITERAL:
        return get_type_for(NIL_TYPE);
    default:
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        fprintf(stderr, "unexpected literal type");
        return NULL;
    }
}

static Expr* get_zero_value(Type* type) {
    switch (type->typeId) {
    case INT_TYPE:
        return NEW_INT_LITERAL(0);
    case FLOAT_TYPE:
        return NEW_FLOAT_LITERAL(0.0);
    case CHAR_TYPE:
        return NEW_CHAR_LITERAL('\0');
    case STRING_TYPE:
        return NEW_STRING_LITERAL("");
    case BOOL_TYPE:
        return NEW_BOOL_LITERAL(false);
    default:
        return NEW_NIL_LITERAL();
    }
}

static Type* do_struct_lookup(TypeChecker* typeChecker, StructType* type, Expr* expression) {
    if (expression->type != MEMBER_EXPR) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("Invalid member access: \n\t");
        expr_to_string(&expression);
        printf("\n");
        return NULL;
    }

    StructType* structType = type;
    List* memberAccessList = ((MemberExpr*) ((Expr*) expression->expr))->members;

    Type* currentMemberType = NULL;

    list_foreach(member, memberAccessList) {
        StringLiteral* memberName = ((LiteralExpr*) ((Expr*) member->value)->expr)->value;
        Type* structFieldType = NULL;

        list_find_first(
            &structType->fields,
            (bool (*)(const void **, void **)) cmp_namedType_fieldName,
            (void**) &memberName->value,
            (void**) &structFieldType
        );

        if (structFieldType == NULL) {
            currentMemberType = NULL; // remove previous assignment
            break;
        }

        currentMemberType = ((NamedType*) structFieldType->type)->type;

        if (currentMemberType != NULL && currentMemberType->typeId == CUSTOM_TYPE) {
            currentMemberType = context_get(typeChecker->env, ((AtomicType*) currentMemberType->type)->name);
            if (currentMemberType == NULL)
                break;
        }

        // enable search in inner structs
        if (currentMemberType != NULL && currentMemberType->typeId == STRUCT_TYPE) {
            structType = currentMemberType->type;
        }
    }

    return currentMemberType;
}

static Type* lookup(TypeChecker* typeChecker, Expr* ident, Expr* name) {
    Type* identType = check_expr(typeChecker, ident);
    switch (identType->typeId) {
    case STRING_TYPE: {
        Type* indexType = check_expr(typeChecker, name);
        if (!equals(indexType, get_type_for(INT_TYPE))) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            return NULL;
        }

        return get_type_for(CHAR_TYPE);
    }
    case ARRAY_TYPE: {
        ArrayType* arrayType = (ArrayType*) identType->type;

        Type* indexType = check_expr(typeChecker, name);
        if (!equals(indexType, get_type_for(INT_TYPE))) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            return NULL;
        }

        return arrayType->type;
    }
    case STRUCT_TYPE: {
        StructType* structType = (StructType*) identType->type;

        Type* currentMemberType = do_struct_lookup(typeChecker, structType, name);

        if (currentMemberType == NULL) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            printf("Invalid member access: ");
            expr_to_string(&name);
            printf("\n\t");
            printf("in: ");
            type_to_string(&identType);
            printf("\n");
            return NULL;
        }

        return currentMemberType;
    }
    case CUSTOM_TYPE: {
        Type* isStruct = context_get(typeChecker->env, ((AtomicType*) identType->type)->name);
        if (isStruct != NULL && isStruct->typeId != STRUCT_TYPE) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            printf("unrecognized type: ");
            type_to_string(&isStruct);
            printf("\n\t");
            printf("in: ");
            expr_to_string(&name);
            printf("\n");
            return NULL;
        }

        StructType* structType = (StructType*) isStruct->type;

        Type* currentMemberType = do_struct_lookup(typeChecker, structType, name);

        if (currentMemberType == NULL) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            printf("Invalid member access: ");
            expr_to_string(&name);
            printf("\n\t");
            printf("in: ");
            type_to_string(&isStruct);
            printf("\n");
            return NULL;
        }

        return currentMemberType;
    }
    default:
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        fprintf(stderr, "can only lookup strings, arrays, structs\n");
        return NULL;
    }
}
