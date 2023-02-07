#include "token.h"

#include <string.h>

#define LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

static const char* tokens[] = {
        [T_ILLEGAL] = "ILLEGAL",

        [T_EOF] = "EOF",
        [T_COMMENT] = "COMMENT",

        [T_IDENT] = "IDENT",
        [T_INT] = "INT",
        [T_FLOAT] = "FLOAT",
        [T_CHAR] = "CHAR",
        [T_STRING] = "STRING",
        [T_BOOL] = "BOOL",

        [T_ADD] = "+",
        [T_SUB] = "-",
        [T_MUL] = "*",
        [T_QUO] = "/",
        [T_REM] = "%",

        [T_AND] = "&",
        [T_OR] = "|",
        [T_XOR] = "^",
        [T_SHL] = "<<",
        [T_SHR] = ">>",
        
        [T_ADD_ASSIGN] = "+=",
        [T_SUB_ASSIGN] = "-=",
        [T_MUL_ASSIGN] = "*=",
        [T_QUO_ASSIGN] = "/=",
        [T_REM_ASSIGN] = "%=",

        [T_AND_ASSIGN] = "&=",
        [T_OR_ASSIGN] = "|=",
        [T_XOR_ASSIGN] = "^=",
        [T_SHL_ASSIGN] = "<<=",
        [T_SHR_ASSIGN] = ">>=",

        [T_LAND] = "&&",
        [T_LOR] = "||",
        [T_INC] = "++",
        [T_DEC] = "--",

        [T_EQL] = "==",
        [T_LSS] = "<",
        [T_GTR] = ">",
        [T_ASSIGN] = "=",
        [T_NOT] = "!",

        [T_NEQ] = "!=",
        [T_LEQ] = "<=",
        [T_GEQ] = ">=",
        [T_DEFINE] = ":=",

        [T_LPAREN] = "(",
        [T_LBRACK] = "[",
        [T_LBRACE] = "{",
        [T_COMMA] = ",",
        [T_PERIOD] = ".",

        [T_RPAREN] = ")",
        [T_RBRACK] = "]",
        [T_RBRACE] = "}",
        [T_SEMICOLON] = ";",
        [T_COLON] = ":",
};

const char* token_str(token_t token) {
        const char* str = "";

        if (0 <= token && token < LEN(tokens)) {
                str = tokens[token];
        }

        if (strcmp(str, "") == 0) {
                str = "invalid token";
        }

        return str;
}

int32_t token_precedence(token_t token) {
        switch (token) {
        case T_LOR:
                return 1;
        case T_LAND:
                return 2;
        case T_EQL:
        case T_NEQ:
        case T_LSS:
        case T_LEQ:
        case T_GTR:
        case T_GEQ:
                return 3;
        case T_ADD:
        case T_SUB:
        case T_OR:
        case T_XOR:
                return 4;
        case T_MUL:
        case T_QUO:
        case T_REM:
        case T_SHL:
        case T_AND:
                return 5;
        }

        return LOWEST_PRECEDENCE;
}
