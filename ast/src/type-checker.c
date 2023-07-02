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


static Type* copy(Type* self) {
    return type_copy((const Type**) &self);
}

static bool equals(Type* self, Type* other) {
    return type_equals(&self, &other);
}

static bool cmp_namedType_fieldName(const Type** namedType, char** fieldName) {
    NamedType* nt = (NamedType*) (*namedType)->type;
    return strcmp(nt->name, *fieldName) == 0;
}

static bool entry_cmp(const MapEntry** entry, char** key) {
    return strcmp((*entry)->key, *key) == 0;
}

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

static Type* type_lookup[] = {
    [INT_TYPE]    = NULL,
    [FLOAT_TYPE]  = NULL,
    [CHAR_TYPE]   = NULL,
    [STRING_TYPE] = NULL,
    [BOOL_TYPE]   = NULL,
    [VOID_TYPE]   = NULL,
    [NIL_TYPE]    = NULL
};

static void init_type_lookup_object(void);
static void free_type_lookup_object(void);
static Type* get_type_of(TypeID typeId);

static Type* check_decl(TypeChecker* typeChecker, Decl* declaration);
static Type* check_let_decl(TypeChecker* typeChecker, LetDecl* letDecl);
static Type* check_const_decl(TypeChecker* typeChecker, ConstDecl* constDecl);
static Type* check_function_decl(TypeChecker* typeChecker, FunctionDecl* functionDecl);
static Type* check_struct_decl(TypeChecker* typeChecker, StructDecl* structDecl);

static Type* check_stmt(TypeChecker* typeChecker, Stmt* statement);
static Type* check_block_stmt(TypeChecker* typeChecker, BlockStmt* blockStmt);
static Type* check_return_stmt(TypeChecker* typeChecker, ReturnStmt* returnStmt);
static Type* check_if_stmt(TypeChecker* typeChecker, IfStmt* ifStmt);
static Type* check_while_stmt(TypeChecker* typeChecker, WhileStmt* whileStmt);
static Type* check_for_stmt(TypeChecker* typeChecker, ForStmt* forStmt);

static Type* check_expr(TypeChecker* typeChecker, Expr* expression);
static Type* check_binary_expr(TypeChecker* typeChecker, BinaryExpr* binaryExpr);
static Type* check_assign_expr(TypeChecker* typeChecker, AssignExpr* assignExpr);
static Type* check_call_expr(TypeChecker* typeChecker, CallExpr* callExpr);
static Type* check_logical_expr(TypeChecker* typeChecker, LogicalExpr* logicalExpr);
static Type* check_unary_expr(TypeChecker* typeChecker, UnaryExpr* unaryExpr);
static Type* check_update_expr(TypeChecker* typeChecker, UpdateExpr* updateExpr);
static Type* check_struct_init_expr(TypeChecker* typeChecker, StructInitExpr* structInitExpr);
static Type* check_struct_inline_expr(TypeChecker* typeChecker, StructInlineExpr* structInlineExpr);
static Type* check_array_init_expr(TypeChecker* typeChecker, ArrayInitExpr* arrayInitExpr);
static Type* check_function_expr(TypeChecker* typeChecker, FunctionExpr* functionExpr);
static Type* check_conditional_expr(TypeChecker* typeChecker, ConditionalExpr* conditionalExpr);
static Type* check_array_member_expr(TypeChecker* typeChecker, ArrayMemberExpr* arrayMemberExpr);
static Type* check_literal_expr(TypeChecker* typeChecker, LiteralExpr* literalExpr);

static Expr* get_zero_value(const Type* type);

static Type* lookup_object(TypeChecker* typeChecker, Expr* object, Expr* objectAccess);

static bool expect_expr_type(Type* exprType, size_t n_elements, ...);
static bool expect_type_id(TypeID type, size_t n_elements, ...);
static bool expect_token_type(TokenType type, size_t n_elements, ...);

static bool struct_type_has_fields_with_valid_types(StructType* structType);
static bool function_has_valid_parameters(TypeChecker* typeChecker, List* parameters);
static bool struct_has_valid_fields(TypeChecker* typeChecker, List* fields);

static bool is_argument_valid_for_parameter(TypeChecker* typeChecker,
    Expr* argument, Type* parameterType);
static bool call_expr_args_match_function_parameters(TypeChecker* typeChecker,
    List* arguments, FunctionType* functionType);

TypeCheckerStatus check(List* declarations) {
    if (declarations == NULL)
        return TYPE_CHECKER_SUCCESS;

    init_type_lookup_object();

    TypeChecker* typeChecker = type_checker_init();

    list_foreach(declaration, declarations) {
        check_decl(typeChecker, declaration->value);
    }

    TypeCheckerStatus status = typeChecker->currentStatus;

    type_checker_free(&typeChecker);

    free_type_lookup_object();

    return status;
}

TypeCheckerStatus init_and_check(TypeChecker** typeChecker, List* declarations) {
    if (declarations == NULL)
        return TYPE_CHECKER_SUCCESS;

    init_type_lookup_object();

    *typeChecker = type_checker_init();

    list_foreach(declaration, declarations) {
        check_decl(*typeChecker, declaration->value);
    }

    TypeCheckerStatus status = (*typeChecker)->currentStatus;

    return status;
}

void type_checker_destroy(TypeChecker** typeChecker) {
    if (typeChecker == NULL || *typeChecker == NULL)
        return;

    type_checker_free(typeChecker);
    free_type_lookup_object();
}

Type* get_decl_type(TypeChecker* typeChecker, Decl* declaration) {
    if (declaration == NULL)
        return NULL;

    // init_type_lookup_object();

    Type* type = check_decl(typeChecker, declaration);
    type = type_copy((const Type**) &type);

    // type_checker_free(&typeChecker);

    // free_type_lookup_object();

    return type;
}

Type* get_expr_type(TypeChecker* typeChecker, Expr* expression) {
    if (expression == NULL)
        return NULL;

    // init_type_lookup_object();

    Type* type = check_expr(typeChecker, expression);
    type = type_copy((const Type**) &type);

    // type_checker_free(&typeChecker);

    // free_type_lookup_object();

    return type;
}

static Type* check_decl(TypeChecker* typeChecker, Decl* declaration) {
    if (typeChecker == NULL || declaration == NULL)
        return NULL;

    switch (declaration->type) {
    case LET_DECL: {
        LetDecl* letDecl = (LetDecl*) declaration->decl;

        return check_let_decl(typeChecker, letDecl);
    }
    case CONST_DECL: {
        ConstDecl* constDecl = (ConstDecl*) declaration->decl;

        return check_const_decl(typeChecker, constDecl);
    }
    case FIELD_DECL: {
        FieldDecl* fieldDecl = (FieldDecl*) declaration->decl;

        return fieldDecl->type;
    }
    case FUNC_DECL: {
        FunctionDecl* functionDecl = (FunctionDecl*) declaration->decl;

        return check_function_decl(typeChecker, functionDecl);
    }
    case STRUCT_DECL: {
        StructDecl* structDecl = (StructDecl*) declaration->decl;

        return check_struct_decl(typeChecker, structDecl);
    }
    case STMT_DECL: {
        StmtDecl* stmtDecl = (StmtDecl*) declaration->decl;

        return check_stmt(typeChecker, stmtDecl->stmt);
    }
    default:
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;

        printf("\nUnexpected declaration type\n");

        return NULL;
    }
}

