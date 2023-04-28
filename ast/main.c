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

    Expr* testChar = NEW_LITERAL_EXPR(NEW_CHAR('F'));

    Expr* testIdent = NEW_LITERAL_EXPR(NEW_IDENT("$HOME"));

    Expr* testString = NEW_LITERAL_EXPR(NEW_STRING("Alone after all."));

    Expr* testBool = NEW_LITERAL_EXPR(NEW_BOOL((2 + 2) == 4));

    Expr* testAssign = NEW_ASSIGN_EXPR(
        NEW_LITERAL_EXPR(NEW_IDENT("result")),
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

    Expr* testCall = NEW_CALL_EXPR(NEW_LITERAL_EXPR(NEW_IDENT("lerp")));
    CALL_EXPR_ADD_ARG(testCall, NEW_LITERAL_EXPR(NEW_FLOAT(0.15)));
    CALL_EXPR_ADD_ARG(testCall, NEW_LITERAL_EXPR(NEW_FLOAT(3.14)));
    CALL_EXPR_ADD_ARG(testCall, NEW_LITERAL_EXPR(NEW_FLOAT(0.3)));

    Expr* testLogicalAnd = NEW_LOGICAL_EXPR(
        NEW_CALL_EXPR(NEW_LITERAL_EXPR(NEW_IDENT("initialized"))),
        NEW_TOKEN(TOKEN_LAND, "&&", 1),
        NEW_CALL_EXPR(NEW_LITERAL_EXPR(NEW_IDENT("empty")))
    );

    Expr* testUnary = NEW_UNARY_EXPR(
        NEW_TOKEN(TOKEN_NOT, "!", 1),
        NEW_LITERAL_EXPR(NEW_BOOL(true))
    );

    Stmt* testExprStmt = NEW_EXPR_STMT(NEW_LITERAL_EXPR(NEW_STRING("OK")));

    Stmt* testInnerBlockStmt = NEW_BLOCK_STMT();

    BLOCK_STMT_ADD_STMT(testInnerBlockStmt, NEW_EXPR_STMT(testMath));
    BLOCK_STMT_ADD_STMT(testInnerBlockStmt, NEW_EXPR_STMT(testChar));
    BLOCK_STMT_ADD_STMT(testInnerBlockStmt, NEW_EXPR_STMT(testIdent));
    BLOCK_STMT_ADD_STMT(testInnerBlockStmt, NEW_EXPR_STMT(testString));

    Stmt* testBlockStmt = NEW_BLOCK_STMT();
    BLOCK_STMT_ADD_STMT(testBlockStmt, testInnerBlockStmt);
    BLOCK_STMT_ADD_STMT(testBlockStmt, NEW_EXPR_STMT(testBool));
    BLOCK_STMT_ADD_STMT(testBlockStmt, NEW_EXPR_STMT(testAssign));
    BLOCK_STMT_ADD_STMT(testBlockStmt, NEW_EXPR_STMT(testCall));
    BLOCK_STMT_ADD_STMT(testBlockStmt, NEW_EXPR_STMT(testLogicalAnd));
    BLOCK_STMT_ADD_STMT(testBlockStmt, NEW_EXPR_STMT(testUnary));
    BLOCK_STMT_ADD_STMT(testBlockStmt, testExprStmt);
    STMT_PRINT_AND_FREE(testBlockStmt);

    Stmt* testFunctionBody = NEW_BLOCK_STMT();
    BLOCK_STMT_ADD_STMT(testFunctionBody, 
        NEW_RETURN_STMT(
            NEW_BINARY_EXPR(
                NEW_LITERAL_EXPR(NEW_IDENT("a")),
                NEW_TOKEN(TOKEN_MUL, "*", 1),
                NEW_BINARY_EXPR(
                    NEW_GROUP_EXPR(
                        NEW_BINARY_EXPR(
                            NEW_LITERAL_EXPR(NEW_FLOAT(1.0)),
                            NEW_TOKEN(TOKEN_SUB, "-", 1),
                            NEW_LITERAL_EXPR(NEW_IDENT("t"))
                        )
                    ),
                    NEW_TOKEN(TOKEN_ADD, "+", 1),
                    NEW_BINARY_EXPR(
                        NEW_LITERAL_EXPR(NEW_IDENT("b")),
                        NEW_TOKEN(TOKEN_MUL, "*", 1),
                        NEW_LITERAL_EXPR(NEW_IDENT("t"))
                    )
                )
            )
        )
    );
    Stmt* testFunctionSmt = NEW_FUNCTION_STMT("lerp", testFunctionBody);
    FUNCTION_ADD_PARAM(testFunctionSmt, NEW_LITERAL_EXPR(NEW_IDENT("a")));
    FUNCTION_ADD_PARAM(testFunctionSmt, NEW_LITERAL_EXPR(NEW_IDENT("b")));
    FUNCTION_ADD_PARAM(testFunctionSmt, NEW_LITERAL_EXPR(NEW_IDENT("t")));
    STMT_PRINT_AND_FREE(testFunctionSmt);

    return EXIT_SUCCESS;
}
