%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.8"

%header
%locations

%define api.token.raw
%define api.token.constructor
%define api.token.prefix {TOK_}

%define api.value.type variant

%define parse.assert
%define parse.trace
%define parse.error detailed
%define parse.lac full

%code requires {

#include <string>
#include <cmath>

class Interpreter;

}

%param { Interpreter& interpreter }

%code {

#include "interpreter.hpp"

}

%token
  ADD     "+"
  SUB     "-"
  MUL     "*"
  QUO     "/"
  POW     "^"
  ASSIGN  ":="
  LPAREN  "("
  RPAREN  ")"
  INPUT   "input"
  PRINT   "print"
  SQRT    "sqrt"
;

%token <std::string> COMMENT "comment"
%token <std::string> IDENTIFIER "identifier"
%token <std::string> STRING "str"
%token <double> NUMBER "number"
%nterm <double> expr

%left "+" "-"
%left "*" "/"

%nonassoc SQRT

%left "^"

%nonassoc CALL_EXPR

%start program

%%

program
    : %empty
    | program statements
    ;

statements
    : comment_statement
    | assign_statement
    | expr_statement
    ;

comment_statement
    : "comment"
    ;

assign_statement
    : assignment
    ;

assignment
    : "identifier" ":=" expr { interpreter.variables[$1] = $3; }
    ;

expr_statement
    : "identifier" ":=" "input" "(" "str" ")" %prec CALL_EXPR
        {
            double value;
            $5.erase(0, 1);
            $5.erase($5.size() - 1);
            std::cout << $5;
            std::cin >> value;
            interpreter.variables[$1] = value;
        }
    | "print" "(" expr ")" %prec CALL_EXPR
        {
            std::cout << $3 << '\n';
        }
    | expr
    ;

expr
    : "number"
    | "identifier"
        { 
            if (interpreter.variables.find($1) != interpreter.variables.end()) {
                $$ = interpreter.variables.at($1);
            } else {
                throw yy::parser::syntax_error(interpreter.location, "undefined variable: " + $1);
            }
        }
    | expr "+" expr { $$ = $1 + $3; }
    | expr "-" expr { $$ = $1 - $3; }
    | expr "*" expr { $$ = $1 * $3; }
    | expr "/" expr { $$ = $1 / $3; }
    | expr "^" expr %prec POW
        { 
            $$ = pow($1, $3); 
        }
    | "(" expr ")"  { $$ = $2; }
    | "sqrt" "(" expr ")" %prec SQRT
        {
            $$ = sqrt($3);
        }
    ;

%%

void yy::parser::error(const location_type& location, const std::string& message) {
    std::cerr << location << ": " << message << '\n';
}
