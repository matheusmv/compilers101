#include "ast.h"


int main(void) {
    Expr* lExpr = NEW_LITERAL_EXPR(NEW_FLOAT(1.1));
    Expr* rExpr = NEW_LITERAL_EXPR(NEW_FLOAT(2.3));
    Token* addToken = NEW_TOKEN(TOKEN_ADD, "+", 1);

    Expr* sum = NEW_BINARY_EXPR(lExpr, addToken, rExpr);

    Expr* lExprGroupSum = NEW_GROUP_EXPR(sum);
    Expr* rExprLiteral = NEW_LITERAL_EXPR(NEW_FLOAT(14.75));
    Token* mulToken = NEW_TOKEN(TOKEN_MUL, "*", 1);

    Expr* mul = NEW_BINARY_EXPR(lExprGroupSum, mulToken, rExprLiteral);

    PRINT_AND_FREE(mul);

    Expr* testChar = NEW_LITERAL_EXPR(NEW_CHAR('F'));
    PRINT_AND_FREE(testChar);

    Expr* testIdent = NEW_LITERAL_EXPR(NEW_IDENT("$HOME"));
    PRINT_AND_FREE(testIdent);

    Expr* testString = NEW_LITERAL_EXPR(NEW_STRING("Alone after all."));
    PRINT_AND_FREE(testString);

    Expr* testBool = NEW_LITERAL_EXPR(NEW_BOOL((2 + 2) == 4));
    PRINT_AND_FREE(testBool);

    return EXIT_SUCCESS;
}
