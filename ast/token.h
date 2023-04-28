#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "utils.h"


typedef enum TokenType {
    TOKEN_ILLEGAL = -1,
    TOKEN_EOF,
    TOKEN_COMMENT,

    literal_start,
    TOKEN_IDENT,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_CHAR,
    TOKEN_STRING,
    literal_end,

    operator_start,
    TOKEN_ADD, // +
    TOKEN_SUB, // -
    TOKEN_MUL, // *
    TOKEN_QUO, // /
    TOKEN_REM, // %

    TOKEN_TILDE, // "~"
    TOKEN_AND,   // "&"
    TOKEN_OR,    // "|"
    TOKEN_XOR,   // "^"
    TOKEN_SHL,   // "<<"
    TOKEN_SHR,   // ">>"

    TOKEN_ADD_ASSIGN, // "+="
    TOKEN_SUB_ASSIGN, // "-="
    TOKEN_MUL_ASSIGN, // "*="
    TOKEN_QUO_ASSIGN, // "/="
    TOKEN_REM_ASSIGN, // "%="

    TOKEN_AND_ASSIGN, // "&="
    TOKEN_OR_ASSIGN,  // "|="
    TOKEN_XOR_ASSIGN, // "^="
    TOKEN_SHL_ASSIGN, // "<<="
    TOKEN_SHR_ASSIGN, // ">>="

    TOKEN_LOR,  // ||
    TOKEN_LAND, // &&

    TOKEN_INC, // "++"
    TOKEN_DEC, // "--"

    TOKEN_EQL,    // "=="
    TOKEN_LSS,    // "<"
    TOKEN_GTR,    // ">"
    TOKEN_ASSIGN, // "="
    TOKEN_NOT,    // !

    TOKEN_NEQ,      // "!="
    TOKEN_LEQ,      // "<="
    TOKEN_GEQ,      // ">="
    TOKEN_ELLIPSIS, // "..."

    TOKEN_LPAREN, // "("
    TOKEN_LBRACK, // "["
    TOKEN_LBRACE, // "{"
    TOKEN_COMMA , // ","
    TOKEN_PERIOD, // "."

    TOKEN_RPAREN,    // ")"
    TOKEN_RBRACK,    // "]"
    TOKEN_RBRACE,    // "}"
    TOKEN_SEMICOLON, // ";"
    TOKEN_COLON,     // ":"
    operator_end,

    keyword_start,
    TOKEN_LET,      // "let"
    TOKEN_CONST,    // "const"
    TOKEN_IF,       // "if"
    TOKEN_ELSE,     // "else"
    TOKEN_FUNC,     // "func"
    TOKEN_RETURN,   // "return"
    TOKEN_WHILE,    // "while"
    TOKEN_FOR,      // "for"
    TOKEN_BREAK,    // "break"
    TOKEN_CONTINUE, // "continue"
    TOKEN_NIL,      // "nil"
    TOKEN_TRUE,     // "true"
    TOKEN_FALSE,    // "false"
    TOKEN_STRUCT,   // "struct"

    TOKEN_TYPE_INT,    // "int"
    TOKEN_TYPE_FLOAT,  // "float"
    TOKEN_TYPE_CHAR,   // "char"
    TOKEN_TYPE_STRING, // "string"
    TOKEN_TYPE_BOOL,   // "bool"
    TOKEN_TYPE_VOID,   // "void"
    keyword_end
} TokenType;


typedef struct Token {
    TokenType type;
    char* literal;
    size_t line;
} Token;

Token* token_new(TokenType type, const char* literal, size_t line);
void token_to_string(Token** token);
void token_free(Token** token);

bool token_is_literal(Token** token);
bool token_is_operator(Token** token);
bool token_is_keyword(Token** token);


#define NEW_TOKEN(type, literal, line) token_new((type), (literal), (line))
