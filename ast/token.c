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

    printf("%s", (*token)->literal);
}

void token_free(Token** token) {
    if (token == NULL || *token == NULL)
        return;

    free((*token)->literal);
    free(*token);
    *token = NULL;
}

bool token_is_literal(Token** token) {
    if (token == NULL || *token == NULL)
        return false;

    return literal_start < (*token)->type && (*token)->type < literal_end;
}

bool token_is_operator(Token** token) {
    if (token == NULL || *token == NULL)
        return false;

    return operator_start < (*token)->type && (*token)->type < operator_end;
}

bool token_is_keyword(Token** token) {
    if (token == NULL || *token == NULL)
        return false;

    return keyword_start < (*token)->type && (*token)->type < keyword_end;
}
