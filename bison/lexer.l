%option noyywrap nounput noinput batch debug

%{ /* -*- C++ -*- */

#include <cerrno>
#include <climits>
#include <cfloat>
#include <cstdlib>
#include <cstring>
#include <string>

#include "interpreter.hpp"
#include "parser.tab.hpp"

%}

%{

yy::parser::symbol_type make_NUMBER(const std::string &s, const yy::parser::location_type& loc);

%}

SL_COMMENT "//"(.*)
ML_COMMENT \/\*([^*]|\*+[^*/])*\*+\/

id     [a-zA-Z][a-zA-Z_0-9]*
str    \"([^"\\]|\\.)*\"
number [0-9]+(\.[0-9]*)?
blank  [ \t\r]

%{

#define YY_USER_ACTION  loc.columns(yyleng);

%}

%%

%{

yy::location& loc = interpreter.location;
loc.step();

%}

{blank}+ loc.step();
\n+ {
    loc.lines(yyleng);
    loc.step();
}

{SL_COMMENT} return yy::parser::make_COMMENT(yytext, loc);
{ML_COMMENT} {
    std::string comment = yytext;
    for (const auto& c : comment) {
        if (c == '\n') {
            loc.lines(1);
        }
    }
    return yy::parser::make_COMMENT(comment, loc);
}

"input"    return yy::parser::make_INPUT(loc);
"print"    return yy::parser::make_PRINT(loc);

"sqrt"     return yy::parser::make_SQRT(loc);

"+"        return yy::parser::make_ADD(loc);
"-"        return yy::parser::make_SUB(loc);
"*"        return yy::parser::make_MUL(loc);
"/"        return yy::parser::make_QUO(loc);
"^"        return yy::parser::make_POW(loc);
":="       return yy::parser::make_ASSIGN(loc);
"("        return yy::parser::make_LPAREN(loc);
")"        return yy::parser::make_RPAREN(loc);

{str}      return yy::parser::make_STRING(yytext, loc);
{number}   return make_NUMBER(yytext, loc);
{id}       return yy::parser::make_IDENTIFIER(yytext, loc);

. {
    throw yy::parser::syntax_error(loc, "invalid character: " + std::string(yytext));
}

<<EOF>> { return yy::parser::make_YYEOF(loc); }

%%

yy::parser::symbol_type make_NUMBER(const std::string &s, const yy::parser::location_type& loc) {
    errno = 0;

    double n = std::stod(s);

    if (!(DBL_MIN <= n && n <= DBL_MAX && errno != ERANGE)) {
        throw yy::parser::syntax_error(loc, "number is out of range: " + s);
    }

    return yy::parser::make_NUMBER(n, loc);
}

void Interpreter::scan_begin() {
    yy_flex_debug = trace_scanning;

    if (file.empty () || file == "-") {
        yyin = stdin;
    } else if (!(yyin = fopen(file.c_str (), "r"))) {
        std::cerr << "cannot open " << file << ": " << strerror (errno) << '\n';
        exit(EXIT_FAILURE);
    }
}

void Interpreter::scan_end() {
    fclose(yyin);
}
