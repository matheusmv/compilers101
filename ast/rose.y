%code {

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/ast.h"
#include "src/smem.h"

bool success = true;

extern int yylineno;

extern int yylex(void);

void yyerror(const char*);

extern List* declarations;

}

%token
    ADD "+"
    SUB "-"
    MUL "*"
    QUO "/"
    REM "%"

    TILDE "~"
    AND   "&"
    OR    "|"
    XOR   "^"
    SHL   "<<"
    SHR   ">>"

    ADD_ASSIGN "+="
    SUB_ASSIGN "-="
    MUL_ASSIGN "*="
    QUO_ASSIGN "/="
    REM_ASSIGN "%="

    AND_ASSIGN "&="
    OR_ASSIGN  "|="
    XOR_ASSIGN "^="
    SHL_ASSIGN "<<="
    SHR_ASSIGN ">>="

    COND "?"

    LOR  "||"
    LAND "&&"

    INC "++"
    DEC "--"

    EQL    "=="
    LSS    "<"
    GTR    ">"
    ASSIGN "="
    NOT    "!"

    NEQ      "!="
    LEQ      "<="
    GEQ      ">="
    ELLIPSIS "..."

    LPAREN "("
    LBRACK "["
    LBRACE "{"
    COMMA  ","
    PERIOD "."

    RPAREN    ")"
    RBRACK    "]"
    RBRACE    "}"
    SEMICOLON ";"
    COLON     ":"

    LET      "let"
    CONST    "const"
    IF       "if"
    ELSE     "else"
    FUNC     "func"
    RETURN   "return"
    WHILE    "while"
    FOR      "for"
    BREAK    "break"
    CONTINUE "continue"
    NIL      "nil"
    TRUE     "true"
    FALSE    "false"
    STRUCT   "struct"

    TYPE_INT    "int"
    TYPE_FLOAT  "float"
    TYPE_CHAR   "char"
    TYPE_STRING "string"
    TYPE_BOOL   "bool"
    TYPE_VOID   "void"
    ;

%union {
    char*  str_value;
    int    int_value;
    double float_value;
    char   char_value;

    struct Token* token_t;

    struct Type* type_t;

    struct List* list_t;

    struct Decl* decl_t;
    struct Stmt* stmt_t;
    struct Expr* expr_t;
}

%token <str_value> ILLEGAL IDENT STRING
%token <int_value> INT
%token <float_value> FLOAT
%token <char_value> CHAR

%nterm <token_t> PostfixOperator UnaryOperator MultiplicativeOperator AdditiveOperator
                ShiftOperator RelationalOperator EqualityOperator AssignmentOperator

%nterm <type_t> TypeDeclaration StructType AtomicType FunctionType ArrayType
                ArrayDimension ValidArrayType NamedType FunctionReturnType

%nterm <list_t> Declarations ArrayArguments FunctionParameterTypeList StructArguments
                StructInitializationListExpression CallExpressionArguments
                StructFieldsDeclaration FunctionParametersDeclaration
                StructNamedTypesDeclaration ArrayDimensionList MemberAccessList
                ArrayIndexAccessList

%nterm <decl_t> Statement StructDeclaration FunctionDeclaration IdentifierDeclaration
                ConstDeclaration LetDeclaration Declaration ForInitializer

%nterm <stmt_t> ExpressionStatement BlockStatement ForStatement WhileStatement IfStatement
                ElseStatement ContinueStatement BreakStatement ReturnStatement FunctionBody

%nterm <expr_t> Expression StructArgumentsExpession StructInitializationExpression
                Literal Identifier GroupExpression ArrayInitializationExpression
                PrimaryExpression CastExpression FunctionExpression PostfixExpression
                MemberExpression CallExpression ArrayMemberExpression
                UnaryExpression MultiplicativeExpression AdditiveExpression ShiftExpression
                RelationalExpression EqualityExpression AndExpression XorExpression OrExpression
                LogicalAndExpression LogicalOrExpression ConditionalExpression AssignmentExpression
                ForCondition ForIncrement

%nonassoc ":" ";" ","

%right "=" "+=" "-=" "*=" "/=" "%=" "&=" "|=" "^=" "<<=" ">>="

