#include "ast.h"
#include "list.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>

BinaryExpr* binary_expr_new(Expr* left, Token* operator, Expr* right) {
    BinaryExpr* expr = NULL;
    expr = malloc(sizeof(BinaryExpr));
    if (expr == NULL) {
        return NULL;
    }

    *expr = (BinaryExpr) {
        .left = left,
        .op = operator,
        .right = right
    };

    return expr;
}

void binary_expr_to_string(BinaryExpr** binaryExpr) {
    if (binaryExpr == NULL || *binaryExpr == NULL)
        return;

    Expr* left = (*binaryExpr)->left;
    if (left != NULL && left->to_string != NULL)
        left->to_string(&left->expr);

    Token* operator = (*binaryExpr)->op;
    if (operator != NULL && operator->literal != NULL)
        printf(" %s ", operator->literal);

    Expr* right = (*binaryExpr)->right;
    if (right != NULL && right->to_string != NULL) {
        right->to_string(&right->expr);
    }
}

void binary_expr_free(BinaryExpr** binaryExpr) {
    if (binaryExpr == NULL || *binaryExpr == NULL)
        return;

    expr_free(&(*binaryExpr)->left);
    token_free(&(*binaryExpr)->op);
    expr_free(&(*binaryExpr)->right);

    free(*binaryExpr);
    *binaryExpr = NULL;
}

GroupExpr* group_expr_new(Expr* expression) {
    GroupExpr* expr = NULL;
    expr = malloc(sizeof(GroupExpr));
    if (expr == NULL) {
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

    printf("( ");

    Expr* expr = (*groupExpr)->expression;
    if (expr != NULL && expr->to_string != NULL)
        expr->to_string(&expr->expr);

    printf(" )");
}

void group_expr_free(GroupExpr** groupExpr) {
    if (groupExpr == NULL || *groupExpr == NULL)
        return;

    expr_free(&(*groupExpr)->expression);

    free(*groupExpr);
    *groupExpr = NULL;
}

AssignExpr* assign_expr_new(Token* name, Expr* expression) {
    AssignExpr* expr = NULL;
    expr = malloc(sizeof(AssignExpr));
    if (expr == NULL) {
        return NULL;
    }

    *expr = (AssignExpr) {
        .name = name,
        .expression = expression
    };

    return expr;
}

void assign_expr_to_string(AssignExpr** assignExpr) {
    if (assignExpr == NULL || *assignExpr == NULL)
        return;

    printf("let ");

    token_to_string(&(*assignExpr)->name);

    printf(" = ");

    Expr* expr = (*assignExpr)->expression;
    if (expr != NULL && expr->to_string != NULL)
        expr->to_string(&expr->expr);
}

void assign_expr_free(AssignExpr** assignExpr) {
    if (assignExpr == NULL || *assignExpr == NULL)
        return;

    token_free(&(*assignExpr)->name);
    expr_free(&(*assignExpr)->expression);

    free(*assignExpr);
    *assignExpr = NULL;
}

CallExpr* call_expr_new(Expr* calle, List* arguments) {
    CallExpr* expr = NULL;
    expr = malloc(sizeof(CallExpr));
    if (expr == NULL) {
        return NULL;
    }

    *expr = (CallExpr) {
        .calle = calle,
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

    Expr* expr = (*callExpr)->calle;
    if (expr != NULL && expr->to_string != NULL)
        expr->to_string(&expr->expr);

    printf("(");

    List* args = (*callExpr)->arguments;
    for (ListNode* arg = args->head; arg != NULL; arg = arg->next) {
        Expr* expr = arg->value;
        if (expr != NULL && expr->to_string != NULL) {
            expr->to_string((void**) &expr->expr);
        }

        if (arg->next != NULL) {
            printf(", ");
        }
    }

    printf(")");
}

void call_expr_free(CallExpr** callExpr) {
    if (callExpr == NULL || *callExpr == NULL)
        return;

    expr_free(&(*callExpr)->calle);
    list_free(&(*callExpr)->arguments);

    free(*callExpr);
    *callExpr = NULL;
}

LogicalExpr* logical_expr_new(Expr* left, Token* operator, Expr* right) {
    LogicalExpr* expr = NULL;
    expr = malloc(sizeof(BinaryExpr));
    if (expr == NULL) {
        return NULL;
    }

    *expr = (LogicalExpr) {
        .left = left,
        .op = operator,
        .right = right
    };

    return expr;
}

void logical_expr_to_string(LogicalExpr** logicalExpr) {
    if (logicalExpr == NULL || *logicalExpr == NULL)
        return;

    Expr* left = (*logicalExpr)->left;
    if (left != NULL && left->to_string != NULL)
        left->to_string(&left->expr);

    Token* operator = (*logicalExpr)->op;
    if (operator != NULL && operator->literal != NULL)
        printf(" %s ", operator->literal);

    Expr* right = (*logicalExpr)->right;
    if (right != NULL && right->to_string != NULL) {
        right->to_string(&right->expr);
    }
}

void logical_expr_free(LogicalExpr** logicalExpr) {
    if (logicalExpr == NULL || *logicalExpr == NULL)
        return;

    expr_free(&(*logicalExpr)->left);
    token_free(&(*logicalExpr)->op);
    expr_free(&(*logicalExpr)->right);

    free(*logicalExpr);
    *logicalExpr = NULL;
}

LiteralExpr* literal_expr_new(LiteralType type, void* value, void (*to_string)(void**), void (*destroy)(void**)) {
    LiteralExpr* expr = NULL;
    expr = malloc(sizeof(LiteralExpr));
    if (expr == NULL) {
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

    (*literalExpr)->destroy(&(*literalExpr)->value);

    free(*literalExpr);
    *literalExpr = NULL;
}

Expr* expr_new(ExprType type, void* expr, void (*to_string)(void**), void (*destroy)(void**)) {
    Expr* new_expr = NULL;
    new_expr = malloc(sizeof(Expr));
    if (new_expr == NULL) {
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

    (*expr)->to_string(&(*expr)->expr);

    puts("");
}

void expr_free(Expr** expr) {
    if (expr == NULL || *expr == NULL)
        return;

    (*expr)->destroy(&(*expr)->expr);

    free(*expr);
    *expr = NULL;
}
