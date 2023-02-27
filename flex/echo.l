%option noyywrap

%{

#include <fstream>
#include <iostream>

#include "token.h"

static std::size_t line_number = 1;

void log_token_info(const TokenType& token_type, const char* literal, const std::size_t line) {
    std::printf("{ TokenType: %s, Literal: %s, Line: %ld }\n",to_string(token_type).c_str(), literal, line);
}

%}

WS [ \t]
NL \n

SL_COMMENT "//"(.*)
ML_COMMENT "/*"({WS}|{NL})*(.*)({WS}|{NL})*"*/"

CHARACTER [a-zA-Z]
DIGIT     [0-9]

INT    (0|[1-9]){DIGIT}*
FLOAT  (0|[1-9])?{DIGIT}*\.{DIGIT}+
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

{NL}  {  line_number += 1;   };

{SL_COMMENT} {};
{ML_COMMENT} {
    std::string comment = YYText();
    for (const auto& c : comment) {
        if (c == '\n') {
            line_number += 1;
        }
    }
};

{INT}    {   log_token_info(TokenType::TT_INT, YYText(), line_number);      };
{FLOAT}  {   log_token_info(TokenType::TT_FLOAT, YYText(), line_number);    };
{CHAR}   {   log_token_info(TokenType::TT_CHAR, YYText(), line_number);     };
{STRING} {   log_token_info(TokenType::TT_STRING, YYText(), line_number);   };
{BOOL}   {   log_token_info(lookup(YYText()), YYText(), line_number);       };
{IDENT}  {
    std::string literal = YYText();
    if (is_identifier(literal) || is_keyword(literal)) {
        log_token_info(lookup(literal), literal.c_str(), line_number);
    } else {
        log_token_info(TokenType::TT_ILLEGAL, literal.c_str(), line_number);
    }
};

{ADD} {   log_token_info(TokenType::TT_ADD, YYText(), line_number);   };
{SUB} {   log_token_info(TokenType::TT_SUB, YYText(), line_number);   };
{MUL} {   log_token_info(TokenType::TT_MUL, YYText(), line_number);   };
{QUO} {   log_token_info(TokenType::TT_QUO, YYText(), line_number);   };
{REM} {   log_token_info(TokenType::TT_REM, YYText(), line_number);   };

{TILDE} {   log_token_info(TokenType::TT_TILDE, YYText(), line_number);   };
{AND}   {   log_token_info(TokenType::TT_AND, YYText(), line_number);     };
{OR}    {   log_token_info(TokenType::TT_OR, YYText(), line_number);      };
{XOR}   {   log_token_info(TokenType::TT_XOR, YYText(), line_number);     };
{SHL}   {   log_token_info(TokenType::TT_SHL, YYText(), line_number);     };
{SHR}   {   log_token_info(TokenType::TT_SHR, YYText(), line_number);     };

{ADD_ASSIGN} {   log_token_info(TokenType::TT_ADD_ASSIGN, YYText(), line_number);   };
{SUB_ASSIGN} {   log_token_info(TokenType::TT_SUB_ASSIGN, YYText(), line_number);   };
{MUL_ASSIGN} {   log_token_info(TokenType::TT_MUL_ASSIGN, YYText(), line_number);   };
{QUO_ASSIGN} {   log_token_info(TokenType::TT_QUO_ASSIGN, YYText(), line_number);   };
{REM_ASSIGN} {   log_token_info(TokenType::TT_REM_ASSIGN, YYText(), line_number);   };

{AND_ASSIGN} {   log_token_info(TokenType::TT_AND_ASSIGN, YYText(), line_number);   };
{OR_ASSIGN}  {   log_token_info(TokenType::TT_OR_ASSIGN, YYText(), line_number);    };
{XOR_ASSIGN} {   log_token_info(TokenType::TT_XOR_ASSIGN, YYText(), line_number);   };
{SHL_ASSIGN} {   log_token_info(TokenType::TT_SHL_ASSIGN, YYText(), line_number);   };
{SHR_ASSIGN} {   log_token_info(TokenType::TT_SHR_ASSIGN, YYText(), line_number);   };

{LAND} {   log_token_info(TokenType::TT_LAND, YYText(), line_number);   };
{LOR}  {   log_token_info(TokenType::TT_LOR, YYText(), line_number);    };
{INC}  {   log_token_info(TokenType::TT_INC, YYText(), line_number);    };
{DEC}  {   log_token_info(TokenType::TT_DEC, YYText(), line_number);    };

{EQL}       {   log_token_info(TokenType::TT_EQL, YYText(), line_number);      };
{LSS}       {   log_token_info(TokenType::TT_LSS, YYText(), line_number);      };
{GTR}       {   log_token_info(TokenType::TT_GTR, YYText(), line_number);      };
{ASSIGN}    {   log_token_info(TokenType::TT_ASSIGN, YYText(), line_number);   };
{NOT}       {   log_token_info(TokenType::TT_NOT, YYText(), line_number);      };

{NEQ}      {   log_token_info(TokenType::TT_NEQ, YYText(), line_number);        };
{LEQ}      {   log_token_info(TokenType::TT_LEQ, YYText(), line_number);        };
{GEQ}      {   log_token_info(TokenType::TT_GEQ, YYText(), line_number);        };
{DEFINE}   {   log_token_info(TokenType::TT_DEFINE, YYText(), line_number);     };
{ELLIPSIS} {   log_token_info(TokenType::TT_ELLIPSIS, YYText(), line_number);   };

{LPAREN} {   log_token_info(TokenType::TT_LPAREN, YYText(), line_number);   };
{LBRACK} {   log_token_info(TokenType::TT_LBRACK, YYText(), line_number);   };
{LBRACE} {   log_token_info(TokenType::TT_LBRACE, YYText(), line_number);   };
{COMMA}  {   log_token_info(TokenType::TT_COMMA, YYText(), line_number);    };
{PERIOD} {   log_token_info(TokenType::TT_PERIOD, YYText(), line_number);   };

{RPAREN}    {   log_token_info(TokenType::TT_RPAREN, YYText(), line_number);      };
{RBRACK}    {   log_token_info(TokenType::TT_RBRACK, YYText(), line_number);      };
{RBRACE}    {   log_token_info(TokenType::TT_RBRACE, YYText(), line_number);      };
{SEMICOLON} {   log_token_info(TokenType::TT_SEMICOLON, YYText(), line_number);   };
{COLON}     {   log_token_info(TokenType::TT_COLON, YYText(), line_number);       };

. {   log_token_info(TokenType::TT_ILLEGAL, YYText(), line_number);   };

<<EOF>> {
    log_token_info(TokenType::TT_EOF, YYText(), line_number);
    return EOF;
};

%%

int main() {
    std::ifstream file;
    file.open("example.txt");

    yyFlexLexer scanner(&file);
    scanner.yylex();

    file.close();
}
