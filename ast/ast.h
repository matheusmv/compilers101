#pragma once

#include "list.h"
#include "literal-type.h"
#include "token.h"
#include "types.h"


typedef enum StmtType {
    BLOCK_STMT,
    EXPRESSION_STMT,
    FUNCTION_STMT,
    RETURN_STMT,
    LET_STMT,
    IF_STMT,
    WHILE_STMT,
    FOR_STMT
} StmtType;

typedef enum ExprType {
    BINARY_EXPR,
    GROUP_EXPR,
    ASSIGN_EXPR,
    CALL_EXPR,
    LOGICAL_EXPR,
    UNARY_EXPR,
    UPDATE_EXPR,
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


typedef struct BlockStmt {
    List* statements; /* List of (Stmt*) */
} BlockStmt;

BlockStmt* block_stmt_new(List* statements);
void block_stmt_add_statement(BlockStmt** blockStmt, Stmt* statement);
void block_stmt_to_string(BlockStmt** blockStmt);
void block_stmt_free(BlockStmt** blockStmt);


typedef struct ExpressionStmt {
    Expr* expression;
} ExpressionStmt;

ExpressionStmt* expression_stmt_new(Expr* expression);
void expression_stmt_to_string(ExpressionStmt** expressionStmt);
void expression_stmt_free(ExpressionStmt** expressionStmt);


typedef struct FunctionStmt {
    Token* name;
    List* parameters; /* List of (Expr*) */
    Stmt* body;
} FunctionStmt;

FunctionStmt* function_stmt_new(Token* name, List* parameters, Stmt* body);
void function_stmt_add_parameter(FunctionStmt** functionStmt, Expr* parameter);
void function_stmt_to_string(FunctionStmt** functionStmt);
void function_stmt_free(FunctionStmt** functionStmt);


typedef struct ReturnStmt {
    Expr* expression;
} ReturnStmt;

ReturnStmt* return_stmt_new(Expr* expression);
void return_stmt_to_string(ReturnStmt** returnStmt);
void return_stmt_free(ReturnStmt** returnStmt);


typedef struct LetStmt {
    Token* name;
    Expr* expression;
} LetStmt;

LetStmt* let_stmt_new(Token* name, Expr* expression);
void let_stmt_to_string(LetStmt** letStmt);
void let_stmt_free(LetStmt** letStmt);


typedef struct IfStmt {
    Expr* condition;
    Stmt* thenBranch;
    Stmt* elseBranch;
} IfStmt;

IfStmt* if_stmt_new(Expr* condition, Stmt* thenBranch, Stmt* elseBranch);
void if_stmt_to_string(IfStmt** ifStmt);
void if_stmt_free(IfStmt** ifStmt);


typedef struct WhileStmt {
    Expr* condition;
    Stmt* body;
} WhileStmt;

WhileStmt* while_stmt_new(Expr* condition, Stmt* body);
void while_stmt_to_string(WhileStmt** whileStmt);
void while_stmt_free(WhileStmt** whileStmt);


typedef struct ForStmt {
    Stmt* initialization;
    Expr* condition;
    Expr* action;
    Stmt* body;
} ForStmt;

ForStmt* for_stmt_new(Stmt* initialization, Expr* condition, Expr* action, Stmt* body);
void for_stmt_to_string(ForStmt** forStmt);
void for_stmt_free(ForStmt** forStmt);


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
    Expr* identifier;
    Token* op;
    Expr* expression;
} AssignExpr;

AssignExpr* assign_expr_new(Expr* identifier, Token* op, Expr* expression);
void assign_expr_to_string(AssignExpr** assignExpr);
void assign_expr_free(AssignExpr** assignExpr);


typedef struct CallExpr {
    Expr* callee;
    List* arguments; /* List of (Expr*) */
} CallExpr;

CallExpr* call_expr_new(Expr* callee, List* arguments);
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


typedef struct UpdateExpr {
    Expr* expression;
    Token* op; /* Expr++ | Expr-- */
} UpdateExpr;

UpdateExpr* update_expr_new(Expr* expression, Token* op);
void update_expr_to_string(UpdateExpr** updateExpr);
void update_expr_free(UpdateExpr** updateExpr);


typedef struct LiteralExpr {
    LiteralType type;
    void* value; /* Ident | Int | Float | Char | String | Bool | Void | Nil */
    void (*to_string)(void**);
    void (*destroy)(void**);
} LiteralExpr;

LiteralExpr* literal_expr_new(LiteralType type, void* value,
    void (*to_string)(void**), void (*destroy)(void**));