static Type* check_stmt(TypeChecker* typeChecker, Stmt* statement) {
    if (typeChecker == NULL || statement == NULL)
        return NULL;

    switch (statement->type) {
    case BLOCK_STMT: {
        BlockStmt* blockStmt = (BlockStmt*) statement->stmt;

        return check_block_stmt(typeChecker, blockStmt);
    }
    case RETURN_STMT: {
        ReturnStmt* returnStmt = (ReturnStmt*) statement->stmt;

        return check_return_stmt(typeChecker, returnStmt);
    }
    case BREAK_STMT: {
        return NULL;
    }
    case CONTINUE_STMT: {
        return NULL;
    }
    case IF_STMT: {
        IfStmt* ifStmt = (IfStmt*) statement->stmt;

        return check_if_stmt(typeChecker, ifStmt);
    }
    case WHILE_STMT: {
        WhileStmt* whileStmt = (WhileStmt*) statement->stmt;

        return check_while_stmt(typeChecker, whileStmt);
    }
    case FOR_STMT: {
        ForStmt* forStmt = (ForStmt*) statement->stmt;

        return check_for_stmt(typeChecker, forStmt);
    }
    case EXPRESSION_STMT: {
        ExpressionStmt* exprStmt = (ExpressionStmt*) statement->stmt;

        return check_expr(typeChecker, exprStmt->expression);
    }
    default:
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;

        printf("\nUnexpected statement type\n");

        return NULL;
    }
}

static Type* check_expr(TypeChecker* typeChecker, Expr* expression) {
    if (typeChecker == NULL || expression == NULL)
        return NULL;

    switch (expression->type) {
    case BINARY_EXPR: {
        BinaryExpr* binaryExpr = (BinaryExpr*) expression->expr;

        return check_binary_expr(typeChecker, binaryExpr);
    }
    case GROUP_EXPR: {
        GroupExpr* groupExpr = (GroupExpr*) expression->expr;

        return check_expr(typeChecker, groupExpr->expression);
    }
    case ASSIGN_EXPR: {
        AssignExpr* assignExpr = (AssignExpr*) expression->expr;

        return check_assign_expr(typeChecker, assignExpr);
    }
    case CALL_EXPR: {
        CallExpr* callExpr = (CallExpr*) expression->expr;

        return check_call_expr(typeChecker, callExpr);
    }
    case LOGICAL_EXPR: {
        LogicalExpr* logicalExpr = (LogicalExpr*) expression->expr;

        return check_logical_expr(typeChecker, logicalExpr);
    }
    case UNARY_EXPR: {
        UnaryExpr* unaryExpr = (UnaryExpr*) expression->expr;

        return check_unary_expr(typeChecker, unaryExpr);
    }
    case UPDATE_EXPR: {
        UpdateExpr* updateExpr = (UpdateExpr*) expression->expr;

        return check_update_expr(typeChecker, updateExpr);
    }
    case FIELD_INIT_EXPR: {
        FieldInitExpr* fieldInitExpr = (FieldInitExpr*) expression->expr;

        return check_expr(typeChecker, fieldInitExpr->value);
    }
    case STRUCT_INIT_EXPR: {
        StructInitExpr* structInitExpr = (StructInitExpr*) expression->expr;

        return check_struct_init_expr(typeChecker, structInitExpr);
    }
    case STRUCT_INLINE_EXPR: {
        StructInlineExpr* structInlineExpr = (StructInlineExpr*) expression->expr;

        return check_struct_inline_expr(typeChecker, structInlineExpr);
    }
    case ARRAY_INIT_EXPR: {
        ArrayInitExpr* arrayInitExpr = (ArrayInitExpr*) expression->expr;

        return check_array_init_expr(typeChecker, arrayInitExpr);
    }
    case FUNC_EXPR: {
        FunctionExpr* functionExpr = (FunctionExpr*) expression->expr;

        return check_function_expr(typeChecker, functionExpr);
    }
    case CONDITIONAL_EXPR: {
        ConditionalExpr* conditionalExpr = (ConditionalExpr*) expression->expr;

        return check_conditional_expr(typeChecker, conditionalExpr);
    }
    case MEMBER_EXPR: {
        MemberExpr* memberExpr = (MemberExpr*) expression->expr;

        Type* objectType = check_expr(typeChecker, memberExpr->object);

        if (objectType != NULL && !expect_type_id(objectType->typeId, 2, CUSTOM_TYPE, STRUCT_TYPE)) {
            printf("\nInvalid MemberExpr: cannot access this object\n\t");
            expr_to_string(&memberExpr->object);
            printf(" (");
            type_to_string(&objectType);
            printf(")");
            printf("\n\tIn: ");
            member_expr_to_string(&memberExpr);
            printf("\n");
            return NULL;
        }

        return objectType != NULL
            ? lookup_object(typeChecker, memberExpr->object, expression)
            : NULL;
    }
    case ARRAY_MEMBER_EXPR: {
        ArrayMemberExpr* arrayMemberExpr = (ArrayMemberExpr*) expression->expr;

        return check_array_member_expr(typeChecker, arrayMemberExpr);
    }
    case CAST_EXPR: {
        // trash impl
        CastExpr* cast = expression->expr;

        return cast->type;
    }
    case LITERAL_EXPR: {
        LiteralExpr* literalExpr = (LiteralExpr*) expression->expr;

        return check_literal_expr(typeChecker, literalExpr);
    }
    default:
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;

        printf("\nUnexpected expression type\n");

        return NULL;
    }
}

/* Decl */

static Type* check_let_decl(TypeChecker* typeChecker, LetDecl* letDecl) {
    if (typeChecker == NULL || letDecl == NULL)
        return NULL;

    if (letDecl->type == NULL && letDecl->expression == NULL) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid LetDecl:\n\t");
        let_decl_to_string(&letDecl);
        printf("?\n");
        return NULL;
    }

    if (letDecl->expression == NULL) {
        letDecl->expression = get_zero_value(letDecl->type);
    }

    Type* initializerType = check_expr(typeChecker, letDecl->expression);

    if (letDecl->type == NULL) {
        if (equals(initializerType, get_type_of(NIL_TYPE))) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            printf("\nInvalid LetDecl: assigning nil to an untyped declaration is not allowed\n\t");
            let_decl_to_string(&letDecl);
            printf("\n");
            return NULL;
        }

        letDecl->type = copy(initializerType);
    }

    Type* declaredType = letDecl->type;

    bool invalidTypeInDecl = equals(declaredType, get_type_of(NIL_TYPE)) ||
        equals(declaredType, get_type_of(VOID_TYPE));

    if (invalidTypeInDecl) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid LetDecl: declaration with type not allowed\n\t");
        let_decl_to_string(&letDecl);
        printf("\n");
        return NULL;
    }

    if (declaredType == NULL || initializerType == NULL) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nCould not do type checking on:\n\t");
        let_decl_to_string(&letDecl);
        printf("\n");
        return NULL;
    }

    bool declaredTypeIsCustomType = declaredType->typeId == CUSTOM_TYPE;
    bool initializerTypeIsCustomType = initializerType->typeId == CUSTOM_TYPE;
    bool isAssigningNilToCompositeType = equals(initializerType, get_type_of(NIL_TYPE)) &&
            expect_type_id(declaredType->typeId, 4, CUSTOM_TYPE, STRUCT_TYPE, ARRAY_TYPE, FUNC_TYPE);

    bool typeMatch = false;

    if (!isAssigningNilToCompositeType && declaredTypeIsCustomType && !initializerTypeIsCustomType) {
        typeMatch = equals(
            context_get(typeChecker->env, ((AtomicType*) declaredType->type)->name),
            initializerType
        );
    } else if (!isAssigningNilToCompositeType && !declaredTypeIsCustomType && initializerTypeIsCustomType) {
        typeMatch = equals(
            declaredType,
            context_get(typeChecker->env, ((AtomicType*) initializerType->type)->name)
        );
    } else {
        typeMatch = isAssigningNilToCompositeType || equals(declaredType, initializerType);
    }

    if (!typeMatch) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf( "\nIncompatible type in LetDecl.");
        printf("\n\tRequired: ");
        type_to_string(&declaredType);
        printf("\n\tGot: ");
        type_to_string(&initializerType);
        printf("\n\tIn: ");
        let_decl_to_string(&letDecl);
        printf("\n");
        return NULL;
    }

    context_define(typeChecker->env, letDecl->name->literal, declaredType);

    if (typeChecker->hasFunctionTypeToDefine) {
        check_expr(typeChecker, letDecl->expression);
    }

    return context_get(typeChecker->env, letDecl->name->literal);
}

