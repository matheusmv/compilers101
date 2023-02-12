export enum TokenType {
  ILLEGAL = 'ILLEGAL',
  EOF = 'EOF',
  COMMENT = 'COMMENT',

  IDENT = 'IDENT',
  INT = 'INT',
  FLOAT = 'FLOAT',
  CHAR = 'CHAR',
  STRING = 'STRING',
  BOOL = 'BOOL',

  ADD = '+',
  SUB = '-',
  MUL = '*',
  QUO = '/',
  REM = '%',

  CPLMT = '~',
  AND = '&',
  OR = '|',
  XOR = '^',
  SHL = '<<',
  SHR = '>>',

  ADD_ASSIGN = '+=',
  SUB_ASSIGN = '-=',
  MUL_ASSIGN = '*=',
  QUO_ASSIGN = '/=',
  REM_ASSIGN = '%=',

  AND_ASSIGN = '&=',
  OR_ASSIGN = '|=',
  XOR_ASSIGN = '^=',
  SHL_ASSIGN = '<<=',
  SHR_ASSIGN = '>>=',

  LAND = '&&',
  LOR = '||',
  INC = '++',
  DEC = '--',

  EQL = '==',
  LSS = '<',
  GTR = '>',
  ASSIGN = '=',
  NOT = '!',

  NEQ = '!=',
  LEQ = '<=',
  GEQ = '>=',
  DEFINE = ':=',
  ELLIPSIS = '...',

  LPAREN = '(',
  LBRACK = '[',
  LBRACE = '{',
  COMMA = ',',
  PERIOD = '.',

  RPAREN = ')',
  RBRACK = ']',
  RBRACE = '}',
  SEMICOLON = ';',
  COLON = ':',

  CLASS = 'class',
  ELSE = 'else',
  FOR = 'for',
  FUN = 'fun',
  IF = 'if',
  LEN = 'len',
  NIL = 'nil',
  PRINTLN = 'println',
  RETURN = 'return',
  SUPER = 'super',
  THIS = 'this',
  TYPE_INT = 'int',
  TYPE_FLOAT = 'float',
  TYPE_CHAR = 'char',
  TYPE_STRING = 'string',
  TYPE_BOOL = 'bool',
  VAR = 'var',
  WHILE = 'while',
}

export interface Token {
  type: TokenType;
  lexeme: string;
  literal: any;
  line: number;
}
