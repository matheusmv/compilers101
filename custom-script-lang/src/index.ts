import { readFileSync } from 'fs';
import { argv, exit } from 'process';
import { Lexer } from './lexer.js';
import { Parser } from './parser.js';

const filePath: string = argv[2];
if (!filePath) {
  console.error('fatal error: no input files');
  exit(1);
}

const source = readFileSync(filePath, 'utf-8');
const lexer = new Lexer(source);
const parser = new Parser(lexer);
const program = parser.parse();

if (parser.errors.length > 0) {
  console.log(parser.getErrors());
} else {
  for (const stmt of program.stmts) {
    console.log(stmt.toString());
    console.log(stmt);
  }
}
