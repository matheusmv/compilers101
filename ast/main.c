#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "map.h"


bool str_cmp(const MapEntry** entry, char** key) {
    return strcmp((*entry)->key, *key) == 0;
}

int main(void) {
    Expr* testMath = NEW_BINARY_EXPR(
        NEW_GROUP_EXPR(
            NEW_BINARY_EXPR(
                NEW_FLOAT_LITERAL(1.1),
                NEW_TOKEN(TOKEN_ADD, "+", 1),
                NEW_FLOAT_LITERAL(2.3)
            )
        ),
        NEW_TOKEN(TOKEN_MUL, "*", 1),
        NEW_FLOAT_LITERAL(14.75)
    );

    Expr* testChar = NEW_CHAR_LITERAL('F');

    Expr* testIdent = NEW_IDENT_LITERAL("$HOME");

    Expr* testString = NEW_STRING_LITERAL("Alone after all.");

    Expr* testBool = NEW_BOOL_LITERAL((2 + 2) == 4);

    Expr* testAssign = NEW_ASSIGN_EXPR(
        NEW_IDENT_LITERAL("result"),
        NEW_TOKEN(TOKEN_ASSIGN, "=", 1),
        NEW_BINARY_EXPR(
            NEW_GROUP_EXPR(
                NEW_BINARY_EXPR(
                    NEW_IDENT_LITERAL("n"),
                    NEW_TOKEN(TOKEN_MUL, "*", 1),
                    NEW_GROUP_EXPR(
                        NEW_BINARY_EXPR(
                            NEW_IDENT_LITERAL("n"),
                            NEW_TOKEN(TOKEN_SUB, "+", 1),
                            NEW_INT_LITERAL(1)
                        )
                    )
                )
            ),
            NEW_TOKEN(TOKEN_QUO, "/", 1),
            NEW_INT_LITERAL(2)
        )
    );

    Expr* testCall = NEW_CALL_EXPR(NEW_IDENT_LITERAL("lerp"));
    CALL_EXPR_ADD_ARG(testCall, NEW_FLOAT_LITERAL(0.15));
    CALL_EXPR_ADD_ARG(testCall, NEW_FLOAT_LITERAL(3.14));
    CALL_EXPR_ADD_ARG(testCall, NEW_FLOAT_LITERAL(0.3));

    Expr* testLogicalAnd = NEW_LOGICAL_EXPR(
        NEW_CALL_EXPR(NEW_IDENT_LITERAL("initialized")),
        NEW_TOKEN(TOKEN_LAND, "&&", 1),
        NEW_CALL_EXPR(NEW_IDENT_LITERAL("empty"))
    );

    Expr* testUnary = NEW_UNARY_EXPR(
        NEW_TOKEN(TOKEN_NOT, "!", 1),
        NEW_BOOL_LITERAL(true)
    );

    Stmt* testExprStmt = NEW_EXPR_STMT(NEW_STRING_LITERAL("OK"));

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
                NEW_IDENT_LITERAL("a"),
                NEW_TOKEN(TOKEN_MUL, "*", 1),
                NEW_BINARY_EXPR(
                    NEW_GROUP_EXPR(
                        NEW_BINARY_EXPR(
                            NEW_FLOAT_LITERAL(1.0),
                            NEW_TOKEN(TOKEN_SUB, "-", 1),
                            NEW_IDENT_LITERAL("t")
                        )
                    ),
                    NEW_TOKEN(TOKEN_ADD, "+", 1),
                    NEW_BINARY_EXPR(
                        NEW_IDENT_LITERAL("b"),
                        NEW_TOKEN(TOKEN_MUL, "*", 1),
                        NEW_IDENT_LITERAL("t")
                    )
                )
            )
        )
    );
    Stmt* testFunctionSmt = NEW_FUNCTION_STMT(
        NEW_TOKEN(TOKEN_IDENT, "lerp", 1),
        testFunctionBody
    );
    FUNCTION_ADD_PARAM(testFunctionSmt, NEW_IDENT_LITERAL("a"));
    FUNCTION_ADD_PARAM(testFunctionSmt, NEW_IDENT_LITERAL("b"));
    FUNCTION_ADD_PARAM(testFunctionSmt, NEW_IDENT_LITERAL("t"));
    STMT_PRINT_AND_FREE(testFunctionSmt);

    Stmt* testLetStmt = NEW_LET_STMT(
        NEW_TOKEN(TOKEN_IDENT, "name", 1),
        NEW_STRING_LITERAL("john nash")
    );
    STMT_PRINT_AND_FREE(testLetStmt);

    Expr* printMessage = NEW_CALL_EXPR(NEW_IDENT_LITERAL("print"));
    CALL_EXPR_ADD_ARG(printMessage, NEW_STRING_LITERAL("the number is even"));

    Stmt* thenBlock = NEW_BLOCK_STMT();
    BLOCK_STMT_ADD_STMT(thenBlock, NEW_EXPR_STMT(printMessage));

    Stmt* elseBlock = NULL;

    Stmt* testIfStmt = NEW_IF_STMT(
        NEW_BINARY_EXPR(
            NEW_BINARY_EXPR(
                NEW_IDENT_LITERAL("i"),
                NEW_TOKEN(TOKEN_REM, "%", 1),
                NEW_INT_LITERAL(2)
            ),
            NEW_TOKEN(TOKEN_EQL, "==", 0),
            NEW_INT_LITERAL(0)
        ),
        thenBlock,
        elseBlock
    );
    STMT_PRINT_AND_FREE(testIfStmt);

    Stmt* whileStmtBody = NEW_BLOCK_STMT();
    BLOCK_STMT_ADD_STMT(whileStmtBody,
        NEW_EXPR_STMT(
            NEW_ASSIGN_EXPR(
                NEW_IDENT_LITERAL("i"),
                NEW_TOKEN(TOKEN_ADD_ASSIGN, "+=", 1),
                NEW_INT_LITERAL(1)
            )
        )
    );
    Stmt* testWhileStmt = NEW_WHILE_STMT(
        NEW_BINARY_EXPR(
            NEW_IDENT_LITERAL("i"),
            NEW_TOKEN(TOKEN_LSS, "<", 1),
            NEW_INT_LITERAL(10)
        ),
        whileStmtBody
    );
    STMT_PRINT_AND_FREE(testWhileStmt);

    Stmt* testForStmt = NEW_FOR_STMT(
        NEW_LET_STMT(NEW_TOKEN(TOKEN_IDENT, "i", 1), NEW_INT_LITERAL(0)),
        NEW_BINARY_EXPR(
            NEW_IDENT_LITERAL("i"),
            NEW_TOKEN(TOKEN_LSS, "<", 1),
            NEW_INT_LITERAL(10)
        ),
        NEW_UPDATE_EXPR(NEW_IDENT_LITERAL("i"), NEW_TOKEN(TOKEN_INC, "++", 1)),
        NEW_BLOCK_STMT()
    );
    STMT_PRINT_AND_FREE(testForStmt);

    Stmt *testLetStmtWithNilValue = NEW_LET_STMT(
        NEW_TOKEN(TOKEN_IDENT, "object", 1),
        NEW_NIL_LITERAL()
    );

    Map* contextMap = MAP_NEW(64, str_cmp, NULL, stmt_free);

    map_put(contextMap, "object", testLetStmtWithNilValue);

    void* object = map_get(contextMap, "object");
    if (object != NULL) {
        stmt_to_string((Stmt**) &object);
        printf("\n");
    }

    map_free(&contextMap);

    return EXIT_SUCCESS;
}
