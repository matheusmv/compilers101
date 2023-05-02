#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "token.h"
#include "types.h"
#include "smem.h"


Stmt* stmt_new(StmtType type, void* stmt, void (*to_string)(void**), void (*destroy)(void**)) {
    Stmt* new_stmt = NULL;
    new_stmt = safe_malloc(sizeof(Expr), NULL);
    if (new_stmt == NULL) {
        if (destroy != NULL) {
            destroy(&stmt);
        }
        return NULL;
    }

    *new_stmt = (Stmt) {
        .type = type,
        .stmt = stmt,
        .to_string = to_string,
        .destroy = destroy
    };

    return new_stmt;
}

void stmt_to_string(Stmt** stmt) {
    if (stmt == NULL || *stmt == NULL)
        return;

    if ((*stmt)->to_string != NULL)
        (*stmt)->to_string(&(*stmt)->stmt);
}

void stmt_free(Stmt** stmt) {
    if (stmt == NULL || *stmt == NULL)
        return;

    if ((*stmt)->destroy != NULL)
        (*stmt)->destroy(&(*stmt)->stmt);

    safe_free((void**) stmt);
}

Expr* expr_new(ExprType type, void* expr, void (*to_string)(void**), void (*destroy)(void**)) {
    Expr* new_expr = NULL;
    new_expr = safe_malloc(sizeof(Expr), NULL);
    if (new_expr == NULL) {
        if (destroy != NULL) {
            destroy(&expr);
        }
        return NULL;
    }

    *new_expr = (Expr) {
        .type = type,
        .expr = expr,
        .to_string = to_string,
        .destroy = destroy
    };

    return new_expr;
}

void expr_to_string(Expr** expr) {
    if (expr == NULL || *expr == NULL)
        return;

    if ((*expr)->to_string != NULL)
        (*expr)->to_string(&(*expr)->expr);
}

void expr_free(Expr** expr) {
    if (expr == NULL || *expr == NULL)
        return;

    if ((*expr)->destroy != NULL)
        (*expr)->destroy(&(*expr)->expr);

    safe_free((void**) expr);
}

BlockStmt* block_stmt_new(List* statements) {
    BlockStmt* stmt = NULL;
    stmt = safe_malloc(sizeof(BlockStmt), NULL);
    if (stmt == NULL) {
        list_free(&statements);
        return NULL;
    }

    *stmt = (BlockStmt) {
        .statements = statements
    };

    return stmt;
}

void block_stmt_add_statement(BlockStmt** blockStmt, Stmt* statement) {
    if (blockStmt == NULL || *blockStmt == NULL || statement == NULL)
        return;

    list_insert_last(&(*blockStmt)->statements, statement);
}

void block_stmt_to_string(BlockStmt** blockStmt) {
    if (blockStmt == NULL || *blockStmt == NULL)
        return;

    printf("{");

    List* args = (*blockStmt)->statements;
    if (!list_is_empty(&args)) {
        printf("\n");

        list_foreach(arg, args) {
            stmt_to_string((Stmt**) &arg->value);

            printf("\n");
        }
    }

    printf("}");
}

void block_stmt_free(BlockStmt** blockStmt) {
    if (blockStmt == NULL || *blockStmt == NULL)
        return;

    list_free(&(*blockStmt)->statements);

    safe_free((void**) blockStmt);
}

ExpressionStmt* expression_stmt_new(Expr* expression) {
    ExpressionStmt* stmt = NULL;
    stmt = safe_malloc(sizeof(ExpressionStmt), NULL);
    if (stmt == NULL) {
        expr_free(&expression);
        return NULL;
    }

    *stmt = (ExpressionStmt) {
        .expression = expression
    };

    return stmt;
}

void expression_stmt_to_string(ExpressionStmt** expressionStmt) {
    if (expressionStmt == NULL || *expressionStmt == NULL)
        return;

    expr_to_string(&(*expressionStmt)->expression);
}

void expression_stmt_free(ExpressionStmt** expressionStmt) {
    if (expressionStmt == NULL || *expressionStmt == NULL)
        return;

    expr_free(&(*expressionStmt)->expression);

    safe_free((void**) expressionStmt);
}

