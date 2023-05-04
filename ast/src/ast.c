#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "token.h"
#include "types.h"
#include "smem.h"


Decl* decl_new(DeclType type, void* decl, void (*to_string)(void**), void (*destroy)(void**)) {
    Decl* new_decl = NULL;
    new_decl = safe_malloc(sizeof(Decl), NULL);
    if (new_decl == NULL) {
        if (destroy != NULL) {
            destroy(&decl);
        }
        return NULL;
    }

    *new_decl = (Decl) {
        .type = type,
        .decl = decl,
        .to_string = to_string,
        .destroy = destroy
    };

    return new_decl;
}

void decl_to_string(Decl** decl) {
    if (decl == NULL || *decl == NULL)
        return;

    if ((*decl)->to_string != NULL)
        (*decl)->to_string(&(*decl)->decl);
}

void decl_free(Decl** decl) {
    if (decl == NULL || *decl == NULL)
        return;

    if ((*decl)->destroy != NULL)
        (*decl)->destroy(&(*decl)->decl);

    safe_free((void**) decl);
}

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

LetDecl* let_decl_new(Token* name, Type* type, Expr* expression) {
    LetDecl* decl = NULL;
    decl = safe_malloc(sizeof(LetDecl), NULL);
    if (decl == NULL) {
        token_free(&name);
        type_free(&type);
        expr_free(&expression);
        return NULL;
    }

    *decl = (LetDecl) {
        .name = name,
        .type = type,
        .expression = expression
    };

    return decl;
}

void let_decl_to_string(LetDecl** letDecl) {
    if (letDecl == NULL || *letDecl == NULL)
        return;

    printf("let ");

    token_to_string(&(*letDecl)->name);

    if ((*letDecl)->type != NULL) {
        printf(": ");
        type_to_string(&(*letDecl)->type);
    }

    printf(" = ");

    expr_to_string(&(*letDecl)->expression);
}

void let_decl_free(LetDecl** letDecl) {
    if (letDecl == NULL || *letDecl == NULL)
        return;

    token_free(&(*letDecl)->name);
    type_free(&(*letDecl)->type);
    expr_free(&(*letDecl)->expression);

    safe_free((void**) letDecl);
}

ConstDecl* const_decl_new(Token* name, Type* type, Expr* expression) {
    ConstDecl* decl = NULL;
    decl = safe_malloc(sizeof(ConstDecl), NULL);
    if (decl == NULL) {
        token_free(&name);
        type_free(&type);
        expr_free(&expression);
        return NULL;
    }

    *decl = (ConstDecl) {
        .name = name,
        .type = type,
        .expression = expression
    };

    return decl;
}

void const_decl_to_string(ConstDecl** constDecl) {
    if (constDecl == NULL || *constDecl == NULL)
        return;

    printf("const ");

    token_to_string(&(*constDecl)->name);

    if ((*constDecl)->type != NULL) {
        printf(": ");
        type_to_string(&(*constDecl)->type);
    }

    printf(" = ");

    expr_to_string(&(*constDecl)->expression);
}

void const_decl_free(ConstDecl** constDecl) {
    if (constDecl == NULL || *constDecl == NULL)
        return;

    token_free(&(*constDecl)->name);
    type_free(&(*constDecl)->type);
    expr_free(&(*constDecl)->expression);

    safe_free((void**) constDecl);
}

FunctionDecl* function_decl_new(Token* name, List* parameters, List* returnTypes, Stmt* body) {
    FunctionDecl* decl = NULL;
    decl = safe_malloc(sizeof(FunctionDecl), NULL);
    if (decl == NULL) {
        token_free(&name);
        list_free(&parameters);
        list_free(&returnTypes);
        stmt_free(&body);
        return NULL;
    }

    *decl = (FunctionDecl) {
        .name = name,
        .parameters = parameters,
        .returnTypes = returnTypes,
        .body = body
    };

    return decl;
}

