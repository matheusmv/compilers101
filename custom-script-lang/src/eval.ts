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
  Node,
  Program,
  ReturnStatement,
  Statement,
  UnaryExpression,
  UpdateExpression,
  WhileStatement,
} from './ast.js';
import { Environment } from './environment.js';
import {
  BooleanObject,
  Error,
  FunctionObject,
  Integer,
  Nil,
  ObjectValue,
  ObjectValueType,
  Return,
} from './object.js';
import { TokenType } from './token.js';

const NIL = new Nil();
const TRUE = new BooleanObject(true);
const FALSE = new BooleanObject(false);

export function evalNode(
  node: Node,
  env: Environment<ObjectValue>,
): ObjectValue {
  switch (node?.kind()) {
    case 'ProgramStatement': {
      const program = node as Program;
      return evalProgram(program.stmts, env);
    }
    case 'ExpressionStatement': {
      const expr = node as ExpressionStatement;
      return evalNode(expr.expr, env);
    }
    case 'UnaryExpression': {
      const uExpr = node as UnaryExpression;
      const right = evalNode(uExpr.right, env);
      if (isError(right)) return right;
      return evalUnaryExpression(uExpr.operator, right);
    }
    case 'BinaryExpression': {
      const bExpr = node as BinaryExpression;
      const left = evalNode(bExpr.left, env);
      if (isError(left)) return left;
      const right = evalNode(bExpr.right, env);
      if (isError(right)) return right;
      return evalBinaryExpression(bExpr.operator, left, right);
    }
    case 'BlockStatement': {
      const block = node as BlockStatement;
      return evalBlockStatement(block, env);
    }
    case 'LetStatement': {
      const letStmt = node as LetStatement;
      const value = evalNode(letStmt.value, env);
      if (isError(value)) return value;
      if (!env.exists(letStmt.name.value)) {
        env.setValue(letStmt.name.value, value);
        return value;
      }
      return newError(`${letStmt.name.value} already defined`);
    }
    case 'IfExpression': {
      const ifExpr = node as IfExpression;
      return evalIfExpression(ifExpr, env);
    }
    case 'ReturnStatement': {
      const retStmt = node as ReturnStatement;
      const value = evalNode(retStmt.returnValue, env);
      if (isError(value)) return value;
      return new Return(value);
    }
    case 'Identifier': {
      const ident = node as Identifier;
      return evalIdentifier(ident, env);
    }
    case 'IntegerLiteral': {
      const intLiteral = node as IntegerLiteral;
      return new Integer(intLiteral.value);
    }
    case 'BooleanLiteral': {
      const boolLiteral = node as BooleanLiteral;
      return nativeBoolToBooleanObject(boolLiteral.value);
    }
    case 'NilLiteral': {
      return NIL;
    }
    case 'FunctionStatement': {
      const funcStmt = node as FunctionStatement;
      const name = funcStmt.identifier;
      if (!env.exists(name.value)) {
        const params = funcStmt.params;
        const body = funcStmt.body;
        return env.setValue(name.value, new FunctionObject(params, body, env));
      }
      return newError(`${name.value} already defined`);
    }
    case 'FunctionExpression': {
      const funcExpr = node as FunctionExpression;
      const params = funcExpr.params;
      const body = funcExpr.body;
      return new FunctionObject(params, body, env);
    }
    case 'CallExpression': {
      const callExpr = node as CallExpression;
      const func = evalNode(callExpr.func, env);
      if (isError(func)) return func;
      const args = evalExpressions(callExpr.args, env);
      if (args.length === 1 && isError(args[0])) return args[0];
      return applyFunction(func, args);
    }
    case 'WhileStatement': {
      const whileStmt = node as WhileStatement;
      return evalWhileStatement(whileStmt, env);
    }
    case 'ForStatement': {
      const forStmt = node as ForStatement;
      const err = evalForStatement(forStmt, env);
      if (isError(err)) return err;
      return NIL;
    }
    case 'AssignExpression': {
      const assignExpr = node as AssignExpression;
      const result = evalAssignExpression(assignExpr, env);
      if (isError(result)) return result;
      return result;
    }
    case 'UpdateExpression': {
      const updateExpr = node as UpdateExpression;
      return evalUpdateExpression(updateExpr, env);
    }
  }
}

