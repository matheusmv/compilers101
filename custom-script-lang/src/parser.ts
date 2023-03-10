import {
  AssignExpression,
  BinaryExpression,
  BlockStatement,
  BooleanLiteral,
  CallExpression,
  Expression,
  ExpressionStatement,
  ForStatement,
  FunctionExpression,
  FunctionStatement,
  Identifier,
  IfExpression,
  IntegerLiteral,
  LetStatement,
  NilLiteral,
  Program,
  ReturnStatement,
  Statement,
  UnaryExpression,
  UpdateExpression,
  WhileStatement,
} from './ast.js';
import { Lexer } from './lexer.js';
import { Token, TokenType } from './token.js';

export class Parser {
  errors: string[] = [];

  curToken: Token;
  peekToken: Token;

  constructor(public readonly lexer: Lexer) {
    this.nextToken();
    this.nextToken();
  }

  getErrors(): string[] {
    return this.errors;
  }

  parse(): Program {
    const program = new Program([]);

    while (!this.curTokenIs(TokenType.EOF)) {
      const stmt = parseStatement(this);
      if (stmt) {
        program.stmts.push(stmt);
      }
      this.nextToken();
    }

    return program;
  }

  peekError(type: TokenType): void {
    const msg = `expected next token to be ${type}, got ${this.peekToken.type} instead`;
    this.errors.push(msg);
  }

  peekPrecedence(): Precedence {
    if (precedences.has(this.peekToken.type)) {
      return precedences.get(this.peekToken.type);
    }

    return Precedence.LOWEST;
  }

  curPrecedence(): Precedence {
    if (precedences.has(this.curToken.type)) {
      return precedences.get(this.curToken.type);
    }

    return Precedence.LOWEST;
  }

  noUnaryExprHandlerError(type: TokenType): void {
    const msg = `no unary handler function for ${type} found`;
    this.errors.push(msg);
  }

  noBinaryExprHandlerError(type: TokenType): void {
    const msg = `no binary handler function for ${type} found`;
    this.errors.push(msg);
  }

  nextToken(): void {
    this.curToken = this.peekToken;
    this.peekToken = this.lexer.nextToken();
  }

  curTokenIs(type: TokenType): boolean {
    return this.curToken.type === type;
  }

  peekTokenIs(type: TokenType): boolean {
    return this.peekToken.type === type;
  }

  expectPeek(type: TokenType): boolean {
    if (this.peekTokenIs(type)) {
      this.nextToken();
      return true;
    }

    this.peekError(type);

    return false;
  }
}

enum Precedence {
  LOWEST = 1,
  LOR,
  LAND,
  COND,
  BIN,
  SUM,
  PROD,
  UNARY,
  UPDT,
  ASSIGN,
  CALL,
}

const precedences: Map<TokenType, Precedence> = new Map([
  [TokenType.LOR, Precedence.LOR],
  [TokenType.LAND, Precedence.LAND],

  [TokenType.EQL, Precedence.COND],
  [TokenType.NEQ, Precedence.COND],
  [TokenType.LEQ, Precedence.COND],
  [TokenType.LSS, Precedence.COND],
  [TokenType.GEQ, Precedence.COND],
  [TokenType.GTR, Precedence.COND],

  [TokenType.SHL, Precedence.BIN],
  [TokenType.OR, Precedence.BIN],
  [TokenType.XOR, Precedence.BIN],
  [TokenType.SHR, Precedence.BIN],
  [TokenType.AND, Precedence.BIN],
  [TokenType.CPLMT, Precedence.BIN],

  [TokenType.ADD, Precedence.SUM],
  [TokenType.SUB, Precedence.SUM],

  [TokenType.MUL, Precedence.PROD],
  [TokenType.QUO, Precedence.PROD],
  [TokenType.REM, Precedence.PROD],

  [TokenType.INC, Precedence.UPDT],
  [TokenType.DEC, Precedence.UPDT],

  [TokenType.ADD_ASSIGN, Precedence.ASSIGN],
  [TokenType.SUB_ASSIGN, Precedence.ASSIGN],
  [TokenType.MUL_ASSIGN, Precedence.ASSIGN],
  [TokenType.QUO_ASSIGN, Precedence.ASSIGN],
  [TokenType.REM_ASSIGN, Precedence.ASSIGN],
  [TokenType.AND_ASSIGN, Precedence.ASSIGN],
  [TokenType.OR_ASSIGN, Precedence.ASSIGN],
  [TokenType.XOR_ASSIGN, Precedence.ASSIGN],
  [TokenType.SHL_ASSIGN, Precedence.ASSIGN],
  [TokenType.SHR_ASSIGN, Precedence.ASSIGN],
  [TokenType.ASSIGN, Precedence.ASSIGN],

  [TokenType.LPAREN, Precedence.CALL],
]);

