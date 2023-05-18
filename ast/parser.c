#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/ast.h"
#include "src/list.h"
#include "src/type-checker.h"

extern FILE* yyin;

extern int yyparse(void);

List* declarations = NULL;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s file.rose\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* src = fopen(argv[1], "r");
    if (src == NULL) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    yyin = src;
    yyparse();

    fclose(src);

    extern bool success;
    if (!success) {
        list_free(&declarations);
        return EXIT_FAILURE;
    }

    printf("Parsing Successful\n");

    TypeCheckerStatus status = check(declarations);
    if (status == TYPE_CHECKER_FAILURE) {
        printf("Type checker error\n");
        list_free(&declarations);
        return EXIT_FAILURE;
    }

    list_foreach(declaration, declarations) {
        decl_to_string((Decl**) &declaration->value);
        printf("\n");
    }

    list_free(&declarations);

    return EXIT_SUCCESS;
}