// TODO: this can be coded in a better way, please refactor this garbage
function evalUpdateExpression(
  updateExpr: UpdateExpression,
  env: Environment<ObjectValue>,
): ObjectValue {
  const oldValue = evalNode(updateExpr.ident, env);

  switch (updateExpr.token.literal) {
    case '++': {
      const exprResult = evalNode(
        new AssignExpression(
          { type: TokenType.ASSIGN, literal: '=' },
          updateExpr.ident,
          new BinaryExpression(
            { type: TokenType.ADD, literal: '+' },
            updateExpr.ident,
            '+',
            new IntegerLiteral({ type: TokenType.INT, literal: 'INT' }, 1),
          ),
        ),
        env,
      );
      if (isError(exprResult)) return exprResult;
      return oldValue;
    }
    case '--': {
      const exprResult = evalNode(
        new AssignExpression(
          { type: TokenType.ASSIGN, literal: '=' },
          updateExpr.ident,
          new BinaryExpression(
            { type: TokenType.ADD, literal: '-' },
            updateExpr.ident,
            '-',
            new IntegerLiteral({ type: TokenType.INT, literal: 'INT' }, 1),
          ),
        ),
        env,
      );
      if (isError(exprResult)) return exprResult;
      return oldValue;
    }
    default:
      return newError(`unknown operator: ${updateExpr.token.literal}`);
  }
}

function evalAssignExpression(
  expr: AssignExpression,
  env: Environment<ObjectValue>,
): ObjectValue {
  if (expr.name.kind() !== 'Identifier') {
    return newError(`not a valid identifier: ${expr.name}`);
  }

  const ident = expr.name as Identifier;

  if (!env.getValue(ident.token.literal)) {
    return newError(`identifier not found: ${expr.name.toString()}`);
  }

  const result = evalNode(expr.value, env);
  if (isError(result)) return result;

  const obj = env.assignValue(expr.name.toString(), result);
  if (!obj) {
    return newError(`identifier not found: ${expr.name.toString()}`);
  }

  return obj;
}

function applyFunction(fn: ObjectValue, args: ObjectValue[]): ObjectValue {
  if (fn.type() !== ObjectValueType.FUNCTION) {
    return newError(`not a function: ${fn.type()}`);
  }

  const func = fn as FunctionObject;

  const innerEnv = extendFunctionEnv(func, args);
  const evaluated = evalNode(func.body, innerEnv);
  return unwrapReturnValue(evaluated);
}

function extendFunctionEnv(
  fn: FunctionObject,
  args: ObjectValue[],
): Environment<ObjectValue> {
  const env = new Environment(fn.env);

  fn.params.forEach((param, index) => env.setValue(param.value, args[index]));

  return env;
}

function unwrapReturnValue(obj: ObjectValue): ObjectValue {
  if (obj.type() === ObjectValueType.RETURN) {
    const returnValue = obj as Return;
    return returnValue.returnValue;
  }

  return obj;
}

function evalExpressions(
  exprs: Expression[],
  env: Environment<ObjectValue>,
): ObjectValue[] {
  const result: ObjectValue[] = [];

  for (const expr of exprs) {
    const evaluated = evalNode(expr, env);
    if (isError(evaluated)) return [evaluated];
    result.push(evaluated);
  }

  return result;
}

function evalProgram(
  stmts: Statement[],
  env: Environment<ObjectValue>,
): ObjectValue {
  let result: ObjectValue;

  for (const stmt of stmts) {
    result = evalNode(stmt, env);

    switch (result.type()) {
      case ObjectValueType.RETURN: {
        const ret = result as Return;
        return ret.returnValue;
      }
      case ObjectValueType.ERROR:
        return result;
    }
  }

  return result;
}

function evalBlockStatement(
  block: BlockStatement,
  env: Environment<ObjectValue>,
): ObjectValue {
  const innerEnv = new Environment(env);
  let result: ObjectValue;

  for (const stmt of block.stmts) {
    result = evalNode(stmt, innerEnv);

    if (result) {
      const returnType = result.type();
      if (
        returnType === ObjectValueType.RETURN ||
        returnType === ObjectValueType.ERROR
      ) {
        return result;
      }
    }
  }

  return result;
}

function evalWhileStatement(
  whileStmt: WhileStatement,
  env: Environment<ObjectValue>,
): ObjectValue {
  while (isTruthy(evalNode(whileStmt.condition, env))) {
    const result = evalBlockStatement(whileStmt.body, env);
    if (result) {
      switch (result.type()) {
        case ObjectValueType.RETURN:
          return NIL;
      }
    }
  }
  return NIL;
}

function evalForStatement(
  forStmt: ForStatement,
  env: Environment<ObjectValue>,
): ObjectValue {
  const innerEnv = new Environment(env);

  if (forStmt.init) {
    switch (forStmt.init.kind()) {
      case 'LetStatement':
        evalNode(forStmt.init as LetStatement, innerEnv);
        break;
      case 'ExpressionStatement': {
        const exprStmt = forStmt.init as ExpressionStatement;
        if (env.getValue(exprStmt.token.literal)) {
          return newError(`identifier not found: ${exprStmt.token.literal}`);
        }
      }
    }
  }

  while (isTruthy(evalNode(forStmt.condition, innerEnv))) {
    const result = evalBlockStatement(forStmt.body, innerEnv);
    if (result) {
      // break, return, goto
      switch (result.type()) {
        case ObjectValueType.RETURN:
          return NIL;
      }
    }

    if (forStmt.update) {
      evalNode(forStmt.update, innerEnv);
    }
  }

  return NIL;
}