static Type* check_const_decl(TypeChecker* typeChecker, ConstDecl* constDecl) {
    if (typeChecker == NULL || constDecl == NULL)
        return NULL;

    if (constDecl->type == NULL && constDecl->expression == NULL) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid ConstDecl:\n\t");
        const_decl_to_string(&constDecl);
        printf("?\n");
        return NULL;
    }

    if (constDecl->expression == NULL) {
        constDecl->expression = get_zero_value(constDecl->type);
    }

    Type* initializerType = check_expr(typeChecker, constDecl->expression);

    if (constDecl->type == NULL) {
        if (equals(initializerType, get_type_of(NIL_TYPE))) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            printf("\nInvalid ConstDecl: assigning nil to an untyped declaration is not allowed\n\t");
            const_decl_to_string(&constDecl);
            printf("\n");
            return NULL;
        }

        constDecl->type = copy(initializerType);
    }

    Type* declaredType = constDecl->type;

    bool invalidTypeInDecl = equals(declaredType, get_type_of(NIL_TYPE)) ||
        equals(declaredType, get_type_of(VOID_TYPE));

    if (invalidTypeInDecl) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid ConstDecl: declaration with type not allowed\n\t");
        const_decl_to_string(&constDecl);
        printf("\n");
        return NULL;
    }

    if (declaredType == NULL || initializerType == NULL) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nCould not do type checking on:\n\t");
        const_decl_to_string(&constDecl);
        printf("\n");
        return NULL;
    }

    bool declaredTypeIsCustomType = declaredType->typeId == CUSTOM_TYPE;
    bool initializerTypeIsCustomType = initializerType->typeId == CUSTOM_TYPE;
    bool isAssigningNilToCompositeType = equals(initializerType, get_type_of(NIL_TYPE)) &&
            expect_type_id(declaredType->typeId, 4, CUSTOM_TYPE, STRUCT_TYPE, ARRAY_TYPE, FUNC_TYPE);

    bool typeMatch = false;

    if (!isAssigningNilToCompositeType && declaredTypeIsCustomType && !initializerTypeIsCustomType) {
        typeMatch = equals(
            context_get(typeChecker->env, ((AtomicType*) declaredType->type)->name),
            initializerType
        );
    } else if (!isAssigningNilToCompositeType && !declaredTypeIsCustomType && initializerTypeIsCustomType) {
        typeMatch = equals(
            declaredType,
            context_get(typeChecker->env, ((AtomicType*) initializerType->type)->name)
        );
    } else {
        typeMatch = isAssigningNilToCompositeType || equals(declaredType, initializerType);
    }

    if (!typeMatch) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nIncompatible type in ConstDecl.");
        printf("\n\trequired: ");
        type_to_string(&declaredType);
        printf("\n\tGot: ");
        type_to_string(&initializerType);
        printf("\n\tIn: ");
        const_decl_to_string(&constDecl);
        printf("\n");
        return NULL;
    }

    context_define(typeChecker->env, constDecl->name->literal, declaredType);

    if (typeChecker->hasFunctionTypeToDefine) {
        check_expr(typeChecker, constDecl->expression);
    }

    return context_get(typeChecker->env, constDecl->name->literal);
}

static Type* check_function_decl(TypeChecker* typeChecker, FunctionDecl* functionDecl) {
    if (typeChecker == NULL || functionDecl == NULL)
        return NULL;

    if (!function_has_valid_parameters(typeChecker, functionDecl->parameters)) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid FunctionDecl: the function has parameters with invalid types --> (void) or (nil)\n");
        function_decl_to_string(&functionDecl);
        printf("\n");
        return NULL;
    }

    List* paramTypes = list_new(NULL);
    list_foreach(param, functionDecl->parameters) {
        list_insert_last(&paramTypes, check_decl(typeChecker, param->value));
    }

    Type* returnType = functionDecl->returnType;

    Type* functionType = NEW_FUNCTION_TYPE_WITH_PARAMS_AND_RETURN(paramTypes, returnType);

    context_define(typeChecker->env, functionDecl->name->literal, functionType);

    Context* previous = typeChecker->env;
    typeChecker->env = context_enclosed_new(previous, MAP_NEW(32, entry_cmp, NULL, NULL));

    typeChecker->hasCurrentFunctionReturned = false;

    Type* previousFunctionType = typeChecker->currentFunctionReturnType;

    typeChecker->currentFunctionReturnType = returnType;

    list_foreach(param, functionDecl->parameters) {
        Decl* decl = (Decl*) param->value;
        FieldDecl* field = (FieldDecl*) decl->decl;

        Type* paramType = check_decl(typeChecker, decl);

        context_define(typeChecker->env, field->name->literal, paramType);
    }

    check_stmt(typeChecker, functionDecl->body);

    context_free(&typeChecker->env);

    typeChecker->env = previous;

    typeChecker->currentFunctionReturnType = previousFunctionType;

    if (returnType != NULL &&
        !equals(returnType, get_type_of(VOID_TYPE)) &&
        !typeChecker->hasCurrentFunctionReturned
    ) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("Invalid FunctionDecl: a return value is specified in the function but none is provided\n");
        function_decl_to_string(&functionDecl);
        printf("\n");
        return NULL;
    }

    return functionType;
}

static Type* check_struct_decl(TypeChecker* typeChecker, StructDecl* structDecl) {
    if (typeChecker == NULL || structDecl == NULL)
        return NULL;

    if (!struct_has_valid_fields(typeChecker, structDecl->fields)) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid StructDecl: the struct has fields with invalid types --> (void) or (nil)\n");
        struct_decl_to_string(&structDecl);
        printf("\n");
        return NULL;
    }

    List* listOfFieldTypes = list_new((void (*)(void **)) NULL);
    list_foreach(field, structDecl->fields) {
        list_insert_last(&listOfFieldTypes, field->value);
    }

    Type* structType = NEW_STRUCT_TYPE_WITH_FIELDS(0, listOfFieldTypes);

    context_define(typeChecker->env, structDecl->name->literal, structType);

    return context_get(typeChecker->env, structDecl->name->literal);
}

/* Stmt */

