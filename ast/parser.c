#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE* yyin;

extern int yyparse(void);

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
    if (success) {
        printf("Parsing Successful\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