%nonassoc "?"
%left "||"
%left "&&"
%left "|"
%left "^"
%left "&"
%nonassoc "==" "!="
%nonassoc "<" ">" "<=" ">="
%left "<<" ">>"
%left "+" "-"
%left "*" "/" "%"
%nonassoc UNARY "~" "!"
%left "++" "--"

%define parse.error verbose

%start Program

%%

Program
    : %empty
    | Declarations
        {
            declarations = $1;
        }
    ;

Declarations
    : Declaration
        {
            List* list = list_new((void(*)(void**)) decl_free);
            list_insert_last(&list, $1);
            $$ = list;
        }
    | Declarations Declaration
        {
            list_insert_last(&$1, $2);
            $$ = $1;
        }
    ;

Declaration
    : Statement
        {
            $$ = $1;
        }
    | LetDeclaration Semicolon
        {
            $$ = $1;
        }
    | ConstDeclaration Semicolon
        {
            $$ = $1;
        }
    | FunctionDeclaration
        {
            $$ = $1;
        }
    | StructDeclaration
        {
            $$ = $1;
        }
    ;

Semicolon
    : ";"
    ;

LetDeclaration
    : "let" IDENT
        {
            Decl* decl = NEW_LET_DECL(
                NEW_TOKEN(TOKEN_IDENT, $2, yylineno),
                NULL,
                NULL
            );
            safe_free((void**) &$2);
            $$ = decl;
        }
    | "let" IDENT "=" Expression
        {
            Decl* decl = NEW_LET_DECL(
                NEW_TOKEN(TOKEN_IDENT, $2, yylineno),
                NULL,
                $4
            );
            safe_free((void**) &$2);
            $$ = decl;
        }
    | "let" IDENT ":" TypeDeclaration
        {
            Decl* decl = NEW_LET_DECL(
                NEW_TOKEN(TOKEN_IDENT, $2, yylineno),
                $4,
                NULL
            );
            safe_free((void**) &$2);
            $$ = decl;
        }
    | "let" IDENT ":" TypeDeclaration "=" Expression
        {
            Decl* decl = NEW_LET_DECL(
                NEW_TOKEN(TOKEN_IDENT, $2, yylineno),
                $4,
                $6
            );
            safe_free((void**) &$2);
            $$ = decl;
        }
    ;

ConstDeclaration
    : "const" IDENT "=" Expression
        {
            Decl* decl = NEW_CONST_DECL(
                NEW_TOKEN(TOKEN_IDENT, $2, yylineno),
                NULL,
                $4
            );
            safe_free((void**) &$2);
            $$ = decl;
        }
    | "const" IDENT ":" TypeDeclaration "=" Expression
        {
            Decl* decl = NEW_CONST_DECL(
                NEW_TOKEN(TOKEN_IDENT, $2, yylineno),
                $4,
                $6
            );
            safe_free((void**) &$2);
            $$ = decl;
        }
    ;

FunctionDeclaration
    : "func" IDENT "(" FunctionParametersDeclaration ")" FunctionBody
        {
            Decl* decl = NEW_FUNCTION_DECL_WITH_PARAMS(
                NEW_TOKEN(TOKEN_IDENT, $2, yylineno),
                $4,
                $6
            );
            safe_free((void**) &$2);
            $$ = decl;
        }
    | "func" IDENT "(" FunctionParametersDeclaration ")" ":" FunctionReturnType FunctionBody
        {
            Decl* decl = NEW_FUNCTION_DECL_WITH_PARAMS_AND_RETURN(
                NEW_TOKEN(TOKEN_IDENT, $2, yylineno),
                $4,
                $7,
                $8
            );
            safe_free((void**) &$2);
            $$ = decl;
        }
    ;

FunctionBody
    : BlockStatement
        {
            $$ = $1;
        }
    ;

StructDeclaration
    : "struct" IDENT "{" StructFieldsDeclaration "}"
        {
            Decl* decl = NEW_STRUCT_DECL_WITH_FIELDS(
                NEW_TOKEN(TOKEN_IDENT, $2, yylineno),
                $4
            );
            safe_free((void**) &$2);
            $$ = decl;
        }
    ;

IdentifierDeclaration
    : IDENT ":" TypeDeclaration
        {
            Decl* decl = NEW_FIELD_DECL(
                NEW_TOKEN(TOKEN_IDENT, $1, yylineno),
                $3
            );
            safe_free((void**) &$1);
            $$ = decl;
        }
    ;