static Type* check_block_stmt(TypeChecker* typeChecker, BlockStmt* blockStmt) {
    if (typeChecker == NULL || blockStmt == NULL)
        return NULL;

    Context* previous = typeChecker->env;

    typeChecker->env = context_enclosed_new(
        previous,
        MAP_NEW(32, entry_cmp, NULL, NULL)
    );

    list_foreach(declaration, blockStmt->declarations) {
        check_decl(typeChecker, declaration->value);
    }

    context_free(&typeChecker->env);

    typeChecker->env = previous;

    return get_type_of(VOID_TYPE);
}

static Type* check_return_stmt(TypeChecker* typeChecker, ReturnStmt* returnStmt) {
    if (typeChecker == NULL || returnStmt == NULL)
        return NULL;

    bool returningAnExpressionInAFunctionThatHasNotReturnValue =
        returnStmt->expression != NULL && typeChecker->currentFunctionReturnType == NULL;

    if (returningAnExpressionInAFunctionThatHasNotReturnValue) {
        Type* returnType =  check_expr(typeChecker, returnStmt->expression);
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nTo many return values");
        printf("\n\tHave: ");
        return_stmt_to_string(&returnStmt);
        printf(" (");
        type_to_string(&returnType);
        printf(")");
        printf("\n\tWant: ()\n");
        return NULL;
    }

    bool returningAnExpressionInAFunctionThatHasVoidReturn =
        returnStmt->expression != NULL &&
        typeChecker->currentFunctionReturnType != NULL &&
        equals(typeChecker->currentFunctionReturnType, get_type_of(VOID_TYPE));

    if (returningAnExpressionInAFunctionThatHasVoidReturn) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nNot expecting any return value");
        printf("\n\tRequire: ");
        type_to_string(&typeChecker->currentFunctionReturnType);
        printf("\n\tGot: ");
        return_stmt_to_string(&returnStmt);
        printf("\n");
        return NULL;
    }

    bool returnValudIsSpecifiedInTheFunctionButNoneIsProvided =
        returnStmt->expression == NULL &&
        typeChecker->currentFunctionReturnType != NULL &&
        !equals(typeChecker->currentFunctionReturnType, get_type_of(VOID_TYPE));

    if (returnValudIsSpecifiedInTheFunctionButNoneIsProvided) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nExpecting return value");
        printf("\n\tRequire: ");
        type_to_string(&typeChecker->currentFunctionReturnType);
        printf("\n\tGot: ");
        return_stmt_to_string(&returnStmt);
        printf("\n");
        return NULL;
    }

    Type* returnType =  check_expr(typeChecker, returnStmt->expression);
    Type* functionReturn = typeChecker->currentFunctionReturnType;

    while (functionReturn != NULL && functionReturn->typeId == CUSTOM_TYPE) {
        functionReturn = context_get(typeChecker->env, ((AtomicType*) functionReturn->type)->name);
    }

    bool returnIsNilAndFunctionReturnIsCompositeType =
        returnType != NULL && functionReturn != NULL &&
        equals(returnType, get_type_of(NIL_TYPE)) &&
        expect_type_id(functionReturn->typeId, 3, STRUCT_TYPE, ARRAY_TYPE, FUNC_TYPE);

    bool emptyReturnInAFunctionThatHasVoidReturn = returnStmt->expression == NULL &&
        (functionReturn == NULL || equals(functionReturn, get_type_of(VOID_TYPE)));

    if (!returnIsNilAndFunctionReturnIsCompositeType &&
        !emptyReturnInAFunctionThatHasVoidReturn &&
        !equals(returnType, functionReturn)
    ) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid FunctionReturn:");
        printf("\n\tRequired: ");
        type_to_string(&functionReturn);
        printf("\n\tGot: ");
        type_to_string(&returnType);
        printf("\n");
        return NULL;
    }

    typeChecker->hasCurrentFunctionReturned = true;

    return returnType;
}

static Type* check_if_stmt(TypeChecker* typeChecker, IfStmt* ifStmt) {
    if (typeChecker == NULL || ifStmt == NULL)
        return NULL;

    Type* conditionType = check_expr(typeChecker, ifStmt->condition);
    if (!equals(conditionType, get_type_of(BOOL_TYPE))) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid IfStmt: incompatible condition type");
        printf("\n\tRequired: bool");
        printf("\n\tGot: ");
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

    return NULL;
}

static Type* check_while_stmt(TypeChecker* typeChecker, WhileStmt* whileStmt) {
    if (typeChecker == NULL || whileStmt == NULL)
        return NULL;

    Type* conditionType = check_expr(typeChecker, whileStmt->condition);
    if (!equals(conditionType, get_type_of(BOOL_TYPE))) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid WhileStmt: incompatible condition type");
        printf("\n\trequired: bool");
        printf("\n\tGot: ");
        type_to_string(&conditionType);
        printf("\n\t");
        expr_to_string(&whileStmt->condition);
        printf("\n");
        return NULL;
    }

    check_stmt(typeChecker, whileStmt->body);

    return NULL;
}

static Type* check_for_stmt(TypeChecker* typeChecker, ForStmt* forStmt) {
    if (typeChecker == NULL || forStmt == NULL)
        return NULL;

    Context* previous = typeChecker->env;

    typeChecker->env = context_enclosed_new(
        previous,
        MAP_NEW(32, entry_cmp, NULL, NULL)
    );

    check_decl(typeChecker, forStmt->initialization);

    Type* conditionType = check_expr(typeChecker, forStmt->condition);
    if (conditionType != NULL && !equals(conditionType, get_type_of(BOOL_TYPE))) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("Invalid ForStmt: condition expression should have type bool\n\t");
        for_stmt_to_string(&forStmt);
        printf("\n");

        context_free(&typeChecker->env);
        typeChecker->env = previous;

        return NULL;
    }

    check_expr(typeChecker, forStmt->action);

    check_stmt(typeChecker, forStmt->body);

    context_free(&typeChecker->env);
    typeChecker->env = previous;

    return NULL;
}

/* Expr */