void literal_expr_to_string(LiteralExpr** literalExpr);
void literal_expr_free(LiteralExpr** literalExpr);


#define NEW_BLOCK_STMT()                                                       \
    stmt_new(BLOCK_STMT,                                                       \
        block_stmt_new((list_new((void (*)(void **)) stmt_free))),             \
        (void (*)(void **))block_stmt_to_string,                               \
        (void (*)(void **))block_stmt_free)

#define BLOCK_STMT_ADD_STMT(block_stmt, arg_stmt)                              \
    block_stmt_add_statement((BlockStmt**) (&(block_stmt)->stmt), (arg_stmt))

#define BLOCK_STMT_ADD_STMTS(block_stmt, ...)                                  \
    do {                                                                       \
        Stmt* stmts[] = { __VA_ARGS__ };                                       \
        size_t n_stmts = sizeof(stmts) / sizeof(stmts[0]);                     \
        for (size_t i = 0; i < n_stmts; i++) {                                 \
            BLOCK_STMT_ADD_STMT((block_stmt), stmts[i]);                       \
        }                                                                      \
    } while(0)

#define NEW_EXPR_STMT(expr)                                                    \
    stmt_new(EXPRESSION_STMT, expression_stmt_new((expr)),                     \
        (void (*)(void **))expression_stmt_to_string,                          \
        (void (*)(void **))expression_stmt_free)

#define NEW_FUNCTION_STMT(name, body)                                          \
    stmt_new(FUNCTION_STMT, function_stmt_new(                                 \
            (name),                                                            \
            (list_new((void (*)(void **)) expr_free)),                         \
            (body)),                                                           \
        (void (*)(void **))function_stmt_to_string,                            \
        (void (*)(void **))function_stmt_free)

#define FUNCTION_ADD_PARAM(func, param)                                        \
    function_stmt_add_parameter((FunctionStmt**) (&(func)->stmt), (param))

#define FUNCTION_ADD_PARAMS(func, ...)                                         \
    do {                                                                       \
        Expr* exprs[] = { __VA_ARGS__ };                                       \
        size_t n_exprs = sizeof(exprs) / sizeof(exprs[0]);                     \
        for (size_t i = 0; i < n_exprs; i++) {                                 \
            FUNCTION_ADD_PARAM((func), exprs[i]);                              \
        }                                                                      \
    } while(0)

#define NEW_RETURN_STMT(expr)                                                  \
    stmt_new(RETURN_STMT, return_stmt_new((expr)),                             \
        (void (*)(void **))return_stmt_to_string,                              \
        (void (*)(void **))return_stmt_free)

#define NEW_LET_STMT(name, expr)                                               \
    stmt_new(LET_STMT, let_stmt_new((name), (expr)),                           \
        (void (*)(void **))let_stmt_to_string,                                 \
        (void (*)(void **))let_stmt_free)

#define NEW_IF_STMT(cond_expr, then_block, else_block)                         \
    stmt_new(IF_STMT, if_stmt_new((cond_expr), (then_block), (else_block)),    \
        (void (*)(void **))if_stmt_to_string,                                  \
        (void (*)(void **))if_stmt_free)

#define NEW_WHILE_STMT(cond_expr, body)                                        \
    stmt_new(WHILE_STMT, while_stmt_new((cond_expr), (body)),                  \
        (void (*)(void **))while_stmt_to_string,                               \
        (void (*)(void **))while_stmt_free)

#define NEW_FOR_STMT(init, cond_expr, action, body)                            \
    stmt_new(FOR_STMT, for_stmt_new((init), (cond_expr), (action), (body)),    \
        (void (*)(void **))for_stmt_to_string,                                 \
        (void (*)(void **))for_stmt_free)

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

#define NEW_ASSIGN_EXPR(ident, op, expr)                                       \
    expr_new(ASSIGN_EXPR, assign_expr_new((ident), (op), (expr)),              \
        (void (*)(void **))assign_expr_to_string,                              \
        (void (*)(void **))assign_expr_free)

#define NEW_CALL_EXPR(callee)                                                  \
    expr_new(CALL_EXPR,                                                        \
        call_expr_new((callee), (list_new((void (*)(void **)) expr_free))),    \
        (void (*)(void **))call_expr_to_string,                                \
        (void (*)(void **))call_expr_free)

#define CALL_EXPR_ADD_ARG(call_expr, arg_expr)                                 \
    call_expr_add_argument((CallExpr**) (&(call_expr)->expr), (arg_expr))

