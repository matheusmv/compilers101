#pragma once

#include "list.h"
#include "literal-type.h"
#include "token.h"
#include "types.h"


typedef enum DeclType {
    LET_DECL,
    CONST_DECL,
    FIELD_DECL,
    FUNC_DECL,
    STRUCT_DECL,
    STMT_DECL
} DeclType;

typedef enum StmtType {
    BLOCK_STMT,
    EXPRESSION_STMT,
    RETURN_STMT,
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
    FIELD_INIT_EXPR,
    STRUCT_INIT_EXPR,
    STRUCT_INLINE_EXPR,
    ARRAY_INIT_EXPR,
    FUNC_EXPR,
    LITERAL_EXPR
} ExprType;


typedef struct Decl {
    DeclType type;
    void* decl;
    void (*to_string)(void**);
    void (*destroy)(void**);
} Decl;

Decl* decl_new(DeclType type, void* decl,
    void (*to_string)(void**), void (*destroy)(void**));
void decl_to_string(Decl** decl);
void decl_free(Decl** decl);


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


typedef struct LetDecl {
    Token* name;
    Type* type;
    Expr* expression;
} LetDecl;

LetDecl* let_decl_new(Token* name, Type* type, Expr* expression);
void let_decl_to_string(LetDecl** letDecl);
void let_decl_free(LetDecl** letDecl);


typedef struct ConstDecl {
    Token* name;
    Type* type;
    Expr* expression;
} ConstDecl;

ConstDecl* const_decl_new(Token* name, Type* type, Expr* expression);
void const_decl_to_string(ConstDecl** constDecl);
void const_decl_free(ConstDecl** constDecl);


typedef struct FieldDecl {
    Token* name;
    Type* type;
} FieldDecl;

FieldDecl* field_decl_new(Token* name, Type* type);
void field_decl_to_string(FieldDecl** fieldDecl);
void field_decl_free(FieldDecl** fieldDecl);


typedef struct FunctionDecl {
    Token* name;
    List* parameters; /* List of (FieldDecl*) */
    List* returnTypes; /* List of (Type*) */
    Stmt* body;
} FunctionDecl;

FunctionDecl* function_decl_new(Token* name, List* parameters, List* returnTypes, Stmt* body);
void function_decl_add_parameter(FunctionDecl** functionDecl, Decl* parameter);
void function_decl_add_return_type(FunctionDecl** functionDecl, Type* type);
void function_decl_to_string(FunctionDecl** functionDecl);
void function_decl_free(FunctionDecl** functionDecl);


typedef struct StructDecl {
    Token* name;
    List* fields; /* List of (FieldDecl*) */
} StructDecl;

StructDecl* struct_decl_new(Token* name, List* fields);
void struct_decl_add_field(StructDecl** structDecl, Decl* field);
void struct_decl_to_string(StructDecl** structDecl);
void struct_decl_free(StructDecl** structDecl);


typedef struct StmtDecl {
    Stmt* stmt;
} StmtDecl;

StmtDecl* stmt_decl_new(Stmt* stmt);
void stmt_decl_to_string(StmtDecl** stmtDecl);
void stmt_decl_free(StmtDecl** stmtDecl);


typedef struct BlockStmt {
    List* declarations; /* List of (Decl*) */
} BlockStmt;

BlockStmt* block_stmt_new(List* statements);
void block_stmt_add_declaration(BlockStmt** blockStmt, Decl* declaration);
void block_stmt_to_string(BlockStmt** blockStmt);
void block_stmt_free(BlockStmt** blockStmt);


typedef struct ExpressionStmt {
    Expr* expression;
} ExpressionStmt;

ExpressionStmt* expression_stmt_new(Expr* expression);
void expression_stmt_to_string(ExpressionStmt** expressionStmt);
void expression_stmt_free(ExpressionStmt** expressionStmt);


typedef struct ReturnStmt {
    Expr* expression;
} ReturnStmt;

ReturnStmt* return_stmt_new(Expr* expression);
void return_stmt_to_string(ReturnStmt** returnStmt);
void return_stmt_free(ReturnStmt** returnStmt);


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
    Decl* initialization;
    Expr* condition;
    Expr* action;
    Stmt* body;
} ForStmt;

