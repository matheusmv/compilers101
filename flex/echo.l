%option noyywrap

%{

#include <stdio.h>
#include <stdlib.h>

#include "token.h"

%}

WS [ \t\n]

CHARACTER [a-zA-Z]
DIGIT [0-9]

INT [+-]?(0|[1-9]{DIGIT}*)
FLOAT [+-]?{DIGIT}+\.{DIGIT}+
CHAR '{CHARACTER}'
STRING \"(.*)\"
BOOL "true"|"false"

IDENT (_)*{CHARACTER}(_|{CHARACTER}|{DIGIT})*

%%

{WS}+       {};

{INT}       { printf("%s: %s\n", token_str(T_INT), yytext);     };
{FLOAT}     { printf("%s: %s\n", token_str(T_FLOAT), yytext);   };
{CHAR}      { printf("%s: %s\n", token_str(T_CHAR), yytext);    };
{STRING}    { printf("%s: %s\n", token_str(T_STRING), yytext);  };
{BOOL}      { printf("%s: %s\n", token_str(T_BOOL), yytext);    };

{IDENT}     { printf("%s: %s\n", token_str(T_IDENT), yytext);   };

.           { printf("%s: %s\n", token_str(T_ILLEGAL), yytext); };

<<EOF>>  { return T_EOF; };

%%

int main(void) {
    yylex();

    return EXIT_SUCCESS;
}
