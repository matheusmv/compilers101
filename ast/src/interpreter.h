#pragma once

#include "ast.h"
#include "object.h"
#include "context.h"


typedef enum InterpreterStatus {
    INTERPRETER_SUCCESS,
    INTERPRETER_FAILURE
} InterpreterStatus;

typedef struct Interpreter {
    Context* env;
    InterpreterStatus exitCode;
} Interpreter;

InterpreterStatus eval(List* declarations);

struct Object* eval_decl(struct Interpreter* interpreter, Decl* declaration);
struct Object* eval_stmt(struct Interpreter* interpreter, Stmt* statement);
struct Object* eval_expr(struct Interpreter* interpreter, Expr* expression);