TypeDeclaration
    : IDENT
        {
            Type* type = NEW_CUSTOM_TYPE(0, $1);
            safe_free((void**) &$1);
            $$ = type;
        }
    | AtomicType
        {
            $$ = $1;
        }
    | FunctionType
        {
            $$ = $1;
        }
    | StructType
        {
            $$ = $1;
        }
    | ArrayType
        {
            $$ = $1;
        }
    ;

AtomicType
    : "int"
        {
            $$ = NEW_INT_TYPE();
        }
    | "float"
        {
            $$ = NEW_FLOAT_TYPE();
        }
    | "char"
        {
            $$ = NEW_CHAR_TYPE();
        }
    | "string"
        {
            $$ = NEW_STRING_TYPE();
        }
    | "bool"
        {
            $$ = NEW_BOOL_TYPE();
        }
    | "void"
        {
            $$ = NEW_VOID_TYPE();
        }
    | "nil"
        {
            $$ = NEW_NIL_TYPE();
        }
    ;

FunctionType
    : "func" "(" FunctionParameterTypeList ")"
        {
            $$ = NEW_FUNCTION_TYPE_WITH_PARAMS($3);
        }
    | "func" "(" FunctionParameterTypeList ")" ":" FunctionReturnType
        {
            $$ = NEW_FUNCTION_TYPE_WITH_PARAMS_AND_RETURN($3, $6);
        }
    ;

FunctionParameterTypeList
    : %empty
        {
            $$ = list_new((void (*)(void **)) type_free);
        }
    | TypeDeclaration
        {
            List* list = list_new((void (*)(void **)) type_free);
            list_insert_last(&list, $1);
            $$ = list;
        }
    | FunctionParameterTypeList "," TypeDeclaration
        {
            list_insert_last(&$1, $3);
            $$ = $1;
        }
    ;

FunctionReturnType
    : TypeDeclaration
        {
            $$ = $1;
        }
    ;

FunctionParametersDeclaration
    : %empty
        {
            $$ = list_new((void (*)(void**)) decl_free);
        }
    | IdentifierDeclaration
        {
            List* list = list_new((void (*)(void**)) decl_free);
            list_insert_last(&list, $1);
            $$ = list;
        }
    | FunctionParametersDeclaration "," IdentifierDeclaration
        {
            list_insert_last(&$1, $3);
            $$ = $1;
        }
    ;

StructType
    : "struct" "{" StructNamedTypesDeclaration "}"
        {
            Type* type = NEW_STRUCT_TYPE_WITH_FIELDS(
                0,
                $3
            );
            $$ = type;
        }
    ;

StructNamedTypesDeclaration
    : %empty
        {
            $$ = list_new((void (*)(void**)) type_free);
        }
    | NamedType
        {
            List* list = list_new((void (*)(void**)) type_free);
            list_insert_last(&list, $1);
            $$ = list;
        }
    | StructNamedTypesDeclaration NamedType
        {
            list_insert_last(&$1, $2);
            $$ = $1;
        }
    ;

NamedType
    : IDENT ":" TypeDeclaration
        {
            Type* type = NEW_NAMED_TYPE($1, $3);
            safe_free((void**) &$1);
            $$ = type;
        }
    ;

StructFieldsDeclaration
    : NamedType
        {
            List* list = list_new((void (*)(void**)) type_free);
            list_insert_last(&list, $1);
            $$ = list;
        }
    | StructFieldsDeclaration NamedType
        {
            list_insert_last(&$1, $2);
            $$ = $1;
        }
    ;

ArrayType
    : ArrayDimensionList ValidArrayType
        {
            Type* type = NEW_ARRAY_TYPE_WITH_DIMENSION($1, $2);
            $$ = type;
        }
    ;

ValidArrayType
    : IDENT
        {
            Type* type = NEW_CUSTOM_TYPE(0, $1);
            safe_free((void**) &$1);
            $$ = type;
        }
    | AtomicType
        {
            $$ = $1;
        }
    | FunctionType
        {
            $$ = $1;
        }
    | StructType
        {
            $$ = $1;
        }
    ;