ForStmt* for_stmt_new(Decl* initialization, Expr* condition, Expr* action, Stmt* body);
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


typedef struct FieldInitExpr {
    Token* name;
    Expr* value;
} FieldInitExpr;

FieldInitExpr* field_init_expr_new(Token* name, Expr* value);
void field_init_expr_to_string(FieldInitExpr** fieldInit);
void field_init_expr_free(FieldInitExpr** fieldInit);


typedef struct StructInitExpr {
    Token* name;
    List* fields; /* List of (FieldInitExpr*) */
} StructInitExpr;

StructInitExpr* struct_init_expr_new(Token* name, List* fields);
void struct_init_expr_add_field(StructInitExpr** structInit, Expr* field);
void struct_init_expr_to_string(StructInitExpr** structInit);
void struct_init_expr_free(StructInitExpr** structInit);


typedef struct StructInlineExpr {
    Type* type; /* STRUCT_TYPE */
    List* fields; /* List of (FieldInitExpr*) */
} StructInlineExpr;

StructInlineExpr* struct_inline_expr_new(Type* type, List* fields);
void struct_inline_expr_add_field(StructInlineExpr** structInline, Expr* field);
void struct_inline_expr_to_string(StructInlineExpr** structInline);
void struct_inline_expr_free(StructInlineExpr** structInline);


typedef struct ArrayInitExpr {
    Type* type;
    List* elements;
} ArrayInitExpr;

ArrayInitExpr* array_init_expr_new(Type* type, List* elements);
void array_init_expr_add_element(ArrayInitExpr** arrayInit, Expr* element);
void array_init_expr_to_string(ArrayInitExpr** arrayInit);
void array_init_expr_free(ArrayInitExpr** arrayInit);


typedef struct FunctionExpr {
    List* parameters; /* List of (FieldDecl*) */
    List* returnTypes; /* List of (Type*) */
    Stmt* body;
} FunctionExpr;

FunctionExpr* function_expr_new(List* parameters, List* returnTypes, Stmt* body);
void function_expr_add_parameter(FunctionExpr** functionExpr, Decl* parameter);
void function_expr_add_return_type(FunctionExpr** functionExpr, Type* type);
void function_expr_to_string(FunctionExpr** functionExpr);
void function_expr_free(FunctionExpr** functionExpr);


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


#define NEW_LET_DECL(name, type, expr)                                         \
    decl_new(LET_DECL, let_decl_new((name), (type), (expr)),                   \
        (void (*)(void **))let_decl_to_string,                                 \
        (void (*)(void **))let_decl_free)

#define NEW_CONST_DECL(name, type, expr)                                       \
    decl_new(CONST_DECL, const_decl_new((name), (type), (expr)),               \
        (void (*)(void **))const_decl_to_string,                               \
        (void (*)(void **))const_decl_free)

#define NEW_FUNCTION_DECL(name, body)                                          \
    decl_new(FUNC_DECL, function_decl_new(                                     \
            (name),                                                            \
            (list_new((void (*)(void **)) decl_free)),                         \
            (list_new((void (*)(void **)) type_free)),                         \
            (body)),                                                           \
        (void (*)(void **))function_decl_to_string,                            \
        (void (*)(void **))function_decl_free)

#define NEW_FUNCTION_DECL_WITH_PARAMS(name, params, body)                      \
    decl_new(FUNC_DECL, function_decl_new(                                     \
            (name),                                                            \
            (params),                                                          \
            (list_new((void (*)(void **)) type_free)),                         \
            (body)),                                                           \
        (void (*)(void **))function_decl_to_string,                            \
        (void (*)(void **))function_decl_free)

#define NEW_FUNCTION_DECL_WITH_RETURNS(name, retrns, body)                     \
    decl_new(FUNC_DECL, function_decl_new(                                     \
            (name),                                                            \
            (list_new((void (*)(void **)) decl_free)),                         \
            (retrns),                                                          \
            (body)),                                                           \
        (void (*)(void **))function_decl_to_string,                            \
        (void (*)(void **))function_decl_free)

#define NEW_FUNCTION_DECL_WITH_PARAMS_AND_RETURNS(name, params, retrns, body)  \
    decl_new(FUNC_DECL, function_decl_new(                                     \
            (name),                                                            \
            (params),                                                          \
            (retrns),                                                          \
            (body)),                                                           \
        (void (*)(void **))function_decl_to_string,                            \
        (void (*)(void **))function_decl_free)