FunctionStmt* function_stmt_new(Token* name, List* parameters, Stmt* body) {
    FunctionStmt* stmt = NULL;
    stmt = safe_malloc(sizeof(FunctionStmt), NULL);
    if (stmt == NULL) {
        token_free(&name);
        list_free(&parameters);
        stmt_free(&body);
        return NULL;
    }

    *stmt = (FunctionStmt) {
        .name = name,
        .parameters = parameters,
        .body = body
    };

    return stmt;
}

void function_stmt_add_parameter(FunctionStmt** functionStmt, Expr* parameter) {
    if (functionStmt == NULL || *functionStmt == NULL || parameter == NULL)
        return;

    list_insert_last(&(*functionStmt)->parameters, parameter);
}

void function_stmt_to_string(FunctionStmt** functionStmt) {
    if (functionStmt == NULL || *functionStmt == NULL)
        return;

    printf("func ");

    token_to_string(&(*functionStmt)->name);

    printf("(");

    List* params = (*functionStmt)->parameters;
    if (!list_is_empty(&params)) {
        list_foreach(param, params) {
            expr_to_string((Expr**) &param->value);

            if (param->next != NULL) {
                printf(", ");
            }
        }
    }

    printf(") ");

    stmt_to_string(&(*functionStmt)->body);
}

void function_stmt_free(FunctionStmt** functionStmt) {
    if (functionStmt == NULL || *functionStmt == NULL)
        return;

    token_free(&(*functionStmt)->name);
    list_free(&(*functionStmt)->parameters);
    stmt_free(&(*functionStmt)->body);

    safe_free((void**) functionStmt);
}

ReturnStmt* return_stmt_new(Expr* expression) {
    ReturnStmt* stmt = NULL;
    stmt = safe_malloc(sizeof(ReturnStmt), NULL);
    if (stmt == NULL) {
        expr_free(&expression);
        return NULL;
    }

    *stmt = (ReturnStmt) {
        .expression = expression
    };

    return stmt;
}

void return_stmt_to_string(ReturnStmt** returnStmt) {
    if (returnStmt == NULL || *returnStmt == NULL)
        return;

    printf("return ");

    expr_to_string(&(*returnStmt)->expression);
}

void return_stmt_free(ReturnStmt** returnStmt) {
    if (returnStmt == NULL || *returnStmt == NULL)
        return;

    expr_free(&(*returnStmt)->expression);

    safe_free((void**) returnStmt);
}

LetStmt* let_stmt_new(Token* name, Expr* expression) {
    LetStmt* stmt = NULL;
    stmt = safe_malloc(sizeof(LetStmt), NULL);
    if (stmt == NULL) {
        token_free(&name);
        expr_free(&expression);
        return NULL;
    }

    *stmt = (LetStmt) {
        .name = name,
        .expression = expression
    };

    return stmt;
}

void let_stmt_to_string(LetStmt** letStmt) {
    if (letStmt == NULL || *letStmt == NULL)
        return;

    printf("let ");

    token_to_string(&(*letStmt)->name);

    printf(" = ");

    expr_to_string(&(*letStmt)->expression);
}

void let_stmt_free(LetStmt** letStmt) {
    if (letStmt == NULL || *letStmt == NULL)
        return;

    token_free(&(*letStmt)->name);
    expr_free(&(*letStmt)->expression);

    safe_free((void**) letStmt);
}

IfStmt* if_stmt_new(Expr* condition, Stmt* thenBranch, Stmt* elseBranch) {
    IfStmt* stmt = NULL;
    stmt = safe_malloc(sizeof(IfStmt), NULL);
    if (stmt == NULL) {
        expr_free(&condition);
        stmt_free(&thenBranch);
        stmt_free(&elseBranch);
        return NULL;
    }

    *stmt = (IfStmt) {
        .condition = condition,
        .thenBranch = thenBranch,
        .elseBranch = elseBranch
    };

    return stmt;
}