function evalUnaryExpression(
  operator: string,
  right: ObjectValue,
): ObjectValue {
  switch (operator) {
    case '!':
      return evalNotOperator(right);
    case '-':
      return evalMinusUnaryOperator(right);
    case '~':
      return evalComplementOperator(right);
    default:
      return newError(`unknown operator: ${operator}${right.type()}`);
  }
}

function evalNotOperator(right: ObjectValue): ObjectValue {
  switch (right) {
    case TRUE:
      return FALSE;
    case FALSE:
      return TRUE;
    case NIL:
      return TRUE;
    default:
      return FALSE;
  }
}

function evalMinusUnaryOperator(right: ObjectValue): ObjectValue {
  if (right.type() === ObjectValueType.INTEGER) {
    const value = right as Integer;
    return new Integer(-value.value);
  }

  return newError(`unknown operator: -${right.type()}`);
}

function evalComplementOperator(right: ObjectValue): ObjectValue {
  if (right.type() === ObjectValueType.INTEGER) {
    const value = right as Integer;
    return new Integer(~value.value);
  }

  return newError(`unknown operator: ~${right.type()}`);
}

function evalBinaryExpression(
  operator: string,
  left: ObjectValue,
  right: ObjectValue,
): ObjectValue {
  if (
    left.type() === ObjectValueType.INTEGER &&
    right.type() === ObjectValueType.INTEGER
  ) {
    return evalIntegerBinaryExpression(operator, left, right);
  }

  if (
    left.type() === ObjectValueType.BOOLEAN &&
    right.type() === ObjectValueType.BOOLEAN
  ) {
    const lBool = (left as BooleanObject).value;
    const rBool = (right as BooleanObject).value;
    switch (operator) {
      case '&&':
        return nativeBoolToBooleanObject(lBool && rBool);
      case '||':
        return nativeBoolToBooleanObject(lBool || rBool);
    }
  }

  if (operator === '==') return nativeBoolToBooleanObject(left === right);
  if (operator === '!=') return nativeBoolToBooleanObject(left !== right);

  if (left.type() !== right.type())
    return newError(
      `type mismatch: ${left.type()} ${operator} ${right.type()}`,
    );

  return newError(
    `unknown operator: ${left.type()} ${operator} ${right.type()}`,
  );
}

function evalIntegerBinaryExpression(
  operator: string,
  left: ObjectValue,
  right: ObjectValue,
): ObjectValue {
  const lInt = (left as Integer).value;
  const rInt = (right as Integer).value;

  switch (operator) {
    case '+':
      return new Integer(lInt + rInt);
    case '-':
      return new Integer(lInt - rInt);
    case '*':
      return new Integer(lInt * rInt);
    case '/':
      return new Integer(lInt / rInt);
    case '%':
      return new Integer(lInt % rInt);
    case '&':
      return new Integer(lInt & rInt);
    case '|':
      return new Integer(lInt | rInt);
    case '^':
      return new Integer(lInt ^ rInt);
    case '<<':
      return new Integer(lInt << rInt);
    case '>>':
      return new Integer(lInt >> rInt);
    case '<':
      return nativeBoolToBooleanObject(lInt < rInt);
    case '>':
      return nativeBoolToBooleanObject(lInt > rInt);
    case '==':
      return nativeBoolToBooleanObject(lInt === rInt);
    case '!=':
      return nativeBoolToBooleanObject(lInt !== rInt);
    case '<=':
      return nativeBoolToBooleanObject(lInt <= rInt);
    case '>=':
      return nativeBoolToBooleanObject(lInt >= rInt);
    default:
      return newError(
        `unknown operator: ${left.type()} ${operator} ${right.type()}`,
      );
  }
}

function nativeBoolToBooleanObject(input: boolean): BooleanObject {
  if (input) return TRUE;
  return FALSE;
}

function evalIfExpression(
  ifExpr: IfExpression,
  env: Environment<ObjectValue>,
): ObjectValue {
  const condition = evalNode(ifExpr.condition, env);
  if (isError(condition)) return condition;

  const innerEnv = new Environment(env);
  if (isTruthy(condition)) {
    return evalNode(ifExpr.consequence, innerEnv);
  } else if (ifExpr.alternative) {
    return evalNode(ifExpr.alternative, innerEnv);
  } else {
    return NIL;
  }
}

function evalIdentifier(
  ident: Identifier,
  env: Environment<ObjectValue>,
): ObjectValue {
  const value = env.getValue(ident.value);
  if (!value) return newError(`identifier not found: ${ident.value}`);
  return value;
}

function isTruthy(obj: ObjectValue): boolean {
  switch (obj) {
    case NIL:
      return false;
    case TRUE:
      return true;
    case FALSE:
      return false;
    default:
      return true;
  }
}

function newError(message: string): Error {
  return new Error(message);
}

function isError(obj: ObjectValue): boolean {
  if (obj) {
    return obj.type() === ObjectValueType.ERROR;
  }

  return false;
}
