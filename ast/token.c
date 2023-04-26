#include "token.h"

#include <stdio.h>
#include <stdlib.h>


Token* token_new(TokenType type, const char* literal, size_t line) {
    Token* tok = NULL;
    tok = malloc(sizeof(Token));
    if (tok == NULL) {
        return NULL;
    }

    *tok = (Token) {
        .type = type,
        .literal = str_dup(literal),
        .line = line
    };

    return tok;
}

void token_to_string(Token** token) {
    if (token == NULL || *token == NULL)
        return;

    printf(" %s ", (*token)->literal);
}

void token_free(Token** token) {
    if (token == NULL || *token == NULL)
        return;

    free((*token)->literal);
    free(*token);
    *token = NULL;
}