ArrayDimensionList
    : ArrayDimension
        {
            List* list = list_new((void (*)(void**)) type_free);
            list_insert_last(&list, $1);
            $$ = list;
        }
    | ArrayDimensionList ArrayDimension
        {
            list_insert_last(&$1, $2);
            $$ = $1;
        }
    ;

ArrayDimension
    : "[" "]"
        {
            $$ = NEW_ARRAY_UNDEFINED_DIMENSION();
        }
    | "[" INT "]"
        {
            $$ = NEW_ARRAY_DIMENSION($2);
        }
    ;

Statement
    : BlockStatement
        {
            $$ = NEW_STMT_DECL($1);
        }
    | ReturnStatement
        {
            $$ = NEW_STMT_DECL($1);
        }
    | BreakStatement
        {
            $$ = NEW_STMT_DECL($1);
        }
    | ContinueStatement
        {
            $$ = NEW_STMT_DECL($1);
        }
    | IfStatement
        {
            $$ = NEW_STMT_DECL($1);
        }
    | WhileStatement
        {
            $$ = NEW_STMT_DECL($1);
        }
    | ForStatement
        {
            $$ = NEW_STMT_DECL($1);
        }
    | ExpressionStatement Semicolon
        {
            $$ = NEW_STMT_DECL($1);
        }
    ;

BlockStatement
    : "{" Declarations "}"
        {
            $$ = NEW_BLOCK_STMT_WITH_DECLS($2);
        }
    ;

ReturnStatement
    : "return" Semicolon
        {
            $$ = NEW_RETURN_STMT(NULL);
        }
    | "return" Expression Semicolon
        {
            $$ = NEW_RETURN_STMT($2);
        }
    ;

BreakStatement
    : "break" Semicolon
        {
            $$ = NEW_BREAK_STMT();
        }
    ;

ContinueStatement
    : "continue" Semicolon
        {
            $$ = NEW_CONTINUE_STMT();
        }
    ;

IfStatement
    : "if" "(" Expression ")" BlockStatement ElseStatement
        {
            $$ = NEW_IF_STMT($3, $5, $6);
        }
    ;

ElseStatement
    : %empty
        {
            $$ = NULL;
        }
    | "else" IfStatement
        {
            $$ = $2;
        }
    | "else" BlockStatement
        {
            $$ = $2;
        }
    ;

WhileStatement
    : "while" "(" Expression ")" BlockStatement
        {
            $$ = NEW_WHILE_STMT($3, $5);
        }
    ;

ForStatement
    : "for" "(" ForInitializer  ";" ForCondition ";" ForIncrement ")" BlockStatement
        {
            $$ = NEW_FOR_STMT($3, $5, $7, $9);
        }
    ;

ForInitializer
    : %empty
        {
            $$ = NULL;
        }
    | LetDeclaration
        {
            $$ = $1;
        }
    | Expression
        {
            $$ = NEW_STMT_DECL(
                NEW_EXPR_STMT($1)
            );
        }
    ;

ForCondition
    : %empty
        {
            $$ = NULL;
        }
    | Expression
        {
            $$ = $1;
        }
    ;

ForIncrement
    : %empty
        {
            $$ = NULL;
        }
    | Expression
        {
            $$ = $1;
        }
    ;

ExpressionStatement
    : Expression
        {
            $$ = NEW_EXPR_STMT($1);
        }
    ;

Expression
    : AssignmentExpression
        {
            $$ = $1;
        }
    ;

AssignmentExpression
    : ConditionalExpression
        {
            $$ = $1;
        }
    | UnaryExpression AssignmentOperator AssignmentExpression
        {
            $$ = NEW_ASSIGN_EXPR($1, $2, $3);
        }
    ;

