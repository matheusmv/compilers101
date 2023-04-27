#pragma once

#include "list.h"
#include "token.h"
#include "types.h"


typedef enum ExprType {
    BINARY_EXPR,
    GROUP_EXPR,
    ASSIGN_EXPR,
    CALL_EXPR,
    LITERAL_EXPR
} ExprType;


typedef struct Expr {
    ExprType type;
    void* expr;
    void (*to_string)(void**);
    void (*destroy)(void**);
} Expr;

Expr* expr_new(ExprType type, void* expr,
    void (*to_string)(void**), void (*destroy)(void**));
void expr_to_string(Expr**);
void expr_free(Expr**);


typedef struct BinaryExpr {
    Expr* left;
    Token* op;
    Expr* right;
} BinaryExpr;

BinaryExpr* binary_expr_new(Expr*, Token*, Expr*);
void binary_expr_to_string(BinaryExpr**);
void binary_expr_free(BinaryExpr**);


typedef struct GroupExpr {
    Expr* expression;
} GroupExpr;

GroupExpr* group_expr_new(Expr*);
void group_expr_to_string(GroupExpr**);
void group_expr_free(GroupExpr**);


typedef struct AssignExpr {
    Token* name;
    Expr* expression;
} AssignExpr;

AssignExpr* assign_expr_new(Token*, Expr*);
void assign_expr_to_string(AssignExpr**);
void assign_expr_free(AssignExpr**);


typedef struct CallExpr {
    Expr* calle;
    List* arguments; /* List of (Expr*) */
} CallExpr;

CallExpr* call_expr_new(Expr*, List*);
void call_expr_add_argument(CallExpr**, Expr*);
void call_expr_to_string(CallExpr**);
void call_expr_free(CallExpr**);


typedef struct LiteralExpr {
    LiteralType type;
    void* value; /* IdentType | IntType | FloatType | CharType | StringType | BoolType */
    void (*to_string)(void**);
    void (*destroy)(void**);
} LiteralExpr;

LiteralExpr* literal_expr_new(LiteralType, void*,
    void (*to_string)(void**), void (*destroy)(void**));
void literal_expr_to_string(LiteralExpr**);
void literal_expr_free(LiteralExpr**);


#define NEW_BINARY_EXPR(left, op, right)                                       \
    expr_new(BINARY_EXPR, binary_expr_new((left), (op), (right)),              \
        (void (*)(void **))binary_expr_to_string,                              \
        (void (*)(void **))binary_expr_free)

#define NEW_GROUP_EXPR(expr)                                                   \
    expr_new(GROUP_EXPR, group_expr_new((expr)),                               \
        (void (*)(void **))group_expr_to_string,                               \
        (void (*)(void **))group_expr_free)

#define NEW_ASSIGN_EXPR(name, expr)                                            \
    expr_new(ASSIGN_EXPR, assign_expr_new((name), (expr)),                     \
        (void (*)(void **))assign_expr_to_string,                              \
        (void (*)(void **))assign_expr_free)

#define NEW_CALL_EXPR(calle)                                                   \
    expr_new(CALL_EXPR,                                                        \
        call_expr_new((calle), (list_new((void (*)(void **)) expr_free))),     \
        (void (*)(void **))call_expr_to_string,                                \
        (void (*)(void **))call_expr_free)

#define CALL_EXPR_ADD_ARG(call_expr, arg_expr)                                 \
    call_expr_add_argument((CallExpr**) (&(call_expr)->expr), (arg_expr))

#define NEW_LITERAL_EXPR(value)                                                \
    expr_new(LITERAL_EXPR, (value),                                            \
        (void (*)(void **))literal_expr_to_string,                             \
        (void (*)(void **))literal_expr_free)

#define PRINT_AND_FREE(expr)                                                   \
  expr_to_string((&(expr)));                                                   \
  expr_free((&(expr)))

#define NEW_IDENT(value)                                                       \
    literal_expr_new(TYPE_IDENT, ident_type_new((value)),                      \
                (void (*)(void **))ident_type_to_string,                       \
                (void (*)(void **))ident_type_free)

#define NEW_INT(value)                                                         \
    literal_expr_new(TYPE_INT, int_type_new((value)),                          \
                (void (*)(void **))int_type_to_string,                         \
                (void (*)(void **))int_type_free)

#define NEW_FLOAT(value)                                                       \
    literal_expr_new(TYPE_FLOAT, float_type_new((value)),                      \
                (void (*)(void **))float_type_to_string,                       \
                (void (*)(void **))float_type_free)

#define NEW_CHAR(value)                                                        \
    literal_expr_new(TYPE_CHAR, char_type_new((value)),                        \
                (void (*)(void **))char_type_to_string,                        \
                (void (*)(void **))char_type_free)

#define NEW_STRING(value)                                                      \
    literal_expr_new(TYPE_STRING, string_type_new((value)),                    \
                (void (*)(void **))string_type_to_string,                      \
                (void (*)(void **))string_type_free)

#define NEW_BOOL(value)                                                        \
    literal_expr_new(TYPE_BOOL, bool_type_new((value)),                        \
                (void (*)(void **))bool_type_to_string,                        \
                (void (*)(void **))bool_type_free)