void if_stmt_to_string(IfStmt** ifStmt) {
    if (ifStmt == NULL || *ifStmt == NULL)
        return;

    printf("if (");

    expr_to_string(&(*ifStmt)->condition);

    printf(") ");

    stmt_to_string(&(*ifStmt)->thenBranch);

    if ((*ifStmt)->elseBranch != NULL) {
        printf(" else ");

        stmt_to_string(&(*ifStmt)->elseBranch);
    }
}

void if_stmt_free(IfStmt** ifStmt) {
    if (ifStmt == NULL || *ifStmt == NULL)
        return;

    expr_free(&(*ifStmt)->condition);
    stmt_free(&(*ifStmt)->thenBranch);
    stmt_free(&(*ifStmt)->elseBranch);

    safe_free((void**) ifStmt);
}

WhileStmt* while_stmt_new(Expr* condition, Stmt* body) {
    WhileStmt* stmt = NULL;
    stmt = safe_malloc(sizeof(WhileStmt), NULL);
    if (stmt == NULL) {
        expr_free(&condition);
        stmt_free(&body);
        return NULL;
    }

    *stmt = (WhileStmt) {
        .condition = condition,
        .body = body
    };

    return stmt;
}

void while_stmt_to_string(WhileStmt** whileStmt) {
    if (whileStmt == NULL || *whileStmt == NULL)
        return;

    printf("while (");

    expr_to_string(&(*whileStmt)->condition);

    printf(") ");

    stmt_to_string(&(*whileStmt)->body);
}

void while_stmt_free(WhileStmt** whileStmt) {
    if (whileStmt == NULL || *whileStmt == NULL)
        return;

    expr_free(&(*whileStmt)->condition);
    stmt_free(&(*whileStmt)->body);

    safe_free((void**) whileStmt);
}

ForStmt* for_stmt_new(Stmt* initialization, Expr* condition, Expr* action, Stmt* body) {
    ForStmt* stmt = NULL;
    stmt = safe_malloc(sizeof(ForStmt), NULL);
    if (stmt == NULL) {
        stmt_free(&initialization);
        expr_free(&condition);
        expr_free(&action);
        stmt_free(&body);
        return NULL;
    }

    *stmt = (ForStmt) {
        .initialization = initialization,
        .condition = condition,
        .action = action,
        .body = body
    };

    return stmt;
}

void for_stmt_to_string(ForStmt** forStmt) {
    if (forStmt == NULL || *forStmt == NULL)
        return;

    printf("for (");

    stmt_to_string(&(*forStmt)->initialization);

    printf("; ");

    expr_to_string(&(*forStmt)->condition);

    printf("; ");

    expr_to_string(&(*forStmt)->action);

    printf(") ");

    stmt_to_string(&(*forStmt)->body);
}

void for_stmt_free(ForStmt** forStmt) {
    if (forStmt == NULL || *forStmt == NULL)
        return;

    stmt_free(&(*forStmt)->initialization);
    expr_free(&(*forStmt)->condition);
    expr_free(&(*forStmt)->action);
    stmt_free(&(*forStmt)->body);

    safe_free((void**) forStmt);
}

FieldDeclStmt* field_decl_stmt_new(Token* name, Type* type) {
    FieldDeclStmt* stmt = NULL;
    stmt = safe_malloc(sizeof(FieldDeclStmt), NULL);
    if (stmt == NULL) {
        token_free(&name);
        type_free(&type);
        return NULL;
    }

    *stmt = (FieldDeclStmt) {
        .name = name,
        .type = type
    };

    return stmt;
}

void field_decl_stmt_to_string(FieldDeclStmt** fieldDecl) {
    if (fieldDecl == NULL || *fieldDecl == NULL)
        return;

    token_to_string(&(*fieldDecl)->name);

    printf(": ");

    type_to_string(&(*fieldDecl)->type);
}

void field_decl_stmt_free(FieldDeclStmt** fieldDecl) {
    if (fieldDecl == NULL || *fieldDecl == NULL)
        return;

    token_free(&(*fieldDecl)->name);
    type_free(&(*fieldDecl)->type);

    safe_free((void**) fieldDecl);
}