AssignmentOperator
    : "="
        {
            $$ = NEW_TOKEN(TOKEN_ASSIGN, "=", yylineno);
        }
    | "+="
        {
            $$ = NEW_TOKEN(TOKEN_ADD_ASSIGN, "+=", yylineno);
        }
    | "-="
        {
            $$ = NEW_TOKEN(TOKEN_SUB_ASSIGN, "-=", yylineno);
        }
    | "*="
        {
            $$ = NEW_TOKEN(TOKEN_MUL_ASSIGN, "*=", yylineno);
        }
    | "/="
        {
            $$ = NEW_TOKEN(TOKEN_QUO_ASSIGN, "/=", yylineno);
        }
    | "%="
        {
            $$ = NEW_TOKEN(TOKEN_REM_ASSIGN, "%=", yylineno);
        }
    | "&="
        {
            $$ = NEW_TOKEN(TOKEN_AND_ASSIGN, "&=", yylineno);
        }
    | "|="
        {
            $$ = NEW_TOKEN(TOKEN_OR_ASSIGN, "|=", yylineno);
        }
    | "^="
        {
            $$ = NEW_TOKEN(TOKEN_XOR_ASSIGN, "^=", yylineno);
        }
    | "<<="
        {
            $$ = NEW_TOKEN(TOKEN_SHL_ASSIGN, "<<=", yylineno);
        }
    | ">>="
        {
            $$ = NEW_TOKEN(TOKEN_SHR_ASSIGN, ">>=", yylineno);
        }
    ;

ConditionalExpression
    : LogicalOrExpression
        {
            $$ = $1;
        }
    | LogicalOrExpression "?" Expression ":" ConditionalExpression
        {
            $$ = NEW_CONDITIONAL_EXPR($1, $3, $5);
        }
    ;

LogicalOrExpression
    : LogicalAndExpression
        {
            $$ = $1;
        }
    | LogicalOrExpression "||" LogicalAndExpression
        {
            $$ = NEW_LOGICAL_EXPR(
                $1,
                NEW_TOKEN(TOKEN_LOR, "||", yylineno),
                $3
            );
        }
    ;

LogicalAndExpression
    : OrExpression
        {
            $$ = $1;
        }
    | LogicalAndExpression "&&" OrExpression
        {
            $$ = NEW_LOGICAL_EXPR(
                $1,
                NEW_TOKEN(TOKEN_LAND, "&&", yylineno),
                $3
            );
        }
    ;

OrExpression
    : XorExpression
        {
            $$ = $1;
        }
    | OrExpression "|" XorExpression
        {
            $$ = NEW_BINARY_EXPR(
                $1,
                NEW_TOKEN(TOKEN_OR, "|", yylineno),
                $3
            );
        }
    ;

XorExpression
    : AndExpression
        {
            $$ = $1;
        }
    | AndExpression "^" AndExpression
        {
            $$ = NEW_BINARY_EXPR(
                $1,
                NEW_TOKEN(TOKEN_XOR, "^", yylineno),
                $3
            );
        }
    ;

AndExpression
    : EqualityExpression
        {
            $$ = $1;
        }
    | AndExpression "&" EqualityExpression
        {
            $$ = NEW_BINARY_EXPR(
                $1,
                NEW_TOKEN(TOKEN_AND, "&", yylineno),
                $3
            );
        }
    ;

EqualityExpression
    : RelationalExpression
        {
            $$ = $1;
        }
    | EqualityExpression EqualityOperator RelationalExpression
        {
            $$ = NEW_BINARY_EXPR($1, $2, $3);
        }
    ;

EqualityOperator
    : "=="
        {
            $$ = NEW_TOKEN(TOKEN_EQL, "==", yylineno);
        }
    | "!="
        {
            $$ = NEW_TOKEN(TOKEN_NEQ, "!=", yylineno);
        }
    ;

RelationalExpression
    : ShiftExpression
        {
            $$ = $1;
        }
    | RelationalExpression RelationalOperator ShiftExpression
        {
            $$ = NEW_BINARY_EXPR($1, $2, $3);
        }
    ;

RelationalOperator
    : "<"
        {
            $$ = NEW_TOKEN(TOKEN_LSS, "<", yylineno);
        }
    | ">"
        {
            $$ = NEW_TOKEN(TOKEN_GTR, ">", yylineno);
        }
    | "<="
        {
            $$ = NEW_TOKEN(TOKEN_LEQ, "<=", yylineno);
        }
    | ">="
        {
            $$ = NEW_TOKEN(TOKEN_GEQ, ">=", yylineno);
        }
    ;

ShiftExpression
    : AdditiveExpression
        {
            $$ = $1;
        }
    | ShiftExpression ShiftOperator AdditiveExpression
        {
            $$ = NEW_BINARY_EXPR($1, $2, $3);
        }
    ;

ShiftOperator
    : "<<"
        {
            $$ = NEW_TOKEN(TOKEN_SHL, "<<", yylineno);
        }
    | ">>"
        {
            $$ = NEW_TOKEN(TOKEN_SHR, ">>", yylineno);
        }
    ;