#define FUNCTION_ADD_PARAM(func, param)                                        \
    function_decl_add_parameter((FunctionDecl**) (&(func)->decl), (param))

#define FUNCTION_ADD_PARAMS(func, ...)                                         \
    do {                                                                       \
        Decl* decls[] = { __VA_ARGS__ };                                       \
        size_t n_decls = sizeof(decls) / sizeof(decls[0]);                     \
        for (size_t i = 0; i < n_decls; i++) {                                 \
            FUNCTION_ADD_PARAM((func), decls[i]);                              \
        }                                                                      \
    } while(0)

#define FUNCTION_ADD_RETURN_TYPE(func, type)                                   \
    function_decl_add_return_type((FunctionDecl**) (&(func)->decl), (type))

#define FUNCTION_ADD_RETURN_TYPES(func, ...)                                   \
    do {                                                                       \
        Type* types[] = { __VA_ARGS__ };                                       \
        size_t n_types = sizeof(types) / sizeof(types[0]);                     \
        for (size_t i = 0; i < n_types; i++) {                                 \
            FUNCTION_ADD_RETURN_TYPE((func), types[i]);                        \
        }                                                                      \
    } while(0)

#define NEW_FIELD_DECL(name, type)                                             \
    decl_new(FIELD_DECL, field_decl_new((name), (type)),                       \
        (void (*)(void **))field_decl_to_string,                               \
        (void (*)(void **))field_decl_free)

#define NEW_STRUCT_DECL(name)                                                  \
    decl_new(STRUCT_DECL,                                                      \
        struct_decl_new((name),                                                \
            (list_new((void (*)(void **)) decl_free))),                        \
        (void (*)(void **))struct_decl_to_string,                              \
        (void (*)(void **))struct_decl_free)

#define NEW_STRUCT_DECL_WITH_FIELDS(name, fields)                              \
    decl_new(STRUCT_DECL,                                                      \
        struct_decl_new((name),                                                \
            (fields)),                                                         \
        (void (*)(void **))struct_decl_to_string,                              \
        (void (*)(void **))struct_decl_free)

#define STRUCT_DECL_ADD_FIELD(struct_decl, field_decl)                         \
    struct_decl_add_field(                                                     \
        (StructDecl**) (&(struct_decl)->decl), (field_decl))

#define STRUCT_DECL_ADD_FIELDS(struct_decl, ...)                               \
    do {                                                                       \
        Decl* decls[] = { __VA_ARGS__ };                                       \
        size_t n_decls = sizeof(decls) / sizeof(decls[0]);                     \
        for (size_t i = 0; i < n_decls; i++) {                                 \
            STRUCT_DECL_ADD_FIELD((struct_decl), decls[i]);                    \
        }                                                                      \
    } while(0)

#define NEW_STMT_DECL(stmt)                                                    \
    decl_new(STMT_DECL, stmt_decl_new((stmt)),                                 \
        (void (*)(void **))stmt_decl_to_string,                                \
        (void (*)(void **))stmt_decl_free)

#define DECL_PRINT_AND_FREE(decl)                                              \
    decl_to_string((&(decl)));                                                 \
    printf("\n");                                                              \
    decl_free((&(decl)))

#define NEW_BLOCK_STMT()                                                       \
    stmt_new(BLOCK_STMT,                                                       \
        block_stmt_new((list_new((void (*)(void **)) decl_free))),             \
        (void (*)(void **))block_stmt_to_string,                               \
        (void (*)(void **))block_stmt_free)

#define NEW_BLOCK_STMT_WITH_DECLS(declarations)                                \
    stmt_new(BLOCK_STMT,                                                       \
        block_stmt_new((declarations)),                                        \
        (void (*)(void **))block_stmt_to_string,                               \
        (void (*)(void **))block_stmt_free)

#define BLOCK_STMT_ADD_DECL(block_stmt, arg_decl)                              \
    block_stmt_add_declaration((BlockStmt**) (&(block_stmt)->stmt), (arg_decl))