StructStmt* struct_stmt_new(Token* name, List* fields) {
    StructStmt* stmt = NULL;
    stmt = safe_malloc(sizeof(StructStmt), NULL);
    if (stmt == NULL) {
        token_free(&name);
        list_free(&fields);
        return NULL;
    }

    *stmt = (StructStmt) {
        .name = name,
        .fields = fields
    };

    return stmt;
}

void struct_stmt_add_field(StructStmt** structStmt, Stmt* field) {
    if (structStmt == NULL || *structStmt == NULL || field == NULL)
        return;

    list_insert_last(&(*structStmt)->fields, field);
}

void struct_stmt_to_string(StructStmt** structStmt) {
    if (structStmt == NULL || *structStmt == NULL)
        return;

    printf("struct ");

    if ((*structStmt)->name != NULL) {
        token_to_string(&(*structStmt)->name);
        printf(" ");
    }

    printf("{");

    List* fields = (*structStmt)->fields;
    if (!list_is_empty(&fields)) {
        printf("\n");

        list_foreach(field, fields) {
            printf("\t");

            stmt_to_string((Stmt**) &field->value);

            if (field->next != NULL) {
                printf("\n");
            }
        }

        printf("\n");
    }

    printf("}");
}

void struct_stmt_free(StructStmt** structStmt) {
    if (structStmt == NULL || *structStmt == NULL)
        return;

    token_free(&(*structStmt)->name);
    list_free(&(*structStmt)->fields);

    safe_free((void**) structStmt);
}

BinaryExpr* binary_expr_new(Expr* left, Token* op, Expr* right) {
    BinaryExpr* expr = NULL;
    expr = safe_malloc(sizeof(BinaryExpr), NULL);
    if (expr == NULL) {
        expr_free(&left);
        token_free(&op);
        expr_free(&right);
        return NULL;
    }

    *expr = (BinaryExpr) {
        .left = left,
        .op = op,
        .right = right
    };

    return expr;
}

void binary_expr_to_string(BinaryExpr** binaryExpr) {
    if (binaryExpr == NULL || *binaryExpr == NULL)
        return;

    expr_to_string(&(*binaryExpr)->left);

    printf(" ");

    token_to_string(&(*binaryExpr)->op);

    printf(" ");

    expr_to_string(&(*binaryExpr)->right);
}

void binary_expr_free(BinaryExpr** binaryExpr) {
    if (binaryExpr == NULL || *binaryExpr == NULL)
        return;

    expr_free(&(*binaryExpr)->left);
    token_free(&(*binaryExpr)->op);
    expr_free(&(*binaryExpr)->right);

    safe_free((void**) binaryExpr);
}

GroupExpr* group_expr_new(Expr* expression) {
    GroupExpr* expr = NULL;
    expr = safe_malloc(sizeof(GroupExpr), NULL);
    if (expr == NULL) {
        expr_free(&expression);
        return NULL;
    }

    *expr = (GroupExpr) {
        .expression = expression
    };

    return expr;
}

void group_expr_to_string(GroupExpr** groupExpr) {
    if (groupExpr == NULL || *groupExpr == NULL)
        return;

    printf("(");

    expr_to_string(&(*groupExpr)->expression);

    printf(")");
}

void group_expr_free(GroupExpr** groupExpr) {
    if (groupExpr == NULL || *groupExpr == NULL)
        return;

    expr_free(&(*groupExpr)->expression);

    safe_free((void**) groupExpr);
}

AssignExpr* assign_expr_new(Expr* identifier, Token* op, Expr* expression) {
    AssignExpr* expr = NULL;
    expr = safe_malloc(sizeof(AssignExpr), NULL);
    if (expr == NULL) {
        expr_free(&identifier);
        token_free(&op);
        expr_free(&expression);
        return NULL;
    }

    *expr = (AssignExpr) {
        .identifier = identifier,
        .op = op,
        .expression = expression
    };

    return expr;
}