AdditiveExpression
    : MultiplicativeExpression
        {
            $$ = $1;
        }
    | AdditiveExpression AdditiveOperator MultiplicativeExpression
        {
            $$ = NEW_BINARY_EXPR($1, $2, $3);
        }
    ;

AdditiveOperator
    : "+"
        {
            $$ = NEW_TOKEN(TOKEN_ADD, "+", yylineno);
        }
    | "-"
        {
            $$ = NEW_TOKEN(TOKEN_SUB, "-", yylineno);
        }
    ;

MultiplicativeExpression
    : UnaryExpression
        {
            $$ = $1;
        }
    | MultiplicativeExpression MultiplicativeOperator UnaryExpression
        {
            $$ = NEW_BINARY_EXPR($1, $2, $3);
        }
    ;

MultiplicativeOperator
    : "*"
        {
            $$ = NEW_TOKEN(TOKEN_MUL, "*", yylineno);
        }
    | "/"
        {
            $$ = NEW_TOKEN(TOKEN_QUO, "/", yylineno);
        }
    | "%"
        {
            $$ = NEW_TOKEN(TOKEN_REM, "%", yylineno);
        }
    ;

UnaryExpression
    : PostfixExpression
        {
            $$ = $1;
        }
    | UnaryOperator %prec UNARY UnaryExpression
        {
            $$ = NEW_UNARY_EXPR($1, $2);
        }
    ;

UnaryOperator
    : "+"
        {
            $$ = NEW_TOKEN(TOKEN_ADD, "+", yylineno);
        }
    | "-"
        {
            $$ = NEW_TOKEN(TOKEN_SUB, "-", yylineno);
        }
    | "~"
        {
            $$ = NEW_TOKEN(TOKEN_TILDE, "~", yylineno);
        }
    | "!"
        {
            $$ = NEW_TOKEN(TOKEN_NOT, "!", yylineno);
        }
    ;

PostfixExpression
    : PrimaryExpression
        {
            $$ = $1;
        }
    | CastExpression
        {
            $$ = $1;
        }
    | ArrayMemberExpression
        {
            $$ = $1;
        }
    | CallExpression
        {
            $$ = $1;
        }
    | MemberExpression
        {
            $$ = $1;
        }
    | PostfixExpression PostfixOperator
        {
            $$ = NEW_UPDATE_EXPR($1, $2);
        }
    ;

PostfixOperator
    : "++"
        {
            $$ = NEW_TOKEN(TOKEN_INC, "++", yylineno);
        }
    | "--"
        {
            $$ = NEW_TOKEN(TOKEN_DEC, "--", yylineno);
        }
    ;

ArrayMemberExpression
    : PostfixExpression ArrayIndexAccessList
        {
            $$ = NEW_ARRAY_MEMBER_EXPR_WITH_ACCESS_LEVEL_LIST($1, $2);
        }
    ;

ArrayIndexAccessList
    : "[" Expression "]"
        {
            List* list = list_new((void (*)(void**)) expr_free);
            list_insert_last(&list, $2);
            $$ = list;
        }
    | ArrayIndexAccessList "[" Expression "]"
        {
            list_insert_last(&$1, $3);
            $$ = $1;
        }
    ;

CallExpression
    : PostfixExpression "(" CallExpressionArguments ")"
        {
            $$ = NEW_CALL_EXPR_WITH_ARGS($1, $3);
        }
    ;

CallExpressionArguments
    : %empty
        {
            $$ = list_new((void (*)(void**)) expr_free);
        }
    | Expression
        {
            List* list = list_new((void (*)(void**)) expr_free);
            list_insert_last(&list, $1);
            $$ = list;
        }
    | CallExpressionArguments "," Expression
        {
            list_insert_last(&$1, $3);
            $$ = $1;
        }
    ;

MemberExpression
    : PostfixExpression "." MemberAccessList
        {
            $$ = NEW_MEMBER_EXPR_WITH_MEMBER_LIST($1, $3);
        }
    ;