type UnaryExpressionHandler = (p: Parser) => Expression;
const unaryExprHandler: Map<TokenType, UnaryExpressionHandler> = new Map([
  [TokenType.IDENT, parseIdentifier],
  [TokenType.INT, parseInteger],
  [TokenType.TRUE, parseBoolean],
  [TokenType.FALSE, parseBoolean],
  [TokenType.NIL, parseNil],
  [TokenType.LPAREN, parseGroupedExpression],
  [TokenType.NOT, parseUnaryExpression],
  [TokenType.SUB, parseUnaryExpression],
  [TokenType.CPLMT, parseUnaryExpression],
  [TokenType.IF, parseIfExpression],
  [TokenType.FUNCTION, parseFunctionExpression],
]);

type BinaryExpressionHandler = (p: Parser, other: Expression) => Expression;
const binaryExprHander: Map<TokenType, BinaryExpressionHandler> = new Map([
  [TokenType.ADD, parseBinaryExpression],
  [TokenType.SUB, parseBinaryExpression],
  [TokenType.MUL, parseBinaryExpression],
  [TokenType.QUO, parseBinaryExpression],
  [TokenType.REM, parseBinaryExpression],
  [TokenType.LAND, parseBinaryExpression],
  [TokenType.LOR, parseBinaryExpression],
  [TokenType.AND, parseBinaryExpression],
  [TokenType.OR, parseBinaryExpression],
  [TokenType.XOR, parseBinaryExpression],
  [TokenType.SHL, parseBinaryExpression],
  [TokenType.SHR, parseBinaryExpression],
  [TokenType.EQL, parseBinaryExpression],
  [TokenType.NEQ, parseBinaryExpression],
  [TokenType.LEQ, parseBinaryExpression],
  [TokenType.LSS, parseBinaryExpression],
  [TokenType.GEQ, parseBinaryExpression],
  [TokenType.GTR, parseBinaryExpression],

  [TokenType.INC, parseUpdateExpression],
  [TokenType.DEC, parseUpdateExpression],

  [TokenType.ADD_ASSIGN, parseAssignExpression],
  [TokenType.SUB_ASSIGN, parseAssignExpression],
  [TokenType.MUL_ASSIGN, parseAssignExpression],
  [TokenType.QUO_ASSIGN, parseAssignExpression],
  [TokenType.REM_ASSIGN, parseAssignExpression],
  [TokenType.AND_ASSIGN, parseAssignExpression],
  [TokenType.OR_ASSIGN, parseAssignExpression],
  [TokenType.XOR_ASSIGN, parseAssignExpression],
  [TokenType.SHL_ASSIGN, parseAssignExpression],
  [TokenType.SHR_ASSIGN, parseAssignExpression],
  [TokenType.ASSIGN, parseAssignExpression],
  [TokenType.LPAREN, parseCallExpression],
]);

function parseStatement(p: Parser): Statement {
  switch (p.curToken.type) {
    case TokenType.LET:
      return parseLetStatement(p);
    case TokenType.LBRACE:
      return parseBlockStatement(p);
    case TokenType.RETURN:
      return parseReturnStatement(p);
    case TokenType.WHILE:
      return parseWhileStatement(p);
    case TokenType.FOR:
      return parseForStatement(p);
    case TokenType.FUNCTION:
      return parseFunctionStatement(p);
    default:
      return parseExpressionStatement(p);
  }
}

function parseLetStatement(p: Parser): LetStatement {
  const token = p.curToken;

  if (!p.expectPeek(TokenType.IDENT)) {
    return null;
  }

  const name = new Identifier(p.curToken, p.curToken.literal);

  if (!p.expectPeek(TokenType.ASSIGN)) {
    return null;
  }

  p.nextToken();

  const value = parseExpression(p, Precedence.LOWEST);

  if (p.peekTokenIs(TokenType.SEMICOLON)) {
    p.nextToken();
  }

  return new LetStatement(token, name, value);
}