void function_decl_add_parameter(FunctionDecl** functionDecl, Decl* parameter) {
    if (functionDecl == NULL || *functionDecl == NULL || parameter == NULL)
        return;

    list_insert_last(&(*functionDecl)->parameters, parameter);
}

void function_decl_add_return_type(FunctionDecl** functionDecl, Type* type) {
    if (functionDecl == NULL || *functionDecl == NULL || type == NULL)
        return;

    list_insert_last(&(*functionDecl)->returnTypes, type);
}

void function_decl_to_string(FunctionDecl** functionDecl) {
    if (functionDecl == NULL || *functionDecl == NULL)
        return;

    printf("func ");

    token_to_string(&(*functionDecl)->name);

    printf("(");

    List* params = (*functionDecl)->parameters;
    if (!list_is_empty(&params)) {
        list_foreach(param, params) {
            expr_to_string((Expr**) &param->value);

            if (param->next != NULL) {
                printf(", ");
            }
        }
    }

    printf(")");

    List* returnTypes = (*functionDecl)->returnTypes;
    if (!list_is_empty(&returnTypes)) {
        printf(": ");

        list_foreach(type, returnTypes) {
            type_to_string((Type**) &type->value);

            if (type->next != NULL) {
                printf(" | ");
            }
        }
    }

    printf(" ");

    stmt_to_string(&(*functionDecl)->body);
}

void function_decl_free(FunctionDecl** functionDecl) {
    if (functionDecl == NULL || *functionDecl == NULL)
        return;

    token_free(&(*functionDecl)->name);
    list_free(&(*functionDecl)->parameters);
    list_free(&(*functionDecl)->returnTypes);
    stmt_free(&(*functionDecl)->body);

    safe_free((void**) functionDecl);
}

FieldDecl* field_decl_new(Token* name, Type* type) {
    FieldDecl* decl = NULL;
    decl = safe_malloc(sizeof(FieldDecl), NULL);
    if (decl == NULL) {
        token_free(&name);
        type_free(&type);
        return NULL;
    }

    *decl = (FieldDecl) {
        .name = name,
        .type = type
    };

    return decl;
}

void field_decl_to_string(FieldDecl** fieldDecl) {
    if (fieldDecl == NULL || *fieldDecl == NULL)
        return;

    token_to_string(&(*fieldDecl)->name);

    printf(": ");

    type_to_string(&(*fieldDecl)->type);
}

void field_decl_free(FieldDecl** fieldDecl) {
    if (fieldDecl == NULL || *fieldDecl == NULL)
        return;

    token_free(&(*fieldDecl)->name);
    type_free(&(*fieldDecl)->type);

    safe_free((void**) fieldDecl);
}

StructDecl* struct_decl_new(Token* name, List* fields) {
    StructDecl* decl = NULL;
    decl = safe_malloc(sizeof(StructDecl), NULL);
    if (decl == NULL) {
        token_free(&name);
        list_free(&fields);
        return NULL;
    }

    *decl = (StructDecl) {
        .name = name,
        .fields = fields
    };

    return decl;
}

void struct_decl_add_field(StructDecl** structDecl, Decl* field) {
    if (structDecl == NULL || *structDecl == NULL || field == NULL)
        return;

    list_insert_last(&(*structDecl)->fields, field);
}

