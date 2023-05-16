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
    List* currentFunctionReturnType;
    bool hasCurrentFunctionReturned;
    bool hasFunctionExprTypeToDefine;
} TypeChecker;

TypeChecker* type_checker_init(void);
void type_checker_free(TypeChecker** typeChecker);

TypeCheckerStatus check(TypeChecker* typeChecker, List* declarations);