function parseBlockStatement(p: Parser): BlockStatement {
  const token = p.curToken;
  const stmts: Statement[] = [];

  p.nextToken();

  while (!p.curTokenIs(TokenType.RBRACE) && !p.curTokenIs(TokenType.EOF)) {
    const stmt = parseStatement(p);
    if (stmt) {
      stmts.push(stmt);
    }
    p.nextToken();
  }

  return new BlockStatement(token, stmts);
}

function parseReturnStatement(p: Parser): ReturnStatement {
  const token = p.curToken;

  p.nextToken();

  let returnValue: Expression = null;
  if (!p.curTokenIs(TokenType.SEMICOLON)) {
    returnValue = parseExpression(p, Precedence.LOWEST);
  }

  if (p.peekTokenIs(TokenType.SEMICOLON)) p.nextToken();

  return new ReturnStatement(token, returnValue);
}

function parseWhileStatement(p: Parser): WhileStatement {
  const token = p.curToken;

  if (!p.expectPeek(TokenType.LPAREN)) {
    return null;
  }

  if (p.peekTokenIs(TokenType.RPAREN)) {
    const msg = `expected a conditional expression before ')'`;
    p.errors.push(msg);
    return null;
  }

  const condition = parseExpression(p, Precedence.LOWEST);

  if (!p.expectPeek(TokenType.LBRACE)) {
    return null;
  }

  const body = parseBlockStatement(p);

  return new WhileStatement(token, condition, body);
}

function parseForStatement(p: Parser): ForStatement {
  const token = p.curToken;

  if (!p.expectPeek(TokenType.LPAREN)) {
    return null;
  }

  let init: ExpressionStatement;
  if (p.peekTokenIs(TokenType.SEMICOLON)) {
    p.nextToken();
    init = null;
  } else if (p.peekTokenIs(TokenType.LET)) {
    p.nextToken();
    init = parseLetStatement(p);
  } else {
    p.nextToken();
    init = parseExpressionStatement(p);
  }

  let condition: Expression;
  if (p.peekTokenIs(TokenType.SEMICOLON)) {
    p.nextToken();
    condition = null;
  } else {
    p.nextToken();
    condition = parseExpression(p, Precedence.LOWEST);
    p.nextToken();
  }

  let update: ExpressionStatement;
  if (!p.peekTokenIs(TokenType.RPAREN)) {
    p.nextToken();
    update = parseExpressionStatement(p);
  } else {
    update = null;
  }

  if (!p.expectPeek(TokenType.RPAREN)) {
    return null;
  }

  if (!p.expectPeek(TokenType.LBRACE)) {
    return null;
  }

  const body = parseBlockStatement(p);

  return new ForStatement(token, init, condition, update, body);
}

function parseFunctionStatement(p: Parser): Statement {
  if (p.curTokenIs(TokenType.FUNCTION) && !p.peekTokenIs(TokenType.IDENT)) {
    return parseExpressionStatement(p);
  }

  const token = p.curToken;

  if (!p.expectPeek(TokenType.IDENT)) {
    return null;
  }

  const ident = new Identifier(p.curToken, p.curToken.literal);

  if (!p.expectPeek(TokenType.LPAREN)) {
    return null;
  }

  const params = parseFunctionParameters(p);

  if (!p.expectPeek(TokenType.LBRACE)) {
    return null;
  }

  const body = parseBlockStatement(p);

  return new FunctionStatement(token, ident, params, body);
}

function parseExpressionStatement(p: Parser): ExpressionStatement {
  const token = p.curToken;
  const expr = parseExpression(p, Precedence.LOWEST);

  if (p.peekTokenIs(TokenType.SEMICOLON)) p.nextToken();

  return new ExpressionStatement(token, expr);
}

function parseExpression(p: Parser, precedence: Precedence): Expression {
  const unaryHandler = unaryExprHandler.get(p.curToken.type);
  if (!unaryHandler) {
    p.noUnaryExprHandlerError(p.curToken.type);
    return null;
  }

  let leftExpr = unaryHandler(p);

  while (
    !p.peekTokenIs(TokenType.SEMICOLON) &&
    precedence < p.peekPrecedence()
  ) {
    const binaryHandler = binaryExprHander.get(p.peekToken.type);
    if (!binaryHandler) {
      p.noBinaryExprHandlerError(p.peekToken.type);
      return leftExpr;
    }

    p.nextToken();

    leftExpr = binaryHandler(p, leftExpr);
  }

  return leftExpr;
}

