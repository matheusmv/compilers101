%option c++
%option noyywrap

%{

#include <fstream>
#include <iostream>

#include "token.h"

static std::size_t line_number = 1;

void log_token_info(const TokenType& token_type, const char* literal, const std::size_t& line) {
    std::printf("{ TokenType: %s, Literal: %s, Line: %ld }\n", to_string(token_type).c_str(), literal, line);
}

%}

WS [ \t]
NL \n

SL_COMMENT "//"(.*)
ML_COMMENT "/*"{NL}*(.*){NL}*"*/"

CHARACTER [a-zA-Z]
DIGIT     [0-9]

INT    {DIGIT}+
FLOAT  {DIGIT}*\.{DIGIT}+
CHAR   '(.*)'
STRING \"(.*)\"
BOOL   "true"|"false"

IDENT (_)*{CHARACTER}(_|{CHARACTER}|{DIGIT})*

ADD "+"
SUB "-"
MUL "*"
QUO "/"
REM "%"

TILDE "~"
AND   "&"
OR    "|"
XOR   "^"
SHL   "<<"
SHR   ">>"

ADD_ASSIGN "+="
SUB_ASSIGN "-="
MUL_ASSIGN "*="
QUO_ASSIGN "/="
REM_ASSIGN "%="

AND_ASSIGN "&="
OR_ASSIGN  "|="
XOR_ASSIGN "^="
SHL_ASSIGN "<<="
SHR_ASSIGN ">>="

LAND "&&"
LOR  "||"
INC  "++"
DEC  "--"

EQL    "=="
LSS    "<"
GTR    ">"
ASSIGN "="
NOT    "!"

NEQ      "!="
LEQ      "<="
GEQ      ">="
DEFINE   ":="
ELLIPSIS "..."

LPAREN "("
LBRACK "["
LBRACE "{"
COMMA  ","
PERIOD "."

RPAREN    ")"
RBRACK    "]"
RBRACE    "}"
SEMICOLON ";"
COLON     ":"

%%

{WS}* {};

{NL}  { line_number += 1; };

{SL_COMMENT} {};
{ML_COMMENT} {
    std::string comment = yytext;
    for (const auto& c : comment) {
        if (c == '\n') {
            line_number += 1;
        }
    }
};

{INT}    { log_token_info(TokenType::TT_INT, yytext, line_number); };
{FLOAT}  { log_token_info(TokenType::TT_FLOAT, yytext, line_number); };
{CHAR}   { log_token_info(TokenType::TT_CHAR, yytext, line_number); };
{STRING} { log_token_info(TokenType::TT_STRING, yytext, line_number); };
{BOOL}   { log_token_info(lookup(yytext), yytext, line_number); };
{IDENT}  {
    std::string literal = yytext;
    if (is_identifier(literal) || is_keyword(literal)) {
        log_token_info(lookup(literal), literal.c_str(), line_number);
    } else {
        log_token_info(TokenType::TT_ILLEGAL, literal.c_str(), line_number);
    }
};

{ADD} { log_token_info(TokenType::TT_ADD, yytext, line_number); };
{SUB} { log_token_info(TokenType::TT_SUB, yytext, line_number); };
{MUL} { log_token_info(TokenType::TT_MUL, yytext, line_number); };
{QUO} { log_token_info(TokenType::TT_QUO, yytext, line_number); };
{REM} { log_token_info(TokenType::TT_REM, yytext, line_number); };

{TILDE} { log_token_info(TokenType::TT_TILDE, yytext, line_number); };
{AND}   { log_token_info(TokenType::TT_AND, yytext, line_number); };
{OR}    { log_token_info(TokenType::TT_OR, yytext, line_number); };
{XOR}   { log_token_info(TokenType::TT_XOR, yytext, line_number); };
{SHL}   { log_token_info(TokenType::TT_SHL, yytext, line_number); };
{SHR}   { log_token_info(TokenType::TT_SHR, yytext, line_number); };

{ADD_ASSIGN} { log_token_info(TokenType::TT_ADD_ASSIGN, yytext, line_number); };
{SUB_ASSIGN} { log_token_info(TokenType::TT_SUB_ASSIGN, yytext, line_number); };
{MUL_ASSIGN} { log_token_info(TokenType::TT_MUL_ASSIGN, yytext, line_number); };
{QUO_ASSIGN} { log_token_info(TokenType::TT_QUO_ASSIGN, yytext, line_number); };
{REM_ASSIGN} { log_token_info(TokenType::TT_REM_ASSIGN, yytext, line_number); };

{AND_ASSIGN} { log_token_info(TokenType::TT_AND_ASSIGN, yytext, line_number); };
{OR_ASSIGN}  { log_token_info(TokenType::TT_OR_ASSIGN, yytext, line_number); };
{XOR_ASSIGN} { log_token_info(TokenType::TT_XOR_ASSIGN, yytext, line_number); };
{SHL_ASSIGN} { log_token_info(TokenType::TT_SHL_ASSIGN, yytext, line_number); };
{SHR_ASSIGN} { log_token_info(TokenType::TT_SHR_ASSIGN, yytext, line_number); };

{LAND} { log_token_info(TokenType::TT_LAND, yytext, line_number); };
{LOR}  { log_token_info(TokenType::TT_LOR, yytext, line_number); };
{INC}  { log_token_info(TokenType::TT_INC, yytext, line_number); };
{DEC}  { log_token_info(TokenType::TT_DEC, yytext, line_number); };

{EQL}    { log_token_info(TokenType::TT_EQL, yytext, line_number); };
{LSS}    { log_token_info(TokenType::TT_LSS, yytext, line_number); };
{GTR}    { log_token_info(TokenType::TT_GTR, yytext, line_number); };
{ASSIGN} { log_token_info(TokenType::TT_ASSIGN, yytext, line_number); };
{NOT}    { log_token_info(TokenType::TT_NOT, yytext, line_number); };

{NEQ}      { log_token_info(TokenType::TT_NEQ, yytext, line_number); };
{LEQ}      { log_token_info(TokenType::TT_LEQ, yytext, line_number); };
{GEQ}      { log_token_info(TokenType::TT_GEQ, yytext, line_number); };
{DEFINE}   { log_token_info(TokenType::TT_DEFINE, yytext, line_number); };
{ELLIPSIS} { log_token_info(TokenType::TT_ELLIPSIS, yytext, line_number); };

{LPAREN} { log_token_info(TokenType::TT_LPAREN, yytext, line_number); };
{LBRACK} { log_token_info(TokenType::TT_LBRACK, yytext, line_number); };
{LBRACE} { log_token_info(TokenType::TT_LBRACE, yytext, line_number); };
{COMMA}  { log_token_info(TokenType::TT_COMMA, yytext, line_number); };
{PERIOD} { log_token_info(TokenType::TT_PERIOD, yytext, line_number); };

{RPAREN}    { log_token_info(TokenType::TT_RPAREN, yytext, line_number); };
{RBRACK}    { log_token_info(TokenType::TT_RBRACK, yytext, line_number); };
{RBRACE}    { log_token_info(TokenType::TT_RBRACE, yytext, line_number); };
{SEMICOLON} { log_token_info(TokenType::TT_SEMICOLON, yytext, line_number); };
{COLON}     { log_token_info(TokenType::TT_COLON, yytext, line_number); };

. { log_token_info(TokenType::TT_ILLEGAL, yytext, line_number); };

<<EOF>> {
    log_token_info(TokenType::TT_EOF, yytext, line_number);
    return EOF;
};

%%
