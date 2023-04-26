#include "ast.h"

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
        .operator= operator,
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

    Token* operator = (*binaryExpr)->operator;
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
    token_free(&(*binaryExpr)->operator);
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
