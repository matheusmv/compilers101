import { BlockStatement, Identifier } from './ast.js';
import { Environment } from './environment.js';

export enum ObjectValueType {
  INTEGER = 'INTEGER',
  BOOLEAN = 'BOOLEAN',
  NIL = 'NIL',
  RETURN = 'RETURN',
  ERROR = 'ERROR',
  FUNCTION = 'FUNCTION',
}

export interface ObjectValue {
  type(): ObjectValueType;
  inspect(): string;
}

export class Integer implements ObjectValue {
  constructor(public value: number) {}

  type(): ObjectValueType {
    return ObjectValueType.INTEGER;
  }

  inspect(): string {
    return `${this.value}`;
  }
}

export class BooleanObject implements ObjectValue {
  constructor(public value: boolean) {}

  type(): ObjectValueType {
    return ObjectValueType.BOOLEAN;
  }

  inspect(): string {
    return `${this.value}`;
  }
}

export class Nil implements ObjectValue {
  type(): ObjectValueType {
    return ObjectValueType.NIL;
  }

  inspect(): string {
    return 'nil';
  }
}

export class Return implements ObjectValue {
  constructor(public returnValue: ObjectValue) {}

  type(): ObjectValueType {
    return ObjectValueType.RETURN;
  }
  inspect(): string {
    return this.returnValue.inspect();
  }
}

export class Error implements ObjectValue {
  constructor(public message: string) {}

  type(): ObjectValueType {
    return ObjectValueType.ERROR;
  }

  inspect(): string {
    return `ERROR: ${this.message}`;
  }
}

export class FunctionObject implements ObjectValue {
  constructor(
    public params: Identifier[],
    public body: BlockStatement,
    public env: Environment<ObjectValue>,
  ) {}

  type(): ObjectValueType {
    return ObjectValueType.FUNCTION;
  }

  inspect(): string {
    let out = '';

    out += `fn (${this.params.join(', ')} ${this.body.toString()}}\n)`;

    return out;
  }
}
