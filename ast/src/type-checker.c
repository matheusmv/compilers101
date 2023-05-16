#include "type-checker.h"

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


static bool entry_cmp(const MapEntry** entry, char** key) {
    return strcmp((*entry)->key, *key) == 0;
}

static bool equals(Type* self, Type* other) {
    return type_equals(&self, &other);
}

static Type* check_decl(TypeChecker* typeChecker, Decl* declaration);
static Type* check_stmt(TypeChecker* typeChecker, Stmt* statement);
static Type* check_expr(TypeChecker* typeChecker, Expr* expression);
static Type* check_literal(TypeChecker* typeChecker, LiteralExpr* literalExpr);
static Expr* get_zero_value(Type* type);
static Type* lookup(TypeChecker* typeChecker, Expr* ident, Expr* name);

TypeChecker* type_checker_init(void) {
    TypeChecker* type_checker = NULL;
    type_checker = safe_malloc(sizeof(TypeChecker), NULL);
    if (type_checker == NULL) {
        return NULL;
    }

    *type_checker = (TypeChecker) {
        .env = context_new(MAP_NEW(32, entry_cmp, NULL, type_free)),
        .currentFunctionReturnType = NULL,
        .hasCurrentFunctionReturned = false
    };

    return type_checker;
}

void type_checker_free(TypeChecker** typeChecker) {
    if (typeChecker == NULL || *typeChecker == NULL)
        return;

    context_free(&(*typeChecker)->env);

    safe_free((void**) typeChecker);
}

TypeCheckerStatus check(TypeChecker* typeChecker, List* declarations) {
    Type* result = NULL;
    list_foreach(declaration, declarations) {
        result = check_decl(typeChecker, declaration->value);
    }

    return result == NULL ? TYPE_CHECKER_FAILURE : TYPE_CHECKER_SUCCESS;
}

static Type* get_decl_type(Decl* decl) {
    return NULL;
}

static Type* get_stmt_type(Stmt* stmt) {
    return NULL;
}

static Type* get_expr_type(Expr* expr) {
    return NULL;
}