static Type* check_binary_expr(TypeChecker* typeChecker, BinaryExpr* binaryExpr) {
    if (typeChecker == NULL || binaryExpr == NULL)
        return NULL;

    Type* leftType = check_expr(typeChecker, binaryExpr->left);
    Type* rightType = check_expr(typeChecker, binaryExpr->right);

    if (leftType == NULL || rightType == NULL) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid BinaryExpr: ");
        binary_expr_to_string(&binaryExpr);
        printf("\n");
        return NULL;
    }

    bool leftIsOk = false;

    TokenType operation = binaryExpr->op->type;

    if (operation == TOKEN_ADD) {
        leftIsOk = expect_expr_type(leftType, 4, INT_TYPE, FLOAT_TYPE, CHAR_TYPE, STRING_TYPE);
    }

    if (expect_token_type(operation, 8, TOKEN_SUB, TOKEN_MUL, TOKEN_QUO, TOKEN_REM, TOKEN_LSS, TOKEN_GTR, TOKEN_LEQ, TOKEN_GEQ)) {
        leftIsOk = expect_expr_type(leftType, 2, INT_TYPE, FLOAT_TYPE);
    }

    if (expect_token_type(operation, 2, TOKEN_EQL, TOKEN_NEQ)) {
        leftIsOk = (leftType != NULL && expect_type_id(leftType->typeId, 4, CUSTOM_TYPE, STRUCT_TYPE, ARRAY_TYPE, FUNC_TYPE))
            || expect_expr_type(leftType, 6, INT_TYPE, FLOAT_TYPE, CHAR_TYPE, STRING_TYPE, BOOL_TYPE, NIL_TYPE);
    }

    if (expect_token_type(operation, 5, TOKEN_AND, TOKEN_OR, TOKEN_XOR, TOKEN_SHL, TOKEN_SHR)) {
        leftIsOk = expect_expr_type(leftType, 1, INT_TYPE);

        if (leftIsOk && !equals(leftType, rightType)) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            printf("\nInvalid BinaryExpr: left type must be equals to right type\n\t");
            binary_expr_to_string(&binaryExpr);
            printf("\n");
            return NULL;
        }
    }

    if (!leftIsOk) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nUnexpected left type in BinaryExpr:\n\t");
        type_to_string(&leftType);
        printf(" ");
        token_to_string(&binaryExpr->op);
        printf(" ");
        type_to_string(&rightType);
        printf("\n\t");
        binary_expr_to_string(&binaryExpr);
        printf("\n");
        return NULL;
    }

    bool leftIntAndRightFloat =
        equals(leftType, get_type_of(INT_TYPE)) &&
        equals(rightType, get_type_of(FLOAT_TYPE));

    bool leftFloatAndRightInt =
        equals(leftType, get_type_of(FLOAT_TYPE)) &&
        equals(rightType, get_type_of(INT_TYPE));

    bool leftFloatAndRightFloat =
        equals(leftType, get_type_of(FLOAT_TYPE)) &&
        equals(rightType, get_type_of(FLOAT_TYPE));

    if (leftIntAndRightFloat || leftFloatAndRightInt || leftFloatAndRightFloat) {
        return expect_token_type(operation, 6, TOKEN_LSS, TOKEN_GTR, TOKEN_LEQ, TOKEN_GEQ, TOKEN_EQL, TOKEN_NEQ)
            ? get_type_of(BOOL_TYPE)
            : get_type_of(FLOAT_TYPE);
    }

    bool validNullCheck =
        (
            expect_type_id(leftType->typeId, 5, STRING_TYPE, CUSTOM_TYPE, STRUCT_TYPE, ARRAY_TYPE, FUNC_TYPE) &&
            equals(rightType, get_type_of(NIL_TYPE))
        ) || (
            expect_type_id(rightType->typeId, 5, STRING_TYPE, CUSTOM_TYPE, STRUCT_TYPE, ARRAY_TYPE, FUNC_TYPE) &&
            equals(leftType, get_type_of(NIL_TYPE))
        );

    if (validNullCheck) {
        return get_type_of(BOOL_TYPE);
    }

    bool isConcat = expect_type_id(leftType->typeId, 2, CHAR_TYPE, STRING_TYPE) &&
        expect_type_id(rightType->typeId, 2, CHAR_TYPE, STRING_TYPE);

    if (!isConcat && !equals(leftType, rightType)) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid BinaryExpr: left type must be equals to right type\n\t");
        type_to_string(&leftType);
        printf(" ");
        token_to_string(&binaryExpr->op);
        printf(" ");
        type_to_string(&rightType);
        printf("\n\t");
        binary_expr_to_string(&binaryExpr);
        printf("\n");
        return NULL;
    }

    if (isConcat && operation == TOKEN_ADD) {
        return get_type_of(STRING_TYPE);
    }

    return expect_token_type(operation, 6, TOKEN_LSS, TOKEN_GTR, TOKEN_LEQ, TOKEN_GEQ, TOKEN_EQL, TOKEN_NEQ)
        ? get_type_of(BOOL_TYPE)
        : get_type_of(INT_TYPE);
}

static Type* check_assign_expr(TypeChecker* typeChecker, AssignExpr* assignExpr) {
    if (typeChecker == NULL || assignExpr == NULL)
        return NULL;

    Type* varType = check_expr(typeChecker, assignExpr->identifier);
    Type* valueType = check_expr(typeChecker, assignExpr->expression);

    if (assignExpr->expression != NULL &&
        assignExpr->expression->type == CALL_EXPR &&
        equals(valueType, get_type_of(VOID_TYPE))
    ) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid AssignExpr: call expression has no return");
        printf("\n\tRequired: ");
        type_to_string(&varType);
        printf("\n\tGot: ");
        type_to_string(&valueType);
        printf("\n\t");
        assign_expr_to_string(&assignExpr);
        printf("\n");
        return NULL;
    }

    if (!equals(varType, valueType)) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid AssignExpr: incompatible types");
        printf("\n\tRequired: ");
        type_to_string(&varType);
        printf("\n\tGot: ");
        type_to_string(&valueType);
        printf("\n\t");
        assign_expr_to_string(&assignExpr);
        printf(" (");
        type_to_string(&valueType);
        printf(")");
        printf("\n");
        return NULL;
    }

    return valueType;
}

static Type* check_call_expr(TypeChecker* typeChecker, CallExpr* callExpr) {
    if (typeChecker == NULL || callExpr == NULL)
        return NULL;

    Expr* callee = callExpr->callee;

    if (callee != NULL && callee->type != FUNC_EXPR && callee->expr != NULL) {
        IdentLiteral* calleeIdent = ((Expr*) callee->expr)->expr;
        char* calleName = calleeIdent->value;

        if (strcmp(calleName, "print") == 0) {
            return get_type_of(VOID_TYPE);
        }

        if (strcmp(calleName, "println") == 0) {
            return get_type_of(VOID_TYPE);
        }

        if (strcmp(calleName, "input") == 0) {
            return get_type_of(STRING_TYPE);
        }

        if (strcmp(calleName, "len") == 0) {
            return get_type_of(INT_TYPE);
        }
    }

    Type* calleeType = check_expr(typeChecker, callExpr->callee);
    if (calleeType == NULL) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid CallExpr: function not defined.");
        printf("\n\t---> ");
        call_expr_to_string(&callExpr);
        printf("\n");
        return NULL;
    }

    if (calleeType->typeId != FUNC_TYPE) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        expr_to_string(&callExpr->callee);
        printf(": not a function.\n");
        return NULL;
    }

    FunctionType* functionType = calleeType->type;

    size_t nArgs = list_size(&callExpr->arguments);
    size_t nParam = list_size(&functionType->parameterTypes);

    // TODO: implement rest "args: ...string" operator

    if (nArgs != nParam) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;

        printf("\nInvalid CallExpr: number of arguments does not match ---> ");

        function_type_to_string(&functionType);

        printf("\n\tRequired: %ld (", nParam);

        list_foreach(parameterType, functionType->parameterTypes) {
            type_to_string((Type**) &parameterType->value);

            if (parameterType->next != NULL) {
                printf(", ");
            }
        }

        printf(")\n\tGot: %ld (", nArgs);

        list_foreach(argument, callExpr->arguments) {
            Type* argumentType = check_expr(typeChecker, argument->value);

            type_to_string(&argumentType);

            if (argument->next != NULL) {
                printf(", ");
            }
        }

        printf(")\n\tIn: ");

        call_expr_to_string(&callExpr);

        printf("\n");

        return NULL;
    }

    if (!call_expr_args_match_function_parameters(typeChecker, callExpr->arguments, functionType)) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;

        printf("\nInvalid CallExpr: ");

        call_expr_to_string(&callExpr);

        printf("\n\tRequired: %ld (", nParam);

        list_foreach(parameterType, functionType->parameterTypes) {
            type_to_string((Type**) &parameterType->value);

            if (parameterType->next != NULL) {
                printf(", ");
            }
        }

        printf(")\n\tGot: %ld (", nArgs);

        list_foreach(argument, callExpr->arguments) {
            Type* argumentType = check_expr(typeChecker, argument->value);

            type_to_string(&argumentType);
            
            if (argument->next != NULL) {
                printf(", ");
            }
        }

        printf(")\n");

        return NULL;
    }

    return !functionType->returnType
        ? get_type_of(VOID_TYPE)
        : functionType->returnType;
}

