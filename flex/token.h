#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>
#include <stdbool.h>

#define LOWEST_PRECEDENCE  0;
#define UNARY_PRECEDENCE   6;
#define HIGHEST_PRECEDENCE 7;

typedef enum token_t {
        T_ILLEGAL,
        T_EOF,
        T_COMMENT,

        literal_start,
        T_IDENT,  // main
        T_INT,    // 1500
        T_FLOAT,  // 15.35
        T_CHAR,   // 'a'
        T_STRING, // 'abc'
        T_BOOL,   // true
        literal_end,

        operator_start,
        T_ADD, // +
	T_SUB, // -
	T_MUL, // *
	T_QUO, // /
	T_REM, // %

	T_AND, // &
	T_OR,  // |
	T_XOR, // ^
	T_SHL, // <<
	T_SHR, // >>

	T_ADD_ASSIGN, // +=
	T_SUB_ASSIGN, // -=
	T_MUL_ASSIGN, // *=
	T_QUO_ASSIGN, // /=
	T_REM_ASSIGN, // %=

	T_AND_ASSIGN, // &=
	T_OR_ASSIGN,  // |=
	T_XOR_ASSIGN, // ^=
	T_SHL_ASSIGN, // <<=
	T_SHR_ASSIGN, // >>=

	T_LAND,  // &&
	T_LOR,   // ||
	T_INC,   // ++
	T_DEC,   // --

	T_EQL,    // ==
	T_LSS,    // <
	T_GTR,    // >
	T_ASSIGN, // =
	T_NOT,    // !

	T_NEQ,    // !=
	T_LEQ,    // <=
	T_GEQ,    // >=
	T_DEFINE, // :=

	T_LPAREN, // (
	T_LBRACK, // [
	T_LBRACE, // {
	T_COMMA,  // ,
	T_PERIOD, // .

	T_RPAREN,    // )
	T_RBRACK,    // ]
	T_RBRACE,    // }
	T_SEMICOLON, // ;
	T_COLON,     // :
        operator_end,

        keyword_start,

        // TODO

        keyword_end
} token_t;

const char* token_str(token_t token);

int32_t token_precedence(token_t token);

#endif /* TOKEN_H */