void struct_decl_to_string(StructDecl** structDecl) {
    if (structDecl == NULL || *structDecl == NULL)
        return;

    printf("struct ");

    if ((*structDecl)->name != NULL) {
        token_to_string(&(*structDecl)->name);
        printf(" ");
    }

    printf("{");

    List* fields = (*structDecl)->fields;
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

void struct_decl_free(StructDecl** structDecl) {
    if (structDecl == NULL || *structDecl == NULL)
        return;

    token_free(&(*structDecl)->name);
    list_free(&(*structDecl)->fields);

    safe_free((void**) structDecl);
}

StmtDecl* stmt_decl_new(Stmt* stmt) {
    StmtDecl* decl = NULL;
    decl = safe_malloc(sizeof(StmtDecl), NULL);
    if (decl == NULL) {
        stmt_free(&stmt);
        return NULL;
    }

    *decl = (StmtDecl) {
        .stmt = stmt
    };

    return decl;
}

void stmt_decl_to_string(StmtDecl** stmtDecl) {
    if (stmtDecl == NULL || *stmtDecl == NULL)
        return;

    stmt_to_string(&(*stmtDecl)->stmt);
}

void stmt_decl_free(StmtDecl** stmtDecl) {
    if (stmtDecl == NULL || *stmtDecl == NULL)
        return;

    stmt_free(&(*stmtDecl)->stmt);

    safe_free((void**) stmtDecl);
}

BlockStmt* block_stmt_new(List* declarations) {
    BlockStmt* stmt = NULL;
    stmt = safe_malloc(sizeof(BlockStmt), NULL);
    if (stmt == NULL) {
        list_free(&declarations);
        return NULL;
    }

    *stmt = (BlockStmt) {
        .declarations = declarations
    };

    return stmt;
}

void block_stmt_add_declaration(BlockStmt** blockStmt, Decl* declaration) {
    if (blockStmt == NULL || *blockStmt == NULL || declaration == NULL)
        return;

    list_insert_last(&(*blockStmt)->declarations, declaration);
}

void block_stmt_to_string(BlockStmt** blockStmt) {
    if (blockStmt == NULL || *blockStmt == NULL)
        return;

    printf("{");

    List* args = (*blockStmt)->declarations;
    if (!list_is_empty(&args)) {
        printf("\n");

        list_foreach(arg, args) {
            decl_to_string((Decl**) &arg->value);

            printf("\n");
        }
    }

    printf("}");
}

void block_stmt_free(BlockStmt** blockStmt) {
    if (blockStmt == NULL || *blockStmt == NULL)
        return;

    list_free(&(*blockStmt)->declarations);

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

ForStmt* for_stmt_new(Decl* initialization, Expr* condition, Expr* action, Stmt* body) {
    ForStmt* stmt = NULL;
    stmt = safe_malloc(sizeof(ForStmt), NULL);
    if (stmt == NULL) {
        decl_free(&initialization);
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

    decl_to_string(&(*forStmt)->initialization);

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

    decl_free(&(*forStmt)->initialization);
    expr_free(&(*forStmt)->condition);
    expr_free(&(*forStmt)->action);
    stmt_free(&(*forStmt)->body);

    safe_free((void**) forStmt);
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

StructInlineExpr* struct_inline_expr_new(Type* type, List* fields) {
    StructInlineExpr* expr = NULL;
    expr = safe_malloc(sizeof(StructInlineExpr), NULL);
    if (expr == NULL) {
        type_free(&type);
        list_free(&fields);
        return NULL;
    }

    *expr = (StructInlineExpr) {
        .type = type,
        .fields = fields
    };

    return expr;
}

void struct_inline_expr_add_field(StructInlineExpr** structInline, Expr* field) {
    if (structInline == NULL || *structInline == NULL)
        return;

    list_insert_last(&(*structInline)->fields, field);
}

void struct_inline_expr_to_string(StructInlineExpr** structInline) {
    if (structInline == NULL || *structInline == NULL)
        return;

    if ((*structInline)->type != NULL)
        type_to_string(&(*structInline)->type);

    printf("{ ");

    list_foreach(field, (*structInline)->fields) {
        expr_to_string((Expr**) &field->value);

        if (field->next != NULL) {
            printf(", ");
        }
    }

    printf(" }");
}

void struct_inline_expr_free(StructInlineExpr** structInline) {
    if (structInline == NULL || *structInline == NULL)
        return;

    type_free(&(*structInline)->type);
    list_free(&(*structInline)->fields);

    safe_free((void**) structInline);
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
