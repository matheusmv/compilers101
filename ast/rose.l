%option yylineno noyywrap nounput noinput batch

%{

#include "src/utils.h"

#include "rose.tab.h"

#define return_token(tok) return (tok)

%}

WS [ \t\r]
NL \n

SL_COMMENT \/\/(.*)
ML_COMMENT \/\*([^*]|\*+[^*/])*\*+\/

IDENT  _*[a-zA-Z][_a-zA-Z0-9]*
INT    [0-9]+
FLOAT  [0-9]*\.[0-9]+
CHAR   \'([^\\\n]|(\\.))*\'
STRING \"([^"\\]|\\.)*\"

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

COND "?"

LOR  "||"
LAND "&&"

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

{WS}*           { /*  */ }
{NL}            { /*  */ }

"int"           { return_token(TYPE_INT);    }
"float"         { return_token(TYPE_FLOAT);  }
"char"          { return_token(TYPE_CHAR);   }
"string"        { return_token(TYPE_STRING); }
"bool"          { return_token(TYPE_BOOL);   }
"void"          { return_token(TYPE_VOID);   }
"nil"           { return_token(NIL);         }

"let"           { return_token(LET);      }
"const"         { return_token(CONST);    }
"if"            { return_token(IF);       }
"else"          { return_token(ELSE);     }
"func"          { return_token(FUNC);     }
"return"        { return_token(RETURN);   }
"while"         { return_token(WHILE);    }
"for"           { return_token(FOR);      }
"break"         { return_token(BREAK);    }
"continue"      { return_token(CONTINUE); }
"true"          { return_token(TRUE);     }
"false"         { return_token(FALSE);    }
"struct"        { return_token(STRUCT);   }

{ADD}           { return_token(ADD); }
{SUB}           { return_token(SUB); }
{MUL}           { return_token(MUL); }
{QUO}           { return_token(QUO); }
{REM}           { return_token(REM); }

{TILDE}         { return_token(TILDE); }
{AND}           { return_token(AND);   }
{OR}            { return_token(OR);    }
{XOR}           { return_token(XOR);   }
{SHL}           { return_token(SHL);   }
{SHR}           { return_token(SHR);   }

{ADD_ASSIGN}    { return_token(ADD_ASSIGN); }
{SUB_ASSIGN}    { return_token(SUB_ASSIGN); }
{MUL_ASSIGN}    { return_token(MUL_ASSIGN); }
{QUO_ASSIGN}    { return_token(QUO_ASSIGN); }
{REM_ASSIGN}    { return_token(REM_ASSIGN); }

{AND_ASSIGN}    { return_token(AND_ASSIGN); }
{OR_ASSIGN}     { return_token(OR_ASSIGN);  }
{XOR_ASSIGN}    { return_token(XOR_ASSIGN); }
{SHL_ASSIGN}    { return_token(SHL_ASSIGN); }
{SHR_ASSIGN}    { return_token(SHR_ASSIGN); }

{COND}          { return_token(COND); }

{LOR}           { return_token(LOR);  }
{LAND}          { return_token(LAND); }

{INC}           { return_token(INC); }
{DEC}           { return_token(DEC); }

{EQL}           { return_token(EQL);    }
{LSS}           { return_token(LSS);    }
{GTR}           { return_token(GTR);    }
{ASSIGN}        { return_token(ASSIGN); }
{NOT}           { return_token(NOT);    }

{NEQ}           { return_token(NEQ);      }
{LEQ}           { return_token(LEQ);      }
{GEQ}           { return_token(GEQ);      }
{ELLIPSIS}      { return_token(ELLIPSIS); }

{LPAREN}        { return_token(LPAREN); }
{LBRACK}        { return_token(LBRACK); }
{LBRACE}        { return_token(LBRACE); }
{COMMA}         { return_token(COMMA);  }
{PERIOD}        { return_token(PERIOD); }

{RPAREN}        { return_token(RPAREN);    }
{RBRACK}        { return_token(RBRACK);    }
{RBRACE}        { return_token(RBRACE);    }
{SEMICOLON}     { return_token(SEMICOLON); }
{COLON}         { return_token(COLON);     }

{SL_COMMENT}    { /* */ }
{ML_COMMENT}    { /* */ }

{IDENT}         { yylval.str_value   = str_dup(yytext); return_token(IDENT);  }
{INT}           { yylval.int_value   = atoi(yytext);    return_token(INT);    }
{FLOAT}         { yylval.float_value = atof(yytext);    return_token(FLOAT);  }
{CHAR}          { yylval.char_value  = yytext[1];       return_token(CHAR);   }
{STRING}        { yylval.str_value   = str_dup(yytext); return_token(STRING); }

.               { return_token(ILLEGAL); }

<<EOF>>         { return_token(EOF); }

%%