static Type* check_logical_expr(TypeChecker* typeChecker, LogicalExpr* logicalExpr) {
    if (typeChecker == NULL || logicalExpr == NULL)
        return NULL;

    Type* leftType = check_expr(typeChecker, logicalExpr->left);
    Type* rightType = check_expr(typeChecker, logicalExpr->right);

    if (!equals(leftType, get_type_of(BOOL_TYPE))) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid LogicalExpr: invalid left type\n\t");
        logical_expr_to_string(&logicalExpr);
        printf("\n");
        return NULL;
    }

    if (!equals(rightType, get_type_of(BOOL_TYPE))) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid LogicalExpr: invalid right type\n\t");
        logical_expr_to_string(&logicalExpr);
        printf("\n");
        return NULL;
    }

    return leftType;
}

static Type* check_unary_expr(TypeChecker* typeChecker, UnaryExpr* unaryExpr) {
    if (typeChecker == NULL || unaryExpr == NULL)
        return NULL;

    Type* rightType = check_expr(typeChecker, unaryExpr->expression);

    TokenType operation = unaryExpr->op->type;

    bool ok = true;

    if (expect_token_type(operation, 2, TOKEN_AND, TOKEN_SUB)) {
        ok = expect_expr_type(rightType, 2, INT_TYPE, FLOAT_TYPE);
    }

    if (operation == TOKEN_NOT) {
        ok = equals(rightType, get_type_of(BOOL_TYPE));
    }

    if (operation == TOKEN_TILDE) {
        ok = equals(rightType, get_type_of(INT_TYPE));
    }

    if (!ok) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid UnaryExpr: invalid right type\n\t");
        unary_expr_to_string(&unaryExpr);
        printf(" (");
        type_to_string(&rightType);
        printf(")\n");
        return NULL;
    }

    return rightType;
}

static Type* check_update_expr(TypeChecker* typeChecker, UpdateExpr* updateExpr) {
    if (typeChecker == NULL || updateExpr == NULL)
        return NULL;

    Type* leftType = check_expr(typeChecker, updateExpr->expression);

    if (!expect_expr_type(leftType, 2, INT_TYPE, FLOAT_TYPE)) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid UpdatedExpr: invalid left type");
        printf("\n\t(");
        type_to_string(&leftType);
        printf(") ");
        update_expr_to_string(&updateExpr);
        printf("\n");
        return NULL;
    }

    return leftType;
}

static Type* check_struct_init_expr(TypeChecker* typeChecker, StructInitExpr* structInitExpr) {
    if (typeChecker == NULL || structInitExpr == NULL)
        return NULL;

    Type* structType = context_get(typeChecker->env, structInitExpr->name->literal);
    if (structType == NULL) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid StructInitExpr: undefined struct.\n\t");
        struct_init_expr_to_string(&structInitExpr);
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
            printf("\nInvalid StructInitExpr: undeclared field\n\t");
            expr_to_string((Expr**) &field->value);
            printf("\n\tIn: ");
            struct_init_expr_to_string(&structInitExpr);
            printf("\n");
            return NULL;
        }

        Type* requiredType = ((NamedType*) namedType->type)->type;
        Type* fieldType = check_expr(typeChecker, field->value);

        if (!equals(requiredType, fieldType)) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            printf("\nInvalid StructInitExpr: type not match");
            printf("\n\tRequired: ");
            type_to_string(&namedType);
            printf("\n\tGot: ");
            field_init_expr_to_string(&fieldInitExpr);
            printf(" (");
            type_to_string(&fieldType);
            printf(")");
            printf("\n\tIn: ");
            struct_init_expr_to_string(&structInitExpr);
            printf("\n");
            return NULL;
        }
    }

    return structType;
}

static Type* check_struct_inline_expr(TypeChecker* typeChecker, StructInlineExpr* structInlineExpr) {
    if (typeChecker == NULL || structInlineExpr == NULL)
        return NULL;

    StructType* inlineStructTypeDefinition = (StructType*) (structInlineExpr->type)->type;

    if (!struct_has_valid_fields(typeChecker, inlineStructTypeDefinition->fields)) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid StructInlineExpr: the struct has fields with invalid types --> (void) or (nil)\n");
        struct_type_to_string(&inlineStructTypeDefinition);
        printf("\n");
        return NULL;
    }

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
            printf("\nInvalid StructInlineExpr: undeclared field\n\t");
            expr_to_string((Expr**) &initExpr->value);
            printf("\n\tIn: ");
            struct_inline_expr_to_string(&structInlineExpr);
            printf("\n");
            return NULL;
        }

        Type* requiredType = ((NamedType*) namedType->type)->type;
        Type* fieldInitExprType = check_expr(typeChecker, fieldInitExpr->value);

        if (!equals(requiredType, fieldInitExprType)) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            printf("\nInvalid StructInlineExpr: type not match");
            printf("\n\tRequired: ");
            type_to_string(&namedType);
            printf("\n\tGot: ");
            field_init_expr_to_string(&fieldInitExpr);
            printf(" (");
            type_to_string(&fieldInitExprType);
            printf(")");
            printf("\n\tIn: ");
            struct_inline_expr_to_string(&structInlineExpr);
            printf("\n");
            return NULL;
        }
    }

    return structInlineExpr->type;
}

static bool array_init_expr_has_elements(ArrayInitExpr* arrayInitExpr) {
    return arrayInitExpr != NULL && list_size(&arrayInitExpr->elements) > 0;
}

static Type* check_array_init_expr(TypeChecker* typeChecker, ArrayInitExpr* arrayInitExpr) {
    if (typeChecker == NULL || arrayInitExpr == NULL)
        return NULL;

    if (array_init_expr_has_elements(arrayInitExpr)) {
        Type* firstElementType = check_expr(typeChecker, arrayInitExpr->elements->head->value);

        list_foreach(element, arrayInitExpr->elements) {
            Type* elementType = check_expr(typeChecker, element->value);
            if (!equals(firstElementType, elementType)) {
                typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
                printf("\nInvalid ArrayInitExpr: elements have diferent types");
                printf("\n\tGot: ");
                type_to_string(&elementType);
                printf("\n\tIn: ");
                array_init_expr_to_string(&arrayInitExpr);
                printf("\n");
                return NULL;
            }
        }
    }

    ArrayType* arrayInitExprType = (ArrayType*) arrayInitExpr->type->type;

    Type* arrayType = NEW_ARRAY_TYPE(arrayInitExprType->type);

    list_foreach(dimension, arrayInitExprType->dimensions) {
        const ArrayDimension* dim = (ArrayDimension*) ((Type*) dimension->value)->type;

        ARRAY_TYPE_ADD_DIMENSION(arrayType, NEW_ARRAY_DIMENSION(dim->size));
    }

    return arrayType;
}