MemberAccessList
    : %empty
        {
            $$ = list_new((void (*)(void**)) expr_free);
        }
    | Identifier
        {
            List* list = list_new((void (*)(void**)) expr_free);
            list_insert_last(&list, $1);
            $$ = list;
        }
    | MemberAccessList "." Identifier
        {
            list_insert_last(&$1, $3);
            $$ = $1;
        }
    ;

CastExpression
    : PostfixExpression "." "(" TypeDeclaration ")"
        {
            $$ = NEW_CAST_EXPR($1, $4);
        }
    ;

PrimaryExpression
    : GroupExpression
        {
            $$ = $1;
        }
    | Identifier
        {
            $$ = $1;
        }
    | Literal
        {
            $$ = $1;
        }
    | FunctionExpression
        {
            $$ = $1;
        }
    | StructInitializationExpression
        {
            $$ = $1;
        }
    | ArrayInitializationExpression
        {
            $$ = $1;
        }
    ;

GroupExpression
    : "(" Expression ")"
        {
            $$ = NEW_GROUP_EXPR($2);
        }
    ;

Identifier
    : IDENT
        {
            Expr* expr = NEW_IDENT_LITERAL($1);
            safe_free((void**) &$1);
            $$ = expr;
        }
    ;

Literal
    : INT
        {
            $$ = NEW_INT_LITERAL($1);
        }
    | FLOAT
        {
            $$ = NEW_FLOAT_LITERAL($1);
        }
    | CHAR
        {
            $$ = NEW_CHAR_LITERAL($1);
        }
    | STRING
        {
            Expr* expr = NEW_STRING_LITERAL($1);
            safe_free((void**) &$1);
            $$ = expr;
        }
    | TRUE
        {
            $$ = NEW_BOOL_LITERAL(true);
        }
    | FALSE
        {
            $$ = NEW_BOOL_LITERAL(false);
        }
    | NIL
        {
            $$ = NEW_NIL_LITERAL();
        }
    ;

FunctionExpression
    : "func" "(" FunctionParametersDeclaration ")" FunctionBody
        {
            $$ = NEW_FUNCTION_EXPR_WITH_PARAMS($3, $5);
        }
    | "func" "(" FunctionParametersDeclaration ")" ":" FunctionReturnType FunctionBody
        {
            $$ = NEW_FUNCTION_EXPR_WITH_PARAMS_AND_RETURN($3, $6, $7);
        }
    ;

StructInitializationExpression
    : IDENT StructInitializationListExpression
        {
            Expr* expr = NEW_STRUCT_INIT_EXPR_WITH_FIELDS(
                NEW_TOKEN(TOKEN_STRING, $1, yylineno),
                $2
            );
            safe_free((void**) &$1);
            $$ = expr;
        }
    | StructType StructInitializationListExpression
        {
            $$ = NEW_STRUCT_INLINE_EXPR_WITH_FIELDS($1, $2);
        }
    ;

StructInitializationListExpression
    : "{" StructArguments "}"
        {
            $$ = $2;
        }
    ;

StructArguments
    : %empty
        {
            $$ = list_new((void (*)(void**)) expr_free);
        }
    | StructArgumentsExpession
        {
            List* list = list_new((void (*)(void**)) expr_free);
            list_insert_last(&list, $1);
            $$ = list;
        }
    | StructArguments "," StructArgumentsExpession
        {
            list_insert_last(&$1, $3);
            $$ = $1;
        }
    ;

StructArgumentsExpession
    : IDENT ":" Expression
        {
            Expr* expr = NEW_FIELD_EXPR(
                NEW_TOKEN(TOKEN_STRING, $1, yylineno),
                $3
            );

            safe_free((void**) &$1);

            $$ = expr;
        }
    ;

ArrayInitializationExpression
    : ArrayType "{" ArrayArguments "}"
        {
            $$ = NEW_ARRAY_INIT_EXPR_WITH_ELEMENTS($1, $3);
        }
    ;

ArrayArguments
    : %empty
        {
            $$ = list_new((void (*)(void**)) expr_free);
        }
    | Expression
        {
            List* list = list_new((void (*)(void**)) expr_free);
            list_insert_last(&list, $1);
            $$ = list;
        }
    | ArrayArguments "," Expression
        {
            list_insert_last(&$1, $3);
            $$ = $1;
        }
    ;

%%

void yyerror(const char* message) {
    fprintf(stderr, "[Line: %d]: %s\n", yylineno, message);
    success = false;
}
