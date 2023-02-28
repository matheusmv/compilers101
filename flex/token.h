#pragma once

#include <cstddef>
#include <iostream>
#include <string>

enum class TokenType {
    TT_ILLEGAL = -1,
    TT_EOF,
    TT_COMMENT,

    literal_start,
    TT_IDENT,  // main
    TT_INT,    // 1500
    TT_FLOAT,  // 15.35
    TT_CHAR,   // 'A'
    TT_STRING, // "abc"
    literal_end,

    operator_start,
    TT_ADD, // +
    TT_SUB, // -
    TT_MUL, // *
    TT_QUO, // /
    TT_REM, // %

    TT_TILDE, // ~
    TT_AND,   // &
    TT_OR,    // |
    TT_XOR,   // ^
    TT_SHL,   // <<
    TT_SHR,   // >>

    TT_ADD_ASSIGN, // +=
    TT_SUB_ASSIGN, // -=
    TT_MUL_ASSIGN, // *=
    TT_QUO_ASSIGN, // /=
    TT_REM_ASSIGN, // %=

    TT_AND_ASSIGN, // &=
    TT_OR_ASSIGN,  // |=
    TT_XOR_ASSIGN, // ^=
    TT_SHL_ASSIGN, // <<=
    TT_SHR_ASSIGN, // >>=

    TT_LAND, // &&
    TT_LOR,  // ||
    TT_INC,  // ++
    TT_DEC,  // --

    TT_EQL,    // ==
    TT_LSS,    // <
    TT_GTR,    // >
    TT_ASSIGN, // =
    TT_NOT,    // !

    TT_NEQ,      // !=
    TT_LEQ,      // <=
    TT_GEQ,      // >=
    TT_DEFINE,   // :=
    TT_ELLIPSIS, // ...

    TT_LPAREN, // (
    TT_LBRACK, // [
    TT_LBRACE, // {
    TT_COMMA,  // ,
    TT_PERIOD, // .

    TT_RPAREN,    // )
    TT_RBRACK,    // ]
    TT_RBRACE,    // }
    TT_SEMICOLON, // ;
    TT_COLON,     // :
    operator_end,

    keyword_start,
    FUNCTION, // func
    LET,      // let
    TRUE,     // true
    FALSE,    // false
    IF,       // if
    ELSE,     // else
    RETURN,   // return
    BREAK,    // break
    CONTINUE, // continue
    WHILE,    // while
    FOR,      // for
    NIL,      // nil
    keyword_end
};

struct Token {
    TokenType type;
    std::string literal;
    std::size_t line;
};

std::ostream& operator<<(std::ostream& out, const Token& token);

const std::string& to_string(const TokenType& token_type);
TokenType lookup(const std::string& literal);
bool is_literal(const TokenType& token_type);
bool is_operator(const TokenType& token_type);
bool is_keyword(const TokenType& token_type);
bool is_keyword(const std::string& literal);
bool is_identifier(const std::string& literal);
