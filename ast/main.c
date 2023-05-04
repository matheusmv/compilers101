#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "src/ast.h"
#include "src/list.h"
#include "src/map.h"
#include "src/context.h"
#include "src/smem.h"
#include "src/token.h"
#include "src/types.h"
#include "src/utils.h"


int str_cmp(const void* a, const void* b) {
    return strcmp(a, b);
}

bool entry_cmp(const MapEntry** entry, char** key) {
    return strcmp((*entry)->key, *key) == 0;
}

void entry_to_string(const void** mapEntry) {
    MapEntry* entry = (MapEntry*) *mapEntry;

    printf("key: %s, value: ", (char*) entry->key);
    stmt_to_string((Stmt**) &entry->value);
    printf("\n");
}

void print_map_entry(const MapEntry* entry) {
    printf("key=%s, value=%s\n",
        (char*) entry->key, (char*) entry->value);
}

void print_map(const Map* map) {
    MapIterator* iterator = map_iterator_new(map);
    while (map_iterator_has_next(iterator)) {
        MapEntry* entry = map_iterator_next(iterator);
        print_map_entry(entry);
    }
    map_iterator_free(&iterator);
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
    CALL_EXPR_ADD_ARGS(testCall,
        NEW_FLOAT_LITERAL(0.15),
        NEW_FLOAT_LITERAL(3.14),
        NEW_FLOAT_LITERAL(0.3)
    );

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
    BLOCK_STMT_ADD_DECLS(testInnerBlockStmt,
        NEW_STMT_DECL(NEW_EXPR_STMT(testMath)),
        NEW_STMT_DECL(NEW_EXPR_STMT(testChar)),
        NEW_STMT_DECL(NEW_EXPR_STMT(testIdent)),
        NEW_STMT_DECL(NEW_EXPR_STMT(testString))
    );

    Stmt* testBlockStmt = NEW_BLOCK_STMT();
    BLOCK_STMT_ADD_DECLS(testBlockStmt,
        NEW_STMT_DECL(testInnerBlockStmt),
        NEW_STMT_DECL(NEW_EXPR_STMT(testBool)),
        NEW_STMT_DECL(NEW_EXPR_STMT(testAssign)),
        NEW_STMT_DECL(NEW_EXPR_STMT(testCall)),
        NEW_STMT_DECL(NEW_EXPR_STMT(testLogicalAnd)),
        NEW_STMT_DECL(NEW_EXPR_STMT(testUnary)),
        NEW_STMT_DECL(testExprStmt)
    );
    STMT_PRINT_AND_FREE(testBlockStmt);

    Stmt* testFunctionBody = NEW_BLOCK_STMT();
    BLOCK_STMT_ADD_DECL(testFunctionBody,
        NEW_STMT_DECL(
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
        )
    );
    Decl* testFunctionDecl = NEW_FUNCTION_DECL(
        NEW_TOKEN(TOKEN_IDENT, "lerp", 1),
        testFunctionBody
    );
    FUNCTION_ADD_PARAMS(testFunctionDecl,
        NEW_FIELD_DECL(NEW_TOKEN(TOKEN_IDENT, "a", 1), NEW_FLOAT_TYPE()),
        NEW_FIELD_DECL(NEW_TOKEN(TOKEN_IDENT, "b", 1), NEW_FLOAT_TYPE()),
        NEW_FIELD_DECL(NEW_TOKEN(TOKEN_IDENT, "t", 1), NEW_FLOAT_TYPE()),
    );
    FUNCTION_ADD_RETURN_TYPE(testFunctionDecl, NEW_FLOAT_TYPE());
    DECL_PRINT_AND_FREE(testFunctionDecl);

    Decl* testLetDecl = NEW_LET_DECL(
        NEW_TOKEN(TOKEN_IDENT, "name", 1),
        NEW_STRING_TYPE(),
        NEW_STRING_LITERAL("john nash")
    );
    DECL_PRINT_AND_FREE(testLetDecl);

    Expr* printMessage = NEW_CALL_EXPR(NEW_IDENT_LITERAL("print"));
    CALL_EXPR_ADD_ARG(printMessage, NEW_STRING_LITERAL("the number is even"));

    Stmt* thenBlock = NEW_BLOCK_STMT();
    BLOCK_STMT_ADD_DECL(thenBlock, NEW_STMT_DECL(NEW_EXPR_STMT(printMessage)));

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
    BLOCK_STMT_ADD_DECL(whileStmtBody,
        NEW_STMT_DECL(
            NEW_EXPR_STMT(
                NEW_ASSIGN_EXPR(
                    NEW_IDENT_LITERAL("i"),
                    NEW_TOKEN(TOKEN_ADD_ASSIGN, "+=", 1),
                    NEW_INT_LITERAL(1)
                )
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
        NEW_LET_DECL(NEW_TOKEN(TOKEN_IDENT, "i", 1), NEW_INT_TYPE(), NEW_INT_LITERAL(0)),
        NEW_BINARY_EXPR(
            NEW_IDENT_LITERAL("i"),
            NEW_TOKEN(TOKEN_LSS, "<", 1),
            NEW_INT_LITERAL(10)
        ),
        NEW_UPDATE_EXPR(NEW_IDENT_LITERAL("i"), NEW_TOKEN(TOKEN_INC, "++", 1)),
        NEW_BLOCK_STMT()
    );
    STMT_PRINT_AND_FREE(testForStmt);

    Decl *testLetDeclWithNilValue = NEW_LET_DECL(
        NEW_TOKEN(TOKEN_IDENT, "object", 1),
        NULL,
        NEW_NIL_LITERAL()
    );

    Decl* testStructDecl = NEW_STRUCT_DECL(NEW_TOKEN(TOKEN_IDENT, "User", 1));
    Type* testCustomType = NEW_STRUCT_TYPE(8);
    CUSTOM_TYPE_ADD_FIELDS(testCustomType,
        NEW_NAMED_TYPE("street", NEW_STRING_TYPE()),
        NEW_NAMED_TYPE("zip", NEW_STRING_TYPE()),
    );
    STRUCT_DECL_ADD_FIELDS(testStructDecl,
        NEW_FIELD_DECL(NEW_TOKEN(TOKEN_STRING, "id", 1), NEW_INT_TYPE()),
        NEW_FIELD_DECL(NEW_TOKEN(TOKEN_STRING, "username", 1), NEW_STRING_TYPE()),
        NEW_FIELD_DECL(NEW_TOKEN(TOKEN_STRING, "password", 1), NEW_STRING_TYPE()),
        NEW_FIELD_DECL(NEW_TOKEN(TOKEN_STRING, "email", 1), NEW_STRING_TYPE()),
        NEW_FIELD_DECL(NEW_TOKEN(TOKEN_STRING, "address", 1), testCustomType)
    );
    DECL_PRINT_AND_FREE(testStructDecl);

    Expr* structInit = NEW_STRUCT_INIT_EXPR(NEW_TOKEN(TOKEN_IDENT, "User", 1));
    Type* structType = NEW_STRUCT_TYPE(0);
    CUSTOM_TYPE_ADD_FIELDS(structType,
        NEW_NAMED_TYPE("street", NEW_STRING_TYPE()),
        NEW_NAMED_TYPE("zip", NEW_STRING_TYPE())
    );
    Expr* inlineStruct = NEW_STRUCT_INLINE_EXPR(structType);
    STRUCT_INIT_EXPR_ADD_FIELDS(inlineStruct,
        NEW_FIELD_EXPR(NEW_TOKEN(TOKEN_STRING, "street", 1), NEW_STRING_LITERAL("street 101")),
        NEW_FIELD_EXPR(NEW_TOKEN(TOKEN_STRING, "zip", 1), NEW_STRING_LITERAL("000111222"))
    );
    STRUCT_INIT_EXPR_ADD_FIELDS(structInit,
        NEW_FIELD_EXPR(NEW_TOKEN(TOKEN_STRING, "id", 1), NEW_CALL_EXPR(NEW_IDENT_LITERAL("getId"))),
        NEW_FIELD_EXPR(NEW_TOKEN(TOKEN_STRING, "username", 1), NEW_STRING_LITERAL("john12345")),
        NEW_FIELD_EXPR(NEW_TOKEN(TOKEN_STRING, "password", 1), NEW_STRING_LITERAL("12345")),
        NEW_FIELD_EXPR(NEW_TOKEN(TOKEN_STRING, "email", 1), NEW_STRING_LITERAL("john@email.com")),
        NEW_FIELD_EXPR(NEW_TOKEN(TOKEN_STRING, "address", 1), inlineStruct),
    );
    Decl* testStructInitExpr = NEW_LET_DECL(
        NEW_TOKEN(TOKEN_IDENT, "john", 1),
        NEW_CUSTOM_TYPE(40, "User"),
        structInit
    );
    DECL_PRINT_AND_FREE(testStructInitExpr);

    Context* globalContext = context_new(MAP_NEW(64, entry_cmp, NULL, decl_free));

    context_define(globalContext, "object", testLetDeclWithNilValue);

    Context* innerScope = context_enclosed_new(
        globalContext,
        MAP_NEW(64, entry_cmp, NULL, stmt_free)
    );

    void* value = context_get(innerScope, "object");
    if (value != NULL) {
        stmt_to_string((Stmt**) &value);
        printf("\n");
    }

    context_free(&innerScope);
    context_free(&globalContext);

    Map* map = MAP_NEW(10, (bool (*)(const void**, void**)) entry_cmp, NULL, NULL);

    map_put(map, "one", "um");
    map_put(map, "two", "dois");
    map_put(map, "three", "três");

    map_remove(map, "two");

    print_map(map);

    map_free(&map);

    List* testSort = list_new(safe_free);

    list_insert_last(&testSort, str_dup("z"));
    list_insert_last(&testSort, str_dup("a"));
    list_insert_last(&testSort, str_dup("j"));
    list_insert_last(&testSort, str_dup("k"));
    list_insert_last(&testSort, str_dup("w"));
    list_insert_last(&testSort, str_dup("b"));
    list_insert_last(&testSort, str_dup("c"));

    printf("Before list_sort:\n");
    list_foreach(word, testSort) {
        printf("%s\n", (char*) word->value);
    }

    list_sort(&testSort, str_cmp);

    printf("After list_sort in order:\n");
    list_foreach(word, testSort) {
        printf("%s\n", (char*) word->value);
    }

    list_free(&testSort);

    return EXIT_SUCCESS;
}
