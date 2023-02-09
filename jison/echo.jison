%lex
%%

\s+                     /* skip whitespace */
[0-9]+("."[0-9]+)?\b    return 'NUMBER'
"*"                     return '*'
"/"                     return '/'
"-"                     return '-'
"+"                     return '+'
"^"                     return '^'
"!"                     return '!'
"%"                     return '%'
"("                     return '('
")"                     return ')'
"PI"                    return 'PI'
"E"                     return 'E'
<<EOF>>                 return 'EOF'
.                       return 'INVALID'

/lex

/* operator associations and precedence */

%left '+' '-'
%left '*' '/'
%left '^'
%right '!'
%right '%'
%left UMINUS

%start exprs
%%

exprs
    : expr EOF
        {
            typeof console !== 'undefined' ? console.log($1) : print($1);
        }
    ;

expr
    : expr '+' expr
        {
            $$ = $1 + $3;
        }
    | expr '-' expr
        {
            $$ = $1 - $3;
        }
    | expr '*' expr
        {
            $$ = $1 * $3;
        }
    | expr '/' expr
        {
            $$ = $1 / $3;
        }
    | expr '^' expr
        {
            $$ = Math.pow($1, $3);
        }
    | expr '!'
        {
            $$ = (function (num) {
                let fact = 1;

                for (let i = num; i > 1; i--)
                    fact *= i;

                return fact;
            })($1)
        }
    | expr '%'
        {
            $$ = $1 / 100;
        }
    | '-' expr %prec UMINUS
        {
            $$ = -$2;
        }
    | '(' expr ')'
        {
            $$ = $2;
        }
    | NUMBER
        {
            $$ = Number(yytext);
        }
    | E
        {
            $$ = Math.E;
        }
    | PI
        {
            $$ = Math.PI;
        }
    ;