#define BLOCK_STMT_ADD_DECLS(block_stmt, ...)                                  \
    do {                                                                       \
        Decl* decls[] = { __VA_ARGS__ };                                       \
        size_t n_decls = sizeof(decls) / sizeof(decls[0]);                     \
        for (size_t i = 0; i < n_decls; i++) {                                 \
            BLOCK_STMT_ADD_DECL((block_stmt), decls[i]);                       \
        }                                                                      \
    } while(0)

#define NEW_EXPR_STMT(expr)                                                    \
    stmt_new(EXPRESSION_STMT, expression_stmt_new((expr)),                     \
        (void (*)(void **))expression_stmt_to_string,                          \
        (void (*)(void **))expression_stmt_free)

#define NEW_RETURN_STMT(expr)                                                  \
    stmt_new(RETURN_STMT, return_stmt_new((expr)),                             \
        (void (*)(void **))return_stmt_to_string,                              \
        (void (*)(void **))return_stmt_free)

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

#define NEW_CALL_EXPR_WITH_ARGS(callee, args)                                  \
    expr_new(CALL_EXPR,                                                        \
        call_expr_new((callee), (args)),                                       \
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

#define NEW_FIELD_EXPR(name, expr)                                             \
    expr_new(FIELD_INIT_EXPR, field_init_expr_new((name), (expr)),             \
        (void (*)(void **))field_init_expr_to_string,                          \
        (void (*)(void **))field_init_expr_free)

#define NEW_STRUCT_INIT_EXPR(name)                                             \
    expr_new(STRUCT_INIT_EXPR,                                                 \
        struct_init_expr_new((name),                                           \
            (list_new((void (*)(void **)) expr_free))),                        \
        (void (*)(void **))struct_init_expr_to_string,                         \
        (void (*)(void **))struct_init_expr_free)

#define NEW_STRUCT_INIT_EXPR_WITH_FIELDS(name, fields)                         \
    expr_new(STRUCT_INIT_EXPR,                                                 \
        struct_init_expr_new((name), (fields)),                                \
        (void (*)(void **))struct_init_expr_to_string,                         \
        (void (*)(void **))struct_init_expr_free)

#define STRUCT_INIT_EXPR_ADD_FIELD(struct_init_expr, field_expr)               \
    struct_init_expr_add_field(                                                \
        (StructInitExpr**) (&(struct_init_expr)->expr), (field_expr))

#define STRUCT_INIT_EXPR_ADD_FIELDS(struct_init_expr, ...)                     \
    do {                                                                       \
        Expr* exprs[] = { __VA_ARGS__ };                                       \
        size_t n_exprs = sizeof(exprs) / sizeof(exprs[0]);                     \
        for (size_t i = 0; i < n_exprs; i++) {                                 \
            STRUCT_INIT_EXPR_ADD_FIELD((struct_init_expr), exprs[i]);          \
        }                                                                      \
    } while(0)

#define NEW_STRUCT_INLINE_EXPR(struct_type)                                    \
    expr_new(STRUCT_INLINE_EXPR,                                               \
        struct_inline_expr_new((struct_type),                                  \
            (list_new((void (*)(void **)) expr_free))),                        \
        (void (*)(void **))struct_inline_expr_to_string,                       \
        (void (*)(void **))struct_inline_expr_free)

#define NEW_STRUCT_INLINE_EXPR_WITH_FIELDS(struct_type, fields)                \
    expr_new(STRUCT_INLINE_EXPR,                                               \
        struct_inline_expr_new((struct_type), (fields)),                       \
        (void (*)(void **))struct_inline_expr_to_string,                       \
        (void (*)(void **))struct_inline_expr_free)

#define STRUCT_INLINE_EXPR_ADD_FIELD(struct_inline_expr, field_expr)           \
    struct_init_expr_add_field(                                                \
        (StructInlineExpr**) (&(struct_inline_expr)->expr), (field_expr))

#define STRUCT_INLINE_EXPR_ADD_FIELDS(struct_inline_expr, ...)                 \
    do {                                                                       \
        Expr* exprs[] = { __VA_ARGS__ };                                       \
        size_t n_exprs = sizeof(exprs) / sizeof(exprs[0]);                     \
        for (size_t i = 0; i < n_exprs; i++) {                                 \
            STRUCT_INLINE_EXPR_ADD_FIELD((struct_inline_expr), exprs[i]);      \
        }                                                                      \
    } while(0)

#define NEW_ARRAY_INIT_EXPR(array_type)                                        \
    expr_new(ARRAY_INIT_EXPR,                                                  \
        array_init_expr_new((array_type),                                      \
            (list_new((void (*)(void **)) expr_free))),                        \
        (void (*)(void **))array_init_expr_to_string,                          \
        (void (*)(void **))array_init_expr_free)

#define NEW_ARRAY_INIT_EXPR_WITH_ELEMENTS(array_type, elements)                \
    expr_new(ARRAY_INIT_EXPR,                                                  \
        array_init_expr_new((array_type), (elements)),                         \
        (void (*)(void **))array_init_expr_to_string,                          \
        (void (*)(void **))array_init_expr_free)

#define ARRAY_INIT_EXPR_ADD_ELEMENT(array_init_expr, element_expr)             \
    array_init_expr_add_element(                                               \
        (ArrayInitExpr**) (&(array_init_expr)->expr), (element_expr))

#define ARRAY_INIT_EXPR_ADD_ELEMENTS(array_init_expr, ...)                     \
    do {                                                                       \
        Expr* exprs[] = { __VA_ARGS__ };                                       \
        size_t n_exprs = sizeof(exprs) / sizeof(exprs[0]);                     \
        for (size_t i = 0; i < n_exprs; i++) {                                 \
            ARRAY_INIT_EXPR_ADD_ELEMENT((array_init_expr), exprs[i]);          \
        }                                                                      \
    } while(0)

#define NEW_FUNCTION_EXPR(body)                                                \
    expr_new(FUNC_EXPR, function_expr_new(                                     \
            (list_new((void (*)(void **)) decl_free)),                         \
            (list_new((void (*)(void **)) type_free)),                         \
            (body)),                                                           \
        (void (*)(void **))function_expr_to_string,                            \
        (void (*)(void **))function_expr_free)

#define NEW_FUNCTION_EXPR_WITH_PARAMS(params, body)                            \
    expr_new(FUNC_EXPR, function_expr_new(                                     \
            (params),                                                          \
            (list_new((void (*)(void **)) type_free)),                         \
            (body)),                                                           \
        (void (*)(void **))function_expr_to_string,                            \
        (void (*)(void **))function_expr_free)

#define NEW_FUNCTION_EXPR_WITH_RETURNS(retrns, body)                           \
    expr_new(FUNC_EXPR, function_expr_new(                                     \
            (list_new((void (*)(void **)) decl_free)),                         \
            (retrns),                                                          \
            (body)),                                                           \
        (void (*)(void **))function_expr_to_string,                            \
        (void (*)(void **))function_expr_free)

#define NEW_FUNCTION_EXPR_WITH_PARAMS_AND_RETURNS(params, retrns, body)        \
    expr_new(FUNC_EXPR, function_expr_new(                                     \
            (params),                                                          \
            (retrns),                                                          \
            (body)),                                                           \
        (void (*)(void **))function_expr_to_string,                            \
        (void (*)(void **))function_expr_free)

#define FUNCTION_EXPR_ADD_PARAM(func, param)                                   \
    function_expr_add_parameter((FunctionExpr**) (&(func)->expr), (param))

#define FUNCTION_EXPR_ADD_PARAMS(func, ...)                                    \
    do {                                                                       \
        Decl* params[] = { __VA_ARGS__ };                                      \
        size_t n_params = sizeof(params) / sizeof(params[0]);                  \
        for (size_t i = 0; i < n_params; i++) {                                \
            FUNCTION_EXPR_ADD_PARAM((func), params[i]);                        \
        }                                                                      \
    } while(0)

#define FUNCTION_EXPR_ADD_RETURN_TYPE(func, type)                              \
    function_expr_add_return_type((FunctionExpr**) (&(func)->expr), (type))

#define FUNCTION_EXPR_ADD_RETURN_TYPES(func, ...)                              \
    do {                                                                       \
        Type* types[] = { __VA_ARGS__ };                                       \
        size_t n_types = sizeof(types) / sizeof(types[0]);                     \
        for (size_t i = 0; i < n_types; i++) {                                 \
            FUNCTION_EXPR_ADD_RETURN_TYPE((func), types[i]);                   \
        }                                                                      \
    } while(0)

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
