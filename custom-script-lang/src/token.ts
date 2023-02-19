export enum TokenType {
  ILLEGAL = 'ILLEGAL', //
  EOF = 'EOF', //
  COMMENT = 'COMMENT',

  IDENT = 'IDENT', //
  INT = 'INT', //
  FLOAT = 'FLOAT',
  CHAR = 'CHAR',
  STRING = 'STRING',

  ADD = '+', //
  SUB = '-', //
  MUL = '*', //
  QUO = '/', //
  REM = '%', //

  CPLMT = '~',
  AND = '&',
  OR = '|',
  XOR = '^',
  SHL = '<<',
  SHR = '>>',

  ADD_ASSIGN = '+=', //
  SUB_ASSIGN = '-=', //
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
  INC = '++', //
  DEC = '--', //

  EQL = '==', //
  LSS = '<', //
  GTR = '>', //
  ASSIGN = '=', //
  NOT = '!', //

  NEQ = '!=',
  LEQ = '<=',
  GEQ = '>=',
  DEFINE = ':=',
  ELLIPSIS = '...',

  LPAREN = '(', //
  LBRACK = '[', //
  LBRACE = '{', //
  COMMA = ',', //
  PERIOD = '.',

  RPAREN = ')', //
  RBRACK = ']', //
  RBRACE = '}', //
  SEMICOLON = ';', //
  COLON = ':',

  FUNCTION = 'FUNCTION', //
  LET = 'LET', //
  TRUE = 'TRUE', //
  FALSE = 'FALSE', //
  IF = 'IF', //
  ELSE = 'ELSE', //
  RETURN = 'RETURN', //
  WHILE = 'WHILE', //
  FOR = 'FOR', //
}

export interface Token {
  literal: string;
  type: TokenType;
}

const keywords: Map<string, TokenType> = new Map([
  ['fn', TokenType.FUNCTION],
  ['let', TokenType.LET],
  ['true', TokenType.TRUE],
  ['false', TokenType.FALSE],
  ['if', TokenType.IF],
  ['else', TokenType.ELSE],
  ['return', TokenType.RETURN],
  ['while', TokenType.WHILE],
  ['for', TokenType.FOR],
]);

export function lookUpIdent(literal: string): TokenType {
  if (keywords.has(literal)) {
    return keywords.get(literal);
  }

  return TokenType.IDENT;
}