void assign_expr_to_string(AssignExpr** assignExpr) {
    if (assignExpr == NULL || *assignExpr == NULL)
        return;

    expr_to_string(&(*assignExpr)->identifier);

    printf(" ");

    token_to_string(&(*assignExpr)->op);

    printf(" ");

    expr_to_string(&(*assignExpr)->expression);
}

void assign_expr_free(AssignExpr** assignExpr) {
    if (assignExpr == NULL || *assignExpr == NULL)
        return;

    expr_free(&(*assignExpr)->identifier);
    token_free(&(*assignExpr)->op);
    expr_free(&(*assignExpr)->expression);

    safe_free((void**) assignExpr);
}

CallExpr* call_expr_new(Expr* callee, List* arguments) {
    CallExpr* expr = NULL;
    expr = safe_malloc(sizeof(CallExpr), NULL);
    if (expr == NULL) {
        expr_free(&callee);
        list_free(&arguments);
        return NULL;
    }

    *expr = (CallExpr) {
        .callee = callee,
        .arguments = arguments
    };

    return expr;
}

void call_expr_add_argument(CallExpr** callExpr, Expr* argument) {
    if (callExpr == NULL || *callExpr == NULL || argument == NULL)
        return;

    list_insert_last(&(*callExpr)->arguments, argument);
}

void call_expr_to_string(CallExpr** callExpr) {
    if (callExpr == NULL || *callExpr == NULL)
        return;

    expr_to_string(&(*callExpr)->callee);

    printf("(");

    list_foreach(arg, (*callExpr)->arguments) {
        expr_to_string((Expr**) &arg->value);

        if (arg->next != NULL) {
            printf(", ");
        }
    }

    printf(")");
}

void call_expr_free(CallExpr** callExpr) {
    if (callExpr == NULL || *callExpr == NULL)
        return;

    expr_free(&(*callExpr)->callee);
    list_free(&(*callExpr)->arguments);

    safe_free((void**) callExpr);
}

LogicalExpr* logical_expr_new(Expr* left, Token* op, Expr* right) {
    LogicalExpr* expr = NULL;
    expr = safe_malloc(sizeof(BinaryExpr), NULL);
    if (expr == NULL) {
        expr_free(&left);
        token_free(&op);
        expr_free(&right);
        return NULL;
    }

    *expr = (LogicalExpr) {
        .left = left,
        .op = op,
        .right = right
    };

    return expr;
}

void logical_expr_to_string(LogicalExpr** logicalExpr) {
    if (logicalExpr == NULL || *logicalExpr == NULL)
        return;

    expr_to_string(&(*logicalExpr)->left);

    printf(" ");

    token_to_string(&(*logicalExpr)->op);

    printf(" ");

    expr_to_string(&(*logicalExpr)->right);
}

void logical_expr_free(LogicalExpr** logicalExpr) {
    if (logicalExpr == NULL || *logicalExpr == NULL)
        return;

    expr_free(&(*logicalExpr)->left);
    token_free(&(*logicalExpr)->op);
    expr_free(&(*logicalExpr)->right);

    safe_free((void**) logicalExpr);
}

UnaryExpr* unary_expr_new(Token* op, Expr* expression) {
    UnaryExpr* expr = NULL;
    expr = safe_malloc(sizeof(UnaryExpr), NULL);
    if (expr == NULL) {
        token_free(&op);
        expr_free(&expression);
        return NULL;
    }

    *expr = (UnaryExpr) {
        .op = op,
        .expression = expression
    };

    return expr;
}

void unary_expr_to_string(UnaryExpr** unaryExpr) {
    if (unaryExpr == NULL || *unaryExpr == NULL)
        return;

    token_to_string(&(*unaryExpr)->op);
    expr_to_string(&(*unaryExpr)->expression);
}

void unary_expr_free(UnaryExpr** unaryExpr) {
    if (unaryExpr == NULL || *unaryExpr == NULL)
        return;

    token_free(&(*unaryExpr)->op);
    expr_free(&(*unaryExpr)->expression);

    safe_free((void**) unaryExpr);
}

