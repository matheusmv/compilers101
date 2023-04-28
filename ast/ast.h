#pragma once

#include "list.h"
#include "token.h"
#include "types.h"


typedef enum StmtType {
    EXPRESSION_STMT
} StmtType;

typedef enum ExprType {
    BINARY_EXPR,
    GROUP_EXPR,
    ASSIGN_EXPR,
    CALL_EXPR,
    LOGICAL_EXPR,
    UNARY_EXPR,
    LITERAL_EXPR
} ExprType;

typedef struct Stmt {
    StmtType type;
    void* stmt;
    void (*to_string)(void**);
    void (*destroy)(void**);
} Stmt;

Stmt* stmt_new(StmtType type, void* stmt,
    void (*to_string)(void**), void (*destroy)(void**));
void stmt_to_string(Stmt** stmt);
void stmt_free(Stmt** stmt);


typedef struct Expr {
    ExprType type;
    void* expr;
    void (*to_string)(void**);
    void (*destroy)(void**);
} Expr;

Expr* expr_new(ExprType type, void* expr,
    void (*to_string)(void**), void (*destroy)(void**));
void expr_to_string(Expr** expr);
void expr_free(Expr** expr);


typedef struct BinaryExpr {
    Expr* left;
    Token* op;
    Expr* right;
} BinaryExpr;

BinaryExpr* binary_expr_new(Expr* left, Token* op, Expr* right);
void binary_expr_to_string(BinaryExpr** binaryExpr);
void binary_expr_free(BinaryExpr** binaryExpr);


typedef struct GroupExpr {
    Expr* expression;
} GroupExpr;

GroupExpr* group_expr_new(Expr* expression);
void group_expr_to_string(GroupExpr** groupExpr);
void group_expr_free(GroupExpr** groupExpr);


typedef struct AssignExpr {
    Token* name;
    Expr* expression;
} AssignExpr;

AssignExpr* assign_expr_new(Token* name, Expr* expression);
void assign_expr_to_string(AssignExpr** assignExpr);
void assign_expr_free(AssignExpr** assignExpr);


typedef struct CallExpr {
    Expr* calle;
    List* arguments; /* List of (Expr*) */
} CallExpr;

CallExpr* call_expr_new(Expr* calle, List* arguments);
void call_expr_add_argument(CallExpr** callExpr, Expr* argument);
void call_expr_to_string(CallExpr** callExpr);
void call_expr_free(CallExpr** callExpr);


typedef struct LogicalExpr {
    Expr* left;
    Token* op;
    Expr* right;
} LogicalExpr;

LogicalExpr* logical_expr_new(Expr* left, Token* op, Expr* right);
void logical_expr_to_string(LogicalExpr** logicalExpr);
void logical_expr_free(LogicalExpr** logicalExpr);


typedef struct UnaryExpr {
    Token* op;
    Expr* expression;
} UnaryExpr;

UnaryExpr* unary_expr_new(Token* op, Expr* expression);
void unary_expr_to_string(UnaryExpr** unaryExpr);
void unary_expr_free(UnaryExpr** unaryExpr);


typedef struct LiteralExpr {
    LiteralType type;
    void* value; /* IdentType | IntType | FloatType | CharType | StringType | BoolType */
    void (*to_string)(void**);
    void (*destroy)(void**);
} LiteralExpr;

LiteralExpr* literal_expr_new(LiteralType type, void* value,
    void (*to_string)(void**), void (*destroy)(void**));
void literal_expr_to_string(LiteralExpr** literalExpr);
void literal_expr_free(LiteralExpr** literalExpr);


#define NEW_EXPR_STMT(expr)                                                    \
    stmt_new(EXPRESSION_STMT, (expr),                                          \
        (void (*)(void **))expr_to_string,                                     \
        (void (*)(void **))expr_free)

#define STMT_PRINT_AND_FREE(stmt)                                              \
    stmt_to_string((&(stmt)));                                                 \
    printf("\n");                                                              \
    stmt_free((&(stmt)))

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

#define NEW_LOGICAL_EXPR(left, op, right)                                      \
    expr_new(LOGICAL_EXPR, logical_expr_new((left), (op), (right)),            \
        (void (*)(void **))logical_expr_to_string,                             \
        (void (*)(void **))logical_expr_free)

#define NEW_UNARY_EXPR(op, expr)                                               \
    expr_new(UNARY_EXPR, unary_expr_new((op), (expr)),                         \
        (void (*)(void **))unary_expr_to_string,                               \
        (void (*)(void **))unary_expr_free)

#define NEW_LITERAL_EXPR(value)                                                \
    expr_new(LITERAL_EXPR, (value),                                            \
        (void (*)(void **))literal_expr_to_string,                             \
        (void (*)(void **))literal_expr_free)

#define EXPR_PRINT_AND_FREE(expr)                                              \
    expr_to_string((&(expr)));                                                 \
    printf("\n");                                                              \
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
