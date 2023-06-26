#pragma once

#include "ast.h"
#include "context.h"
#include "list.h"
#include "types.h"


typedef enum TypeCheckerStatus {
    TYPE_CHECKER_SUCCESS,
    TYPE_CHECKER_FAILURE
} TypeCheckerStatus;

typedef struct TypeChecker {
    Context* env;
    Type* currentFunctionReturnType;
    bool hasCurrentFunctionReturned;
    bool hasFunctionTypeToDefine;
    TypeCheckerStatus currentStatus;
} TypeChecker;

TypeCheckerStatus check(List* declarations);

TypeCheckerStatus init_and_check(TypeChecker** typeChecker, List* declarations);
void type_checker_destroy(TypeChecker** typeChecker);

Type* get_decl_type(TypeChecker* typeChecker, Decl* declaration);
Type* get_expr_type(TypeChecker* typeChecker, Expr* expression);
