#include "ast.h"
#include "token.h"


int main(void) {
    Expr* testMath = NEW_BINARY_EXPR(
        NEW_GROUP_EXPR(
            NEW_BINARY_EXPR(
                NEW_LITERAL_EXPR(NEW_FLOAT(1.1)),
                NEW_TOKEN(TOKEN_ADD, "+", 1),
                NEW_LITERAL_EXPR(NEW_FLOAT(2.3))
            )
        ),
        NEW_TOKEN(TOKEN_MUL, "*", 1),
        NEW_LITERAL_EXPR(NEW_FLOAT(14.75))
    );
    PRINT_AND_FREE(testMath);

    Expr* testChar = NEW_LITERAL_EXPR(NEW_CHAR('F'));
    PRINT_AND_FREE(testChar);

    Expr* testIdent = NEW_LITERAL_EXPR(NEW_IDENT("$HOME"));
    PRINT_AND_FREE(testIdent);

    Expr* testString = NEW_LITERAL_EXPR(NEW_STRING("Alone after all."));
    PRINT_AND_FREE(testString);

    Expr* testBool = NEW_LITERAL_EXPR(NEW_BOOL((2 + 2) == 4));
    PRINT_AND_FREE(testBool);

    Expr* testAssign = NEW_ASSIGN_EXPR(
        NEW_TOKEN(TOKEN_IDENT, "result", 1),
        NEW_BINARY_EXPR(
            NEW_GROUP_EXPR(
                NEW_BINARY_EXPR(
                    NEW_LITERAL_EXPR(NEW_IDENT("n")),
                    NEW_TOKEN(TOKEN_MUL, "*", 1),
                    NEW_GROUP_EXPR(
                        NEW_BINARY_EXPR(
                            NEW_LITERAL_EXPR(NEW_IDENT("n")),
                            NEW_TOKEN(TOKEN_SUB, "+", 1),
                            NEW_LITERAL_EXPR(NEW_INT(1))
                        )
                    )
                )
            ),
            NEW_TOKEN(TOKEN_QUO, "/", 1),
            NEW_LITERAL_EXPR(NEW_INT(2))
        )
    );
    PRINT_AND_FREE(testAssign);

    Expr* testCall = NEW_CALL_EXPR(NEW_LITERAL_EXPR(NEW_IDENT("lerp")));
    CALL_EXPR_ADD_ARG(testCall, NEW_LITERAL_EXPR(NEW_FLOAT(0.15)));
    CALL_EXPR_ADD_ARG(testCall, NEW_LITERAL_EXPR(NEW_FLOAT(3.14)));
    CALL_EXPR_ADD_ARG(testCall, NEW_LITERAL_EXPR(NEW_FLOAT(0.3)));
    PRINT_AND_FREE(testCall);

    Expr* testLogicalAnd = NEW_LOGICAL_EXPR(
        NEW_CALL_EXPR(NEW_LITERAL_EXPR(NEW_IDENT("initialized"))),
        NEW_TOKEN(TOKEN_LAND, "&&", 1),
        NEW_CALL_EXPR(NEW_LITERAL_EXPR(NEW_IDENT("empty")))
    );
    PRINT_AND_FREE(testLogicalAnd);

    Expr* testUnary = NEW_UNARY_EXPR(
        NEW_TOKEN(TOKEN_NOT, "!", 1),
        NEW_LITERAL_EXPR(NEW_BOOL(true))
    );
    PRINT_AND_FREE(testUnary);

    return EXIT_SUCCESS;
}