function parseUnaryExpression(p: Parser): Expression {
  const token = p.curToken;
  p.nextToken();
  const rightExpr = parseExpression(p, Precedence.UNARY);
  return new UnaryExpression(token, token.literal, rightExpr);
}

function parseBinaryExpression(p: Parser, left: Expression): Expression {
  const token = p.curToken;
  const precedence = p.curPrecedence();
  p.nextToken();
  const rightExpr = parseExpression(p, precedence);
  return new BinaryExpression(token, left, token.literal, rightExpr);
}

function parseGroupedExpression(p: Parser): Expression {
  p.nextToken();

  const expr = parseExpression(p, Precedence.LOWEST);
  if (!p.expectPeek(TokenType.RPAREN)) {
    return null;
  }

  return expr;
}

function parseIdentifier(p: Parser): Expression {
  return new Identifier(p.curToken, p.curToken.literal);
}

function parseInteger(p: Parser): Expression {
  try {
    const token = p.curToken;
    const value = Number(p.curToken.literal);
    return new IntegerLiteral(token, value);
  } catch (err) {
    const msg = `could not parse ${p.curToken.literal} as integer`;
    p.errors.push(msg);
    return null;
  }
}

function parseBoolean(p: Parser): Expression {
  return new BooleanLiteral(p.curToken, p.curTokenIs(TokenType.TRUE));
}

function parseNil(p: Parser): Expression {
  return new NilLiteral(p.curToken);
}

function parseAssignExpression(p: Parser, ident: Expression): Expression {
  const buildAssignOpExpr = (
    token: Token,
    ident: Expression,
    opToken: Token,
    left: Expression,
    right: Expression,
  ) => {
    return new AssignExpression(
      token,
      ident,
      new BinaryExpression(opToken, left, opToken.literal, right),
    );
  };

  const token = p.curToken;

  switch (token.type) {
    case TokenType.ADD_ASSIGN: {
      p.nextToken();
      const rExpr = parseExpression(p, Precedence.LOWEST);
      const sumToken: Token = { type: TokenType.ADD, literal: '+' };
      return buildAssignOpExpr(token, ident, sumToken, ident, rExpr);
    }
    case TokenType.SUB_ASSIGN: {
      p.nextToken();
      const rExpr = parseExpression(p, Precedence.LOWEST);
      const subToken: Token = { type: TokenType.SUB, literal: '-' };
      return buildAssignOpExpr(token, ident, subToken, ident, rExpr);
    }
    case TokenType.MUL_ASSIGN: {
      p.nextToken();
      const rExpr = parseExpression(p, Precedence.LOWEST);
      const mulToken: Token = { type: TokenType.MUL, literal: '*' };
      return buildAssignOpExpr(token, ident, mulToken, ident, rExpr);
    }
    case TokenType.QUO_ASSIGN: {
      p.nextToken();
      const rExpr = parseExpression(p, Precedence.LOWEST);
      const quoToken: Token = { type: TokenType.QUO, literal: '/' };
      return buildAssignOpExpr(token, ident, quoToken, ident, rExpr);
    }
    case TokenType.REM_ASSIGN: {
      p.nextToken();
      const rExpr = parseExpression(p, Precedence.LOWEST);
      const remToken: Token = { type: TokenType.REM, literal: '%' };
      return buildAssignOpExpr(token, ident, remToken, ident, rExpr);
    }
    case TokenType.AND_ASSIGN: {
      p.nextToken();
      const rExpr = parseExpression(p, Precedence.LOWEST);
      const andToken: Token = { type: TokenType.AND, literal: '&' };
      return buildAssignOpExpr(token, ident, andToken, ident, rExpr);
    }
    case TokenType.OR_ASSIGN: {
      p.nextToken();
      const rExpr = parseExpression(p, Precedence.LOWEST);
      const orToken: Token = { type: TokenType.OR, literal: '|' };
      return buildAssignOpExpr(token, ident, orToken, ident, rExpr);
    }
    case TokenType.XOR_ASSIGN: {
      p.nextToken();
      const rExpr = parseExpression(p, Precedence.LOWEST);
      const xorToken: Token = { type: TokenType.XOR, literal: '^' };
      return buildAssignOpExpr(token, ident, xorToken, ident, rExpr);
    }
    case TokenType.SHL_ASSIGN: {
      p.nextToken();
      const rExpr = parseExpression(p, Precedence.LOWEST);
      const shlToken: Token = { type: TokenType.SHL, literal: '<<' };
      return buildAssignOpExpr(token, ident, shlToken, ident, rExpr);
    }
    case TokenType.SHR_ASSIGN: {
      p.nextToken();
      const rExpr = parseExpression(p, Precedence.LOWEST);
      const shrToken: Token = { type: TokenType.SHL, literal: '>>' };
      return buildAssignOpExpr(token, ident, shrToken, ident, rExpr);
    }
    default: {
      p.nextToken();
      const value = parseExpression(p, Precedence.LOWEST);
      return new AssignExpression(token, ident, value);
    }
  }
}

