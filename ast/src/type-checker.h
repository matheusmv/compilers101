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
