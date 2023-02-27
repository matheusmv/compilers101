%option noyywrap

%{

#include <iostream>

#include "token.h"

void log_token_info(const TokenType& token_type, const char* literal) {
    std::cout << to_string(token_type) << " : " << literal << std::endl;
}

%}

WS [ \t\n]

CHARACTER [a-zA-Z]
DIGIT     [0-9]

ML_COMMENT "/*"(.*)"*/"
SL_COMMENT "//"(.*)
COMMENT {ML_COMMENT}|{SL_COMMENT}

INT    (0|[1-9]){DIGIT}*
FLOAT  (0|[1-9])?{DIGIT}*\.{DIGIT}+
CHAR   '(.*)'
STRING \"(.*)\"
BOOL   "true"|"false"

IDENT (_)*{CHARACTER}(_|{CHARACTER}|{DIGIT})*

%%

{WS}+       {};

{COMMENT}   {
                log_token_info(TokenType::TT_COMMENT, YYText());
            };

{INT}       {
                log_token_info(TokenType::TT_INT, YYText());
            };

{FLOAT}     {
                log_token_info(TokenType::TT_FLOAT, YYText());
            };

{CHAR}      {
                log_token_info(TokenType::TT_CHAR, YYText());
            };

{STRING}    {
                log_token_info(TokenType::TT_STRING, YYText());
            };

{BOOL}      {
                log_token_info(lookup(YYText()), YYText());
            };

{IDENT}     {
                if (is_identifier(YYText())) {
                    log_token_info(lookup(YYText()), YYText());
                } else {
                    log_token_info(TokenType::TT_ILLEGAL, YYText());
                }
            };

.           {
                log_token_info(TokenType::TT_ILLEGAL, YYText());
            };

<<EOF>>     {
                return (int) TokenType::TT_EOF;
            };

%%

int main() {
    yyFlexLexer scanner;
    scanner.yylex();

    return 0;
}
