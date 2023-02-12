import { readFileSync } from 'fs';
import { argv, exit } from 'process';
import { Scanner } from './lexer.js';

const filePath: string = argv[2];
if (!filePath) {
  console.error('fatal error: no input files');
  exit(1);
}

const source: string = readFileSync(filePath, 'utf-8');

const scanner = new Scanner(source);
const tokens = scanner.tokenize();

for (const { type, lexeme, literal, line } of tokens) {
  console.log(
    `Type: ${type} Lexeme: ${lexeme} Literal: ${literal} Line: ${line}`,
  );
}