function parseUpdateExpression(p: Parser, ident: Expression): UpdateExpression {
  const token = p.curToken;

  if (ident?.kind() !== 'Identifier') {
    p.errors.push(
      `invalid left-hand side expression in postfix operation: ${ident?.toString()}${
        token.literal
      }`,
    );
    return null;
  }

  switch (token.type) {
    case TokenType.INC:
      return new UpdateExpression(token, ident);
    case TokenType.DEC:
      return new UpdateExpression(token, ident);
    default:
      p.errors.push(
        `unknown postfix operation: ${ident?.toString()}${token.literal}`,
      );
      return null;
  }
}

function parseIfExpression(p: Parser): Expression {
  const token = p.curToken;

  if (!p.expectPeek(TokenType.LPAREN)) {
    return null;
  }

  p.nextToken();

  const condition = parseExpression(p, Precedence.LOWEST);

  if (!p.expectPeek(TokenType.RPAREN) || !p.expectPeek(TokenType.LBRACE)) {
    return null;
  }

  const consequence: BlockStatement = parseBlockStatement(p);

  let alternative: BlockStatement = null;
  if (p.peekTokenIs(TokenType.ELSE)) {
    p.nextToken();

    if (!p.expectPeek(TokenType.LBRACE)) {
      return null;
    }

    alternative = parseBlockStatement(p);
  }

  return new IfExpression(token, condition, consequence, alternative);
}

function parseCallExpression(p: Parser, func: Expression): Expression {
  const parseCallArguments = (): Expression[] => {
    const exprs: Expression[] = [];

    if (p.peekTokenIs(TokenType.RPAREN)) {
      p.nextToken();
      return exprs;
    }

    p.nextToken();
    exprs.push(parseExpression(p, Precedence.LOWEST));

    while (p.peekTokenIs(TokenType.COMMA)) {
      p.nextToken();
      p.nextToken();
      exprs.push(parseExpression(p, Precedence.LOWEST));
    }

    if (!p.expectPeek(TokenType.RPAREN)) {
      return null;
    }

    if (p.curTokenIs(TokenType.SEMICOLON)) {
      p.nextToken();
    }

    return exprs;
  };

  return new CallExpression(p.curToken, func, parseCallArguments());
}

function parseFunctionExpression(p: Parser): Expression {
  const token = p.curToken;

  if (!p.expectPeek(TokenType.LPAREN)) {
    return null;
  }

  const params = parseFunctionParameters(p);

  if (!p.expectPeek(TokenType.LBRACE)) {
    return null;
  }

  const body = parseBlockStatement(p);

  return new FunctionExpression(token, params, body);
}

function parseFunctionParameters(p: Parser): Identifier[] {
  const args: Identifier[] = [];

  if (p.peekTokenIs(TokenType.RPAREN)) {
    p.nextToken();
    return args;
  }

  p.nextToken();
  let ident = new Identifier(p.curToken, p.curToken.literal);
  args.push(ident);

  while (p.peekTokenIs(TokenType.COMMA)) {
    p.nextToken();
    p.nextToken();
    ident = new Identifier(p.curToken, p.curToken.literal);
    args.push(ident);
  }

  if (!p.expectPeek(TokenType.RPAREN)) {
    return null;
  }

  return args;
}