#define CALL_EXPR_ADD_ARGS(call_expr, ...)                                     \
    do {                                                                       \
        Expr* exprs[] = { __VA_ARGS__ };                                       \
        size_t n_exprs = sizeof(exprs) / sizeof(exprs[0]);                     \
        for (size_t i = 0; i < n_exprs; i++) {                                 \
            CALL_EXPR_ADD_ARG((call_expr), exprs[i]);                          \
        }                                                                      \
    } while(0)

#define NEW_LOGICAL_EXPR(left, op, right)                                      \
    expr_new(LOGICAL_EXPR, logical_expr_new((left), (op), (right)),            \
        (void (*)(void **))logical_expr_to_string,                             \
        (void (*)(void **))logical_expr_free)

#define NEW_UNARY_EXPR(op, expr)                                               \
    expr_new(UNARY_EXPR, unary_expr_new((op), (expr)),                         \
        (void (*)(void **))unary_expr_to_string,                               \
        (void (*)(void **))unary_expr_free)

#define NEW_UPDATE_EXPR(expr, op)                                              \
    expr_new(UPDATE_EXPR, update_expr_new((expr), (op)),                       \
        (void (*)(void **))update_expr_to_string,                              \
        (void (*)(void **))update_expr_free)

#define NEW_LITERAL_EXPR(value)                                                \
    expr_new(LITERAL_EXPR, (value),                                            \
        (void (*)(void **))literal_expr_to_string,                             \
        (void (*)(void **))literal_expr_free)

#define EXPR_PRINT_AND_FREE(expr)                                              \
    expr_to_string((&(expr)));                                                 \
    printf("\n");                                                              \
    expr_free((&(expr)))

#define NEW_IDENT(value)                                                       \
    literal_expr_new(IDENT_LITERAL, ident_literal_new((value)),                \
                (void (*)(void **))ident_literal_to_string,                    \
                (void (*)(void **))ident_literal_free)

#define NEW_IDENT_LITERAL(value) NEW_LITERAL_EXPR(NEW_IDENT((value)))

#define NEW_INT(value)                                                         \
    literal_expr_new(INT_LITERAL, int_literal_new((value)),                    \
                (void (*)(void **))int_literal_to_string,                      \
                (void (*)(void **))int_literal_free)

#define NEW_INT_LITERAL(value) NEW_LITERAL_EXPR(NEW_INT((value)))

#define NEW_FLOAT(value)                                                       \
    literal_expr_new(FLOAT_LITERAL, float_literal_new((value)),                \
                (void (*)(void **))float_literal_to_string,                    \
                (void (*)(void **))float_literal_free)

#define NEW_FLOAT_LITERAL(value) NEW_LITERAL_EXPR(NEW_FLOAT((value)))

#define NEW_CHAR(value)                                                        \
    literal_expr_new(CHAR_LITERAL, char_literal_new((value)),                  \
                (void (*)(void **))char_literal_to_string,                     \
                (void (*)(void **))char_literal_free)

#define NEW_CHAR_LITERAL(value) NEW_LITERAL_EXPR(NEW_CHAR((value)))

#define NEW_STRING(value)                                                      \
    literal_expr_new(STRING_LITERAL, string_literal_new((value)),              \
                (void (*)(void **))string_literal_to_string,                   \
                (void (*)(void **))string_literal_free)

#define NEW_STRING_LITERAL(value) NEW_LITERAL_EXPR(NEW_STRING((value)))

#define NEW_BOOL(value)                                                        \
    literal_expr_new(BOOL_LITERAL, bool_literal_new((value)),                  \
                (void (*)(void **))bool_literal_to_string,                     \
                (void (*)(void **))bool_literal_free)

#define NEW_BOOL_LITERAL(value) NEW_LITERAL_EXPR(NEW_BOOL((value)))

#define NEW_VOID()                                                             \
    literal_expr_new(VOID_LITERAL, void_literal_new(),                         \
                (void (*)(void **))void_literal_to_string,                     \
                (void (*)(void **))void_literal_free)

#define NEW_VOID_LITERAL() NEW_LITERAL_EXPR(NEW_VOID())

#define NEW_NIL()                                                              \
    literal_expr_new(NIL_LITERAL, nil_literal_new(),                           \
                (void (*)(void **))nil_literal_to_string,                      \
                (void (*)(void **))nil_literal_free)

#define NEW_NIL_LITERAL() NEW_LITERAL_EXPR(NEW_NIL())