UpdateExpr* update_expr_new(Expr* expression, Token* op) {
    UpdateExpr* expr = NULL;
    expr = safe_malloc(sizeof(UpdateExpr), NULL);
    if (expr == NULL) {
        expr_free(&expression);
        token_free(&op);
        return NULL;
    }

    *expr = (UpdateExpr) {
        .expression = expression,
        .op = op
    };

    return expr;
}

void update_expr_to_string(UpdateExpr** updateExpr) {
    if (updateExpr == NULL || *updateExpr == NULL)
        return;

    expr_to_string(&(*updateExpr)->expression);
    token_to_string(&(*updateExpr)->op);
}

void update_expr_free(UpdateExpr** updateExpr) {
    if (updateExpr == NULL || *updateExpr == NULL)
        return;

    expr_free(&(*updateExpr)->expression);
    token_free(&(*updateExpr)->op);

    safe_free((void**) updateExpr);
}

FieldInitExpr* field_init_expr_new(Token* name, Expr* value) {
    FieldInitExpr* expr = NULL;
    expr = safe_malloc(sizeof(FieldInitExpr), NULL);
    if (expr == NULL) {
        token_free(&name);
        expr_free(&value);
        return NULL;
    }

    *expr = (FieldInitExpr) {
        .name = name,
        .value = value
    };

    return expr;
}

void field_init_expr_to_string(FieldInitExpr** fieldInit) {
    if (fieldInit == NULL || *fieldInit == NULL)
        return;

    token_to_string(&(*fieldInit)->name);

    printf(": ");

    expr_to_string(&(*fieldInit)->value);
}

void field_init_expr_free(FieldInitExpr** fieldInit) {
    if (fieldInit == NULL || *fieldInit == NULL)
        return;

    token_free(&(*fieldInit)->name);
    expr_free(&(*fieldInit)->value);

    safe_free((void**) fieldInit);
}

StructInitExpr* struct_init_expr_new(Token* name, List* fields) {
    StructInitExpr* expr = NULL;
    expr = safe_malloc(sizeof(StructInitExpr), NULL);
    if (expr == NULL) {
        token_free(&name);
        list_free(&fields);
        return NULL;
    }

    *expr = (StructInitExpr) {
        .name = name,
        .fields = fields
    };

    return expr;
}

void struct_init_expr_add_field(StructInitExpr** structInit, Expr* field) {
    if (structInit == NULL || *structInit == NULL || field == NULL)
        return;

    list_insert_last(&(*structInit)->fields, field);
}

void struct_init_expr_to_string(StructInitExpr** structInit) {
    if (structInit == NULL || *structInit == NULL)
        return;

    if ((*structInit)->name != NULL)
        token_to_string(&(*structInit)->name);

    printf("{ ");

    list_foreach(field, (*structInit)->fields) {
        expr_to_string((Expr**) &field->value);

        if (field->next != NULL) {
            printf(", ");
        }
    }

    printf(" }");
}

void struct_init_expr_free(StructInitExpr** structInit) {
    if (structInit == NULL || *structInit == NULL)
        return;

    token_free(&(*structInit)->name);
    list_free(&(*structInit)->fields);

    safe_free((void**) structInit);
}

LiteralExpr* literal_expr_new(LiteralType type, void* value, void (*to_string)(void**), void (*destroy)(void**)) {
    LiteralExpr* expr = NULL;
    expr = safe_malloc(sizeof(LiteralExpr), NULL);
    if (expr == NULL) {
        if (destroy != NULL) {
            destroy(&value);
        }
        return NULL;
    }

    *expr = (LiteralExpr) {
        .type = type,
        .value = value,
        .to_string = to_string,
        .destroy = destroy
    };

    return expr;
}

void literal_expr_to_string(LiteralExpr** literalExpr) {
    if (literalExpr == NULL || *literalExpr == NULL)
        return;

    (*literalExpr)->to_string(&(*literalExpr)->value);
}

void literal_expr_free(LiteralExpr** literalExpr) {
    if (literalExpr == NULL || *literalExpr == NULL)
        return;

    if ((*literalExpr)->destroy != NULL)
        (*literalExpr)->destroy(&(*literalExpr)->value);

    safe_free((void**) literalExpr);
}