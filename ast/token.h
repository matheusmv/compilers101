#pragma once

#include <stddef.h>

#include "utils.h"


typedef enum TokenType {
    TOKEN_ILLEGAL = -1,
    TOKEN_EOF,
    TOKEN_COMMENT,

    TOKEN_IDENT,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_CHAR,
    TOKEN_STRING,

    TOKEN_ADD, // +
    TOKEN_SUB, // -
    TOKEN_MUL, // *
    TOKEN_QUO, // /
    TOKEN_REM, // %
} TokenType;

typedef struct Token {
    TokenType type;
    char* literal;
    size_t line;
} Token;

Token* token_new(TokenType, const char*, size_t);
void token_to_string(Token**);
void token_free(Token**);

#define NEW_TOKEN(type, literal, line) token_new((type), (literal), (line))