static Type* check_function_expr(TypeChecker* typeChecker, FunctionExpr* functionExpr) {
    if (typeChecker == NULL || functionExpr == NULL)
        return NULL;

    if (!function_has_valid_parameters(typeChecker, functionExpr->parameters)) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid FunctionExpr: the function has parameters with invalid types --> (void) or (nil)\n");
        function_expr_to_string(&functionExpr);
        printf("\n");
        return NULL;
    }

    List* paramTypes = list_new(NULL);
    list_foreach(param, functionExpr->parameters) {
        list_insert_last(&paramTypes, check_decl(typeChecker, param->value));
    }

    Type* returnType = functionExpr->returnType;

    Type* functionType = NEW_FUNCTION_TYPE_WITH_PARAMS_AND_RETURN(paramTypes, returnType);

    // TODO: (bug fix)
    // FunctionExpr is returned by another and it is not possible to continue type checking

    if (!typeChecker->hasFunctionTypeToDefine) {
        typeChecker->hasFunctionTypeToDefine = true;
        return functionType;
    }

    Context* previous = typeChecker->env;
    typeChecker->env = context_enclosed_new(previous, MAP_NEW(32, entry_cmp, NULL, NULL));

    typeChecker->hasFunctionTypeToDefine = false;
    typeChecker->hasCurrentFunctionReturned = false;

    Type* previousFuncType = typeChecker->currentFunctionReturnType;

    typeChecker->currentFunctionReturnType = returnType;

    list_foreach(param, functionExpr->parameters) {
        Decl* decl = (Decl*) param->value;
        FieldDecl* field = (FieldDecl*) decl->decl;

        Type* paramType = check_decl(typeChecker, decl);

        context_define(typeChecker->env, field->name->literal, paramType);
    }

    check_stmt(typeChecker, functionExpr->body);

    context_free(&typeChecker->env);

    typeChecker->env = previous;

    typeChecker->currentFunctionReturnType = previousFuncType;

    if (returnType != NULL &&
        !equals(returnType, get_type_of(VOID_TYPE)) &&
        !typeChecker->hasCurrentFunctionReturned
    ) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("Invalid FunctionExpr: a return value is specified in the function but none is provided\n");
        function_expr_to_string(&functionExpr);
        printf("\n");
        return NULL;
    }

    return functionType;
}

static Type* check_conditional_expr(TypeChecker* typeChecker, ConditionalExpr* conditionalExpr) {
    if (typeChecker == NULL || conditionalExpr == NULL)
        return NULL;

    Type* conditionType = check_expr(typeChecker, conditionalExpr->condition);

    if (!equals(conditionType, get_type_of(BOOL_TYPE))) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid ConditionalExpr: condition should have type bool\n\t");
        conditional_expr_to_string(&conditionalExpr);
        printf("\n");
        return NULL;
    }

    Type* isTrueType = check_expr(typeChecker, conditionalExpr->isTrue);
    Type* isFalseType = check_expr(typeChecker, conditionalExpr->isFalse);

    if (!equals(isTrueType, isFalseType)) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid ConditionalExpr: type mismatch in conditional expression\n\t");
        conditional_expr_to_string(&conditionalExpr);
        printf(" (");
        type_to_string(&isTrueType);
        printf(" : ");
        type_to_string(&isFalseType);
        printf(")\n");
        return NULL;
    }

    return isTrueType;
}

static Type* check_array_member_expr(TypeChecker* typeChecker, ArrayMemberExpr* arrayMemberExpr) {
    if (typeChecker == NULL || arrayMemberExpr == NULL)
        return NULL;

    Type* objectType = check_expr(typeChecker, arrayMemberExpr->object);
    if (objectType == NULL || objectType->typeId != ARRAY_TYPE) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid ArrayMemberExpr: invalid member access\n\t");
        array_member_expr_to_string(&arrayMemberExpr);
        printf("\n");
        return NULL;
    }

    ArrayType* arrayType = objectType->type;

    if (list_size(&arrayMemberExpr->levelOfAccess) > list_size(&arrayType->dimensions)) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid ArrayMemberExpr: invalid member access\n\t");
        array_member_expr_to_string(&arrayMemberExpr);
        printf("\n");
        return NULL;
    }

    if (list_size(&arrayMemberExpr->levelOfAccess) == list_size(&arrayType->dimensions)) {
        return type_copy((const Type**) &arrayType->type);
    }

    Type* memberType = NEW_ARRAY_TYPE(type_copy((const Type**) &arrayType->type));

    size_t current_dimension = 0;
    Type* arrayDimension = list_get_at(&arrayType->dimensions, current_dimension);

    list_foreach(level, arrayMemberExpr->levelOfAccess) {
        size_t dimension_size = ((ArrayDimension*) arrayDimension->type)->size;

        ARRAY_TYPE_ADD_DIMENSION(memberType, NEW_ARRAY_DIMENSION(dimension_size));

        if (level->next != NULL) {
            current_dimension += 1;
            arrayDimension = list_get_at(&arrayType->dimensions, current_dimension);
        }
    }

    return memberType;
}

static Type* check_literal_expr(TypeChecker* typeChecker, LiteralExpr* literalExpr) {
    if (typeChecker == NULL || literalExpr == NULL)
        return NULL;

    switch (literalExpr->type) {
    case IDENT_LITERAL: {
        IdentLiteral* identLiteral = literalExpr->value;
        return context_get(typeChecker->env, identLiteral->value);
    }
    case INT_LITERAL:
        return get_type_of(INT_TYPE);
    case FLOAT_LITERAL:
        return get_type_of(FLOAT_TYPE);
    case CHAR_LITERAL:
        return get_type_of(CHAR_TYPE);
    case STRING_LITERAL:
        return get_type_of(STRING_TYPE);
    case BOOL_LITERAL:
        return get_type_of(BOOL_TYPE);
    case VOID_LITERAL:
        return get_type_of(VOID_TYPE);
    case NIL_LITERAL:
        return get_type_of(NIL_TYPE);
    default:
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;

        printf("\nUnexpected literal type\n");

        return NULL;
    }
}

static Expr* get_zero_value(const Type* type) {
    if (type == NULL)
        return NULL;

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
    if (typeChecker == NULL || type == NULL || expression == NULL)
        return NULL;

    if (expression->type != MEMBER_EXPR) {
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
        printf("\nInvalid member access: \n\t");
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
            printf("\nStruct field does not exist: (%s)\n\t", memberName->value);
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

static Type* lookup_object(TypeChecker* typeChecker, Expr* object, Expr* objectAccess) {
    if (typeChecker == NULL || object == NULL || objectAccess == NULL)
        return NULL;

    Type* identType = check_expr(typeChecker, object);

    if (identType == NULL)
        return NULL;

    switch (identType->typeId) {
    case STRING_TYPE: {
        Type* indexType = check_expr(typeChecker, objectAccess);
        if (!equals(indexType, get_type_of(INT_TYPE))) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            return NULL;
        }

        return get_type_of(CHAR_TYPE);
    }
    case ARRAY_TYPE: {
        ArrayType* arrayType = (ArrayType*) identType->type;

        Type* indexType = check_expr(typeChecker, objectAccess);
        if (!equals(indexType, get_type_of(INT_TYPE))) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            return NULL;
        }

        return arrayType->type;
    }
    case STRUCT_TYPE: {
        StructType* structType = (StructType*) identType->type;

        Type* currentMemberType = do_struct_lookup(typeChecker, structType, objectAccess);

        if (currentMemberType == NULL) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            printf("\nInvalid member access: ");
            expr_to_string(&objectAccess);
            printf("\n\tIn: ");
            type_to_string(&identType);
            return NULL;
        }

        return currentMemberType;
    }
    case CUSTOM_TYPE: {
        char* name = ((AtomicType*) identType->type)->name;
        Type* isStruct = context_get(typeChecker->env, name);
        if (isStruct == NULL || isStruct->typeId != STRUCT_TYPE) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            printf("\nUndefined: %s\n", name);
            return NULL;
        }

        StructType* structType = (StructType*) isStruct->type;

        Type* currentMemberType = do_struct_lookup(typeChecker, structType, objectAccess);

        if (currentMemberType == NULL) {
            typeChecker->currentStatus = TYPE_CHECKER_FAILURE;
            printf("\nInvalid member access: ");
            expr_to_string(&objectAccess);
            printf("\n\tIn: ");
            type_to_string(&isStruct);
            printf("\n");
            return NULL;
        }

        return currentMemberType;
    }
    default:
        typeChecker->currentStatus = TYPE_CHECKER_FAILURE;

        printf("\nCan only lookup strings, arrays, structs\n\t");
        expr_to_string(&object);
        printf(" (");
        type_to_string(&identType);
        printf(")\n\tInvalid access ---> ");
        expr_to_string(&objectAccess);
        printf("\n");

        return NULL;
    }
}