static Type* check_decl(TypeChecker* typeChecker, Decl* declaration) {
    switch (declaration->type) {
    case LET_DECL: {
        LetDecl* letDecl = (LetDecl*) declaration->decl;

        Type* declaredType = letDecl->type;
        Type* initializerType = NULL;

        if (declaredType == NULL && letDecl->expression == NULL) {
            fprintf(stderr, "invalid LetDecl: impossible do define value\n\t");
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
            bool ok = equals(declaredType, initializerType);
            if (!ok) {
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

        context_define(typeChecker->env, letDecl->name->literal, initializerType);

        return context_get(typeChecker->env, letDecl->name->literal);
    }
    case CONST_DECL: {
        ConstDecl* constDecl = (ConstDecl*) declaration->decl;

        Type* declaredType = constDecl->type;
        Type* initializerType = NULL;

        if (declaredType == NULL && constDecl->expression == NULL) {
            fprintf(stderr, "invalid ConstDecl: impossible do define value\n\t");
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
            fprintf(stderr, "invalid ConstDecl: undefined type\n\t");
            decl_to_string(&declaration);
            printf("\n");
            return NULL;
        }

        if (declaredType != NULL) {
            bool ok = equals(declaredType, initializerType);
            if (!ok) {
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

        context_define(typeChecker->env, constDecl->name->literal, initializerType);

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

        typeChecker->hasCurrentFunctionReturned = false;
        typeChecker->currentFunctionReturnType = NULL;

        return funcType;
    }
    case STRUCT_DECL: break;
    case STMT_DECL: {
        StmtDecl* stmtDecl = (StmtDecl*) declaration->decl;
        return check_stmt(typeChecker, stmtDecl->stmt);
    }
    default:
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
                if (equals(retrnType->value, NEW_VOID_TYPE())) {
                    fprintf(stderr, "not expecting any return value\n");
                    return NULL;
                }
            }
        }

        Type* returnType =  check_expr(typeChecker, returnStmt->expression);
        if (!list_is_empty(&typeChecker->currentFunctionReturnType)) {
            list_foreach(retrnType, typeChecker->currentFunctionReturnType) {
                if (!equals(retrnType->value, returnType)) {
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
        }

        typeChecker->hasCurrentFunctionReturned = true;

        return returnType;
    }
    case BREAK_STMT: break;
    case CONTINUE_STMT: break;
    case IF_STMT: {
        IfStmt* ifStmt = (IfStmt*) statement->stmt;

        Type* conditionType = check_expr(typeChecker, ifStmt->condition);
        if (!equals(conditionType, NEW_BOOL_TYPE())) {
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

        return NEW_VOID_TYPE();
    }
    case WHILE_STMT: {
        WhileStmt* whileStmt = (WhileStmt*) statement->stmt;

        Type* conditionType = check_expr(typeChecker, whileStmt->condition);
        if (!equals(conditionType, NEW_BOOL_TYPE())) {
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

        return NEW_VOID_TYPE();
    }
    case FOR_STMT: break;
    case EXPRESSION_STMT: {
        ExpressionStmt* exprStmt = (ExpressionStmt*) statement->stmt;
        return check_expr(typeChecker, exprStmt->expression);
    }
    default:
        fprintf(stderr, "unexpected statement type\n");
    }

    return NULL;
}

static bool expect_expr(Type* exprType, size_t n_elements, Type** elements) {
    for (size_t i = 0; i < n_elements; i++) {
        if (equals(exprType, elements[i])) {
            return true;
        }
    }

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
            Type* permitedTypes[] = { NEW_INT_TYPE(), NEW_FLOAT_TYPE(), NEW_STRING_TYPE(), NEW_CHAR_TYPE() };
            leftIsOk = expect_expr(leftType, 3, permitedTypes);
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
            Type* permitedTypes[] = { NEW_INT_TYPE(), NEW_FLOAT_TYPE() };
            leftIsOk = expect_expr(leftType, 2, permitedTypes);
            break;
        }
        case TOKEN_EQL:
        case TOKEN_NEQ: {
            Type* permitedTypes[] = { NEW_INT_TYPE(), NEW_FLOAT_TYPE(), NEW_STRING_TYPE(), NEW_CHAR_TYPE(), NEW_BOOL_TYPE() };
            leftIsOk = expect_expr(leftType, 5, permitedTypes);
            break;
        }
        case TOKEN_AND:
        case TOKEN_OR:
        case TOKEN_XOR:
        case TOKEN_SHL:
        case TOKEN_SHR: {
            Type* permitedTypes[] = { NEW_INT_TYPE() };
            leftIsOk = expect_expr(leftType, 1, permitedTypes);
            if (leftIsOk && !equals(leftType, rightType)) {
                fprintf(stderr, "invalid BinaryExpr: left type must be equals to right type\n\t");
                expr_to_string(&expression);
                printf("\n");
                return NULL;
            }
        }
        }

        if (!leftIsOk) {
            fprintf(stderr, "unexpected left type in BinaryExpr\n\t");
            expr_to_string(&expression);
            printf("\n");
            return NULL;
        }

        if (equals(leftType, NEW_INT_TYPE()) && equals(rightType, NEW_FLOAT_TYPE())
        || equals(leftType, NEW_FLOAT_TYPE()) && equals(rightType, NEW_INT_TYPE())) {
            switch (binaryExpr->op->type) {
            case TOKEN_LSS:
            case TOKEN_GTR:
            case TOKEN_LEQ:
            case TOKEN_GEQ:
            case TOKEN_EQL:
            case TOKEN_NEQ:
                return NEW_BOOL_TYPE();
            default:
                return NEW_FLOAT_TYPE();
            }
        } else if (!equals(leftType, rightType)) {
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
            return NEW_BOOL_TYPE();
        default:
            return NEW_INT_TYPE();
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
        if (calleType->typeId != FUNC_TYPE) {
            expr_to_string(&callExpr->callee);
            fprintf(stderr, ": not a function.\n");
            return NULL;
        }

        FunctionType* funcType = calleType->type;

        size_t index = 0;
        list_foreach(arg, callExpr->arguments) {
            Type* argType = check_expr(typeChecker, arg->value);
            Type* expectedType = list_get_at(&funcType->parameterTypes, index);

            if (!equals(argType, expectedType)) {
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

        Type* permitedTypes[] = { NEW_BOOL_TYPE() };
        if (!expect_expr(leftType, 1, permitedTypes)) {
            fprintf(stderr, "invalid LogicalExpr: invalid left type\n\t");
            expr_to_string(&expression);
            printf("\n");
            return NULL;
        }

        if (!expect_expr(rightType, 1, permitedTypes)) {
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
            Type* permitedTypes[] = { NEW_INT_TYPE(), NEW_FLOAT_TYPE() };
            if (!expect_expr(rightType, 2, permitedTypes)) {
                fprintf(stderr, "invalid UnaryExpr: invalid right type\n\t");
                expr_to_string(&expression);
                printf("\n");
                return NULL;
            }
            break;
        }
        case TOKEN_NOT: {
            Type* permitedTypes[] = { NEW_BOOL_TYPE() };
            if (!expect_expr(rightType, 1, permitedTypes)) {
                fprintf(stderr, "invalid UnaryExpr: invalid right type\n\t");
                expr_to_string(&expression);
                printf("\n");
                return NULL;
            }
            break;
        }
        case TOKEN_TILDE: {
            Type* permitedTypes[] = { NEW_INT_TYPE() };
            if (!expect_expr(rightType, 1, permitedTypes)) {
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

        Type* permitedTypes[] = { NEW_INT_TYPE(), NEW_FLOAT_TYPE() };
        if (!expect_expr(leftType, 2, permitedTypes)) {
            fprintf(stderr, "invalid UpdatedExpr: invalid left type\n\t");
            expr_to_string(&expression);
            printf("\n");
            return NULL;
        }

        return leftType;
    }
    case FIELD_INIT_EXPR: break;
    case STRUCT_INIT_EXPR: break;
    case STRUCT_INLINE_EXPR: break;
    case ARRAY_INIT_EXPR: {
        ArrayInitExpr* arrayInitExpr = (ArrayInitExpr*) expression->expr;

        Type* firstElementType = check_expr(typeChecker, arrayInitExpr->elements->head->value);

        list_foreach(element, arrayInitExpr->elements) {
            Type* elementType = check_expr(typeChecker, element->value);
            if (!equals(firstElementType, elementType)) {
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
    case FUNC_EXPR: break;
    case CONDITIONAL_EXPR: break;
    case MEMBER_EXPR: break;
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
        return NEW_INT_TYPE();
    case FLOAT_LITERAL:
        return NEW_FLOAT_TYPE();
    case CHAR_LITERAL:
        return NEW_CHAR_TYPE();
    case STRING_LITERAL:
        return NEW_STRING_TYPE();
    case BOOL_LITERAL:
        return NEW_BOOL_TYPE();
    case VOID_LITERAL:
        return NEW_VOID_TYPE();
    case NIL_LITERAL:
        return NEW_NIL_TYPE();
    default:
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

static Type* lookup(TypeChecker* typeChecker, Expr* ident, Expr* name) {
    Type* identType = check_expr(typeChecker, ident);
    switch (identType->typeId) {
    case ARRAY_TYPE: {
        ArrayType* arrayType = (ArrayType*) identType->type;

        Type* indexType = check_expr(typeChecker, name);
        if (!equals(indexType, NEW_INT_TYPE())) {
            return NULL;
        }

        return arrayType->type;
    }
    default:
        fprintf(stderr, "can only lookup arrays");
        return NULL;
    }
}
