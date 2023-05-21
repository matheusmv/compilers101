#include "token_test.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../../src/token.h"


static void test_token_new(void) {
    Token* tok = token_new(TOKEN_ADD, "+", 1);

    assert(tok != NULL);
    assert(tok->type == TOKEN_ADD);
    assert(strcmp(tok->literal, "+") == 0);
    assert(tok->line == 1);

    token_free(&tok);
    assert(tok == NULL);
}

static void test_token_is_literal(void) {
    Token* int_tok = token_new(TOKEN_INT, "42", 1);
    Token* float_tok = token_new(TOKEN_FLOAT, "3.14", 2);
    Token* char_tok = token_new(TOKEN_CHAR, "'a'", 3);
    Token* string_tok = token_new(TOKEN_STRING, "\"hello\"", 4);
    Token* ident_tok = token_new(TOKEN_IDENT, "foo", 5);
    Token* func_tok = token_new(TOKEN_FUNC, "func", 5);

    assert(token_is_literal(&int_tok) == true);
    assert(token_is_literal(&float_tok) == true);
    assert(token_is_literal(&char_tok) == true);
    assert(token_is_literal(&string_tok) == true);
    assert(token_is_literal(&ident_tok) == true);
    assert(token_is_literal(&func_tok) == false);

    token_free(&int_tok);
    token_free(&float_tok);
    token_free(&char_tok);
    token_free(&string_tok);
    token_free(&ident_tok);
    token_free(&func_tok);
}

static void test_token_is_operator(void) {
    Token* add_tok = token_new(TOKEN_ADD, "+", 1);
    Token* sub_tok = token_new(TOKEN_SUB, "-", 2);
    Token* mul_tok = token_new(TOKEN_MUL, "*", 3);
    Token* quo_tok = token_new(TOKEN_QUO, "/", 4);
    Token* rem_tok = token_new(TOKEN_REM, "%", 5);

    assert(token_is_operator(&add_tok) == true);
    assert(token_is_operator(&sub_tok) == true);
    assert(token_is_operator(&mul_tok) == true);
    assert(token_is_operator(&quo_tok) == true);
    assert(token_is_operator(&rem_tok) == true);

    Token* ident_tok2 = token_new(TOKEN_IDENT, "foo", 6);

    assert(token_is_operator(&ident_tok2) == false);

    token_free(&add_tok);
    token_free(&sub_tok);
    token_free(&mul_tok);
    token_free(&quo_tok);
    token_free(&rem_tok);
    token_free(&ident_tok2);
}

static void test_token_is_keyword(void) {
    Token* if_tok = token_new(TOKEN_IF, "if", 1);
    Token* else_tok = token_new(TOKEN_ELSE, "else", 2);
    Token* let_tok = token_new(TOKEN_LET, "let", 3);
    Token* func_tok = token_new(TOKEN_FUNC, "func", 4);
    Token* return_tok = token_new(TOKEN_RETURN, "return", 5);

    assert(token_is_keyword(&if_tok) == true);
    assert(token_is_keyword(&else_tok) == true);
    assert(token_is_keyword(&let_tok) == true);
    assert(token_is_keyword(&func_tok) == true);
    assert(token_is_keyword(&return_tok) == true);

    Token* ident_tok3 = token_new(TOKEN_IDENT, "foo", 6);

    assert(token_is_keyword(&ident_tok3) == false);

    token_free(&if_tok);
    token_free(&else_tok);
    token_free(&let_tok);
    token_free(&func_tok);
    token_free(&return_tok);
    token_free(&ident_tok3);
}

void run_token_tests(void) {
    test_token_new();
    test_token_is_literal();
    test_token_is_operator();
    test_token_is_keyword();

    printf("%s: All tests passed successfully!\n", __FILE__);
}