static void init_type_lookup_object(void) {
    type_lookup[INT_TYPE] = NEW_INT_TYPE();
    type_lookup[FLOAT_TYPE] = NEW_FLOAT_TYPE();
    type_lookup[CHAR_TYPE] = NEW_CHAR_TYPE();
    type_lookup[STRING_TYPE] = NEW_STRING_TYPE();
    type_lookup[BOOL_TYPE] = NEW_BOOL_TYPE();
    type_lookup[VOID_TYPE] = NEW_VOID_TYPE();
    type_lookup[NIL_TYPE] = NEW_NIL_TYPE();
}

static void free_type_lookup_object(void) {
    for (size_t i = _atomic_start + 1; i < _atomic_end; i++) {
        type_free(&type_lookup[i]);
    }
}

static Type* get_type_of(TypeID typeId) {
    if (_atomic_start < typeId && typeId < _atomic_end)
        return type_lookup[typeId];

    return NULL;
}

static bool expect_expr_type(Type* exprType, size_t n_elements, ...) {
    va_list args;

    va_start(args, n_elements);

    for (size_t i = 0; i < n_elements; i++) {
        if (equals(exprType, get_type_of(va_arg(args, TypeID)))) {
            va_end(args);
            return true;
        }
    }

    va_end(args);

    return false;
}

static bool expect_type_id(TypeID type, size_t n_elements, ...) {
    va_list args;

    va_start(args, n_elements);

    for (size_t i = 0; i < n_elements; i++) {
        if (type == va_arg(args, TypeID)) {
            va_end(args);
            return true;
        }
    }

    va_end(args);

    return false;
}

static bool expect_token_type(TokenType type, size_t n_elements, ...) {
    va_list args;

    va_start(args, n_elements);

    for (size_t i = 0; i < n_elements; i++) {
        if (type == va_arg(args, TokenType)) {
            va_end(args);
            return true;
        }
    }

    va_end(args);

    return false;
}

static bool struct_type_has_fields_with_valid_types(StructType* structType) {
    if (structType == NULL)
        return false;

    list_foreach(field, structType->fields) {
        NamedType* fieldType = ((Type*) field->value)->type;

        bool fieldIsVoidOrNil = equals(fieldType->type, get_type_of(NIL_TYPE)) ||
            equals(fieldType->type, get_type_of(VOID_TYPE));

        bool fieldIsStructTypeAndHasInvalidFields = fieldType->type->typeId == STRUCT_TYPE &&
            !struct_type_has_fields_with_valid_types(fieldType->type->type);

        if (fieldIsVoidOrNil || fieldIsStructTypeAndHasInvalidFields) {
            return false;
        }
    }

    return true;
}

static bool function_has_valid_parameters(TypeChecker* typeChecker, List* parameters) {
    if (typeChecker == NULL || parameters == NULL)
        return false;

    list_foreach(parameter, parameters) {
        Type* parameterType = check_decl(typeChecker, parameter->value);

        bool parameterTypeIsVoidOrNil = equals(parameterType, get_type_of(VOID_TYPE)) || 
            equals(parameterType, get_type_of(NIL_TYPE));

        bool parameterTypeIsStructTypeAndHasInvalidFields = parameterType->typeId == STRUCT_TYPE &&
            !struct_type_has_fields_with_valid_types(parameterType->type);

        if (parameterTypeIsVoidOrNil || parameterTypeIsStructTypeAndHasInvalidFields) {
            return false;
        }
    }

    return true;
}

static bool struct_has_valid_fields(TypeChecker* typeChecker, List* fields) {
    if (typeChecker == NULL || fields == NULL)
        return false;

    list_foreach(field, fields) {
        NamedType* fieldType = ((Type*) field->value)->type;

        bool fieldIsVoidOrNil = equals(fieldType->type, get_type_of(NIL_TYPE)) ||
            equals(fieldType->type, get_type_of(VOID_TYPE));

        bool fieldIsStructTypeAndHasInvalidFields = fieldType->type->typeId == STRUCT_TYPE &&
            !struct_type_has_fields_with_valid_types(fieldType->type->type);

        if (fieldIsVoidOrNil || fieldIsStructTypeAndHasInvalidFields) {
            return false;
        }
    }

    return true;
}

static bool is_argument_valid_for_parameter(TypeChecker* typeChecker,
    Expr* argument, Type* parameterType
) {
    if (typeChecker == NULL || argument == NULL || parameterType == NULL)
        return false;

    Type* argumentType = check_expr(typeChecker, argument);

    if (argumentType == NULL)
        return false;

    if (equals(argumentType, get_type_of(NIL_TYPE)) &&
        expect_type_id(parameterType->typeId, 4, CUSTOM_TYPE, STRUCT_TYPE, ARRAY_TYPE, FUNC_TYPE)
    ) {
        return true;
    }

    if (argumentType->typeId == CUSTOM_TYPE) {
        argumentType = context_get(typeChecker->env, ((AtomicType*)argumentType->type)->name);
    }

    if (parameterType->typeId == CUSTOM_TYPE) {
        parameterType = context_get(typeChecker->env, ((AtomicType*)parameterType->type)->name);
    }

    return equals(argumentType, parameterType);
}

static bool call_expr_args_match_function_parameters(TypeChecker* typeChecker,
    List* arguments, FunctionType* functionType
) {
    if (typeChecker == NULL || arguments == NULL || functionType == NULL)
        return false;

    bool success = true;

    ListNode* argNode = arguments->head;
    ListNode* paramNode = functionType->parameterTypes->head;

    while (argNode != NULL && paramNode != NULL) {
        Expr* argument = argNode->value;
        Type* parameterType = paramNode->value;

        if (!is_argument_valid_for_parameter(typeChecker, argument, parameterType)) {
            printf("Type mismatch:\n\t");
            expr_to_string(&argument);
            printf(" != ");
            type_to_string(&parameterType);
            printf("\n");
            success = false;
        }

        argNode = argNode->next;
        paramNode = paramNode->next;
    }

    return success;
}
