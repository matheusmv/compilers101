import { Token } from './token.js';

interface Node {
  tokenLiteral(): string;
  toString(): string;
}

type StatementKind =
  | 'ProgramStatement'
  | 'LetStatement'
  | 'BlockStatement'
  | 'ReturnStatement'
  | 'WhileStatement'
  | 'ForStatement'
  | 'FunctionStatement'
  | 'ExpressionStatement';

export interface Statement extends Node {
  kind: StatementKind;
}

type ExpressionKind =
  | 'Identifier'
  | 'IntegerLiteral'
  | 'BooleanLiteral'
  | 'AssignExpression'
  | 'IfExpression'
  | 'BinaryExpression'
  | 'UnaryExpression'
  | 'CallExpression'
  | 'FunctionExpression';

export interface Expression extends Node {
  kind: ExpressionKind;
}

export class Program implements Statement {
  kind: StatementKind = 'ProgramStatement';

  constructor(public stmts: Statement[]) {}

  tokenLiteral(): string {
    if (this.stmts.length > 0) {
      return this.stmts[0].toString();
    }

    return '';
  }

  toString(): string {
    let out = '';

    for (const stmt of this.stmts) {
      out += `${stmt.toString()}`;
    }

    return out;
  }
}

export class LetStatement implements Statement {
  kind: StatementKind = 'LetStatement';

  constructor(
    public token: Token,
    public name: Identifier,
    public value?: Expression,
  ) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    let out = '';

    out += `${this.tokenLiteral()} ${this.name.toString()}`;
    if (this.value) {
      out += ` = ${this.value.toString()}`;
    }
    out += ';';

    return out;
  }
}

export class BlockStatement implements Statement {
  kind: StatementKind = 'BlockStatement';

  constructor(public token: Token, public stmts: Statement[]) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    let out = '';

    for (const stmt of this.stmts) {
      out += `${stmt.toString()}`;
    }

    return out;
  }
}

export class ReturnStatement implements Statement {
  kind: StatementKind = 'ReturnStatement';

  constructor(public token: Token, public returnValue?: Expression) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    let out = '';

    out += `${this.tokenLiteral()}`;
    if (this.returnValue) {
      out += `${this.returnValue.toString()}`;
    }
    out += ';';

    return out;
  }
}

export class WhileStatement implements Statement {
  kind: StatementKind = 'WhileStatement';

  constructor(
    public token: Token,
    public condition: Expression,
    public body: BlockStatement,
  ) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    let out = '';

    out += `${this.tokenLiteral()} (${
      this.condition.toString
    }) { ${this.body.toString()} }`;

    return out;
  }
}

export class ForStatement implements Statement {
  kind: StatementKind = 'ForStatement';

  constructor(
    public token: Token,
    public init: Statement,
    public condition: Expression,
    public update: ExpressionStatement,
    public body: BlockStatement,
  ) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    let out = '';

    out += `${this.tokenLiteral()} (${this.init.toString()}; ${this.condition.toString()}; ${this.update.toString()}) { ${this.body.toString()} }`;

    return out;
  }
}

export class FunctionStatement implements Statement {
  kind: StatementKind = 'FunctionStatement';

  constructor(
    public token: Token,
    public identifier: Identifier,
    public params: Identifier[],
    public body: BlockStatement,
  ) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    let out = '';

    out += `${this.tokenLiteral()} ${this.identifier.toString()}(${this.params
      .map((param) => param.toString())
      .join(', ')}) ${this.body.toString()}}`;

    return out;
  }
}

export class ExpressionStatement implements Statement {
  kind: StatementKind = 'ExpressionStatement';

  constructor(public token: Token, public expr?: Expression) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    if (this.expr) return this.expr.toString();
    return '';
  }
}

export class Identifier implements Expression {
  kind: ExpressionKind = 'Identifier';

  constructor(public token: Token, public value: string) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    return this.value;
  }
}

export class IntegerLiteral implements Expression {
  kind: ExpressionKind = 'IntegerLiteral';

  constructor(public token: Token, public value: number) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    return this.token.literal;
  }
}

export class BooleanLiteral implements Expression {
  kind: ExpressionKind = 'BooleanLiteral';

  constructor(public token: Token, public value: boolean) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    return this.token.literal;
  }
}

export class AssignExpression implements Expression {
  kind: ExpressionKind = 'AssignExpression';

  constructor(
    public token: Token,
    public name: Expression,
    public value: Expression,
  ) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    let out = '';

    out += `${this.name.toString()} = ${this.value.toString()}`;

    return out;
  }
}

export class IfExpression implements Expression {
  kind: ExpressionKind = 'IfExpression';

  constructor(
    public token: Token,
    public condition: Expression,
    public consequence: BlockStatement,
    public alternative?: BlockStatement,
  ) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    let out = '';

    out += `if ${this.condition.toString()} ${this.consequence.toString()}`;

    if (this.alternative) {
      out += ` else ${this.alternative.toString()}`;
    }

    return out;
  }
}

export class BinaryExpression implements Expression {
  kind: ExpressionKind = 'BinaryExpression';

  constructor(
    public token: Token,
    public left: Expression,
    public operator: string,
    public right: Expression,
  ) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    let out = '';

    out += `(${this.left.toString()} ${
      this.operator
    } ${this.right.toString()})`;

    return out;
  }
}

export class UnaryExpression implements Expression {
  kind: ExpressionKind = 'UnaryExpression';

  constructor(
    public token: Token,
    public operator: string,
    public right: Expression,
  ) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    let out = '';

    out += `(${this.operator} ${this.right.toString()})`;

    return out;
  }
}

export class CallExpression implements Expression {
  kind: ExpressionKind = 'CallExpression';

  constructor(
    public token: Token,
    public func: Expression,
    public args: Expression[],
  ) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    let out = '';

    out += `${this.func.toString()} (${this.args
      .map((arg) => arg.toString())
      .join(', ')})`;

    return out;
  }
}

export class FunctionExpression implements Expression {
  kind: ExpressionKind = 'FunctionExpression';

  constructor(
    public token: Token,
    public params: Identifier[],
    public body: BlockStatement,
  ) {}

  tokenLiteral(): string {
    return this.token.literal;
  }

  toString(): string {
    let out = '';

    out += `${this.tokenLiteral()} (${this.params
      .map((param) => param.toString())
      .join(', ')}) ${this.body.toString()}}`;

    return out;
  }
}
