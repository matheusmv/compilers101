%option c++
%option noyywrap

%{

#include <fstream>
#include <iostream>

#include "token.h"

#define RETURN_AS_INT(TT) return static_cast<int>(TT);

std::size_t line_number = 1;

%}

WS [ \t]
NL \n

SL_COMMENT "//"(.*)
ML_COMMENT "/*"({WS}|{NL})*(.*)({WS}|{NL})*"*/"

CHARACTER [a-zA-Z]
DIGIT     [0-9]

IDENT  (_)*{CHARACTER}(_|{CHARACTER}|{DIGIT})*
INT    {DIGIT}+
FLOAT  {DIGIT}*\.{DIGIT}+
CHAR   '(.*)'
STRING \"(.*)\"

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

{IDENT} {
    std::string literal = yytext;
    if (is_identifier(literal) || is_keyword(literal)) {
        RETURN_AS_INT(lookup(literal))
    } else {
        RETURN_AS_INT(TokenType::TT_ILLEGAL)
    }
};
{INT}    { RETURN_AS_INT(TokenType::TT_INT) };
{FLOAT}  { RETURN_AS_INT(TokenType::TT_FLOAT) };
{CHAR}   { RETURN_AS_INT(TokenType::TT_CHAR) };
{STRING} { RETURN_AS_INT(TokenType::TT_STRING) };

{ADD} { RETURN_AS_INT(TokenType::TT_ADD) };
{SUB} { RETURN_AS_INT(TokenType::TT_SUB) };
{MUL} { RETURN_AS_INT(TokenType::TT_MUL) };
{QUO} { RETURN_AS_INT(TokenType::TT_QUO) };
{REM} { RETURN_AS_INT(TokenType::TT_REM) };

{TILDE} { RETURN_AS_INT(TokenType::TT_TILDE) };
{AND}   { RETURN_AS_INT(TokenType::TT_AND) };
{OR}    { RETURN_AS_INT(TokenType::TT_OR) };
{XOR}   { RETURN_AS_INT(TokenType::TT_XOR) };
{SHL}   { RETURN_AS_INT(TokenType::TT_SHL) };
{SHR}   { RETURN_AS_INT(TokenType::TT_SHR) };

{ADD_ASSIGN} { RETURN_AS_INT(TokenType::TT_ADD_ASSIGN) };
{SUB_ASSIGN} { RETURN_AS_INT(TokenType::TT_SUB_ASSIGN) };
{MUL_ASSIGN} { RETURN_AS_INT(TokenType::TT_MUL_ASSIGN) };
{QUO_ASSIGN} { RETURN_AS_INT(TokenType::TT_QUO_ASSIGN) };
{REM_ASSIGN} { RETURN_AS_INT(TokenType::TT_REM_ASSIGN) };

{AND_ASSIGN} { RETURN_AS_INT(TokenType::TT_AND_ASSIGN) };
{OR_ASSIGN}  { RETURN_AS_INT(TokenType::TT_OR_ASSIGN) };
{XOR_ASSIGN} { RETURN_AS_INT(TokenType::TT_XOR_ASSIGN) };
{SHL_ASSIGN} { RETURN_AS_INT(TokenType::TT_SHL_ASSIGN) };
{SHR_ASSIGN} { RETURN_AS_INT(TokenType::TT_SHR_ASSIGN) };

{LAND} { RETURN_AS_INT(TokenType::TT_LAND) };
{LOR}  { RETURN_AS_INT(TokenType::TT_LOR) };
{INC}  { RETURN_AS_INT(TokenType::TT_INC) };
{DEC}  { RETURN_AS_INT(TokenType::TT_DEC) };

{EQL}    { RETURN_AS_INT(TokenType::TT_EQL) };
{LSS}    { RETURN_AS_INT(TokenType::TT_LSS) };
{GTR}    { RETURN_AS_INT(TokenType::TT_GTR) };
{ASSIGN} { RETURN_AS_INT(TokenType::TT_ASSIGN) };
{NOT}    { RETURN_AS_INT(TokenType::TT_NOT) };

{NEQ}      { RETURN_AS_INT(TokenType::TT_NEQ) };
{LEQ}      { RETURN_AS_INT(TokenType::TT_LEQ) };
{GEQ}      { RETURN_AS_INT(TokenType::TT_GEQ) };
{DEFINE}   { RETURN_AS_INT(TokenType::TT_DEFINE) };
{ELLIPSIS} { RETURN_AS_INT(TokenType::TT_ELLIPSIS) };

{LPAREN} { RETURN_AS_INT(TokenType::TT_LPAREN) };
{LBRACK} { RETURN_AS_INT(TokenType::TT_LBRACK) };
{LBRACE} { RETURN_AS_INT(TokenType::TT_LBRACE) };
{COMMA}  { RETURN_AS_INT(TokenType::TT_COMMA) };
{PERIOD} { RETURN_AS_INT(TokenType::TT_PERIOD) };

{RPAREN}    { RETURN_AS_INT(TokenType::TT_RPAREN) };
{RBRACK}    { RETURN_AS_INT(TokenType::TT_RBRACK) };
{RBRACE}    { RETURN_AS_INT(TokenType::TT_RBRACE) };
{SEMICOLON} { RETURN_AS_INT(TokenType::TT_SEMICOLON) };
{COLON}     { RETURN_AS_INT(TokenType::TT_COLON) };


. { RETURN_AS_INT(TokenType::TT_ILLEGAL) };

<<EOF>> { RETURN_AS_INT(TokenType::TT_EOF) };

%%
