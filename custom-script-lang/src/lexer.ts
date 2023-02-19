import { lookUpIdent, Token, TokenType } from './token.js';

export class Lexer {
  private position = 0;
  private readPosition = 0;
  private currentLiteral: string;

  constructor(private readonly source: string) {
    this.readChar();
  }

  nextToken(): Token {
    let token: Token = null;

    this.skipWhitespace();

    switch (this.currentLiteral) {
      case '=':
        if (this.peekChar() === '=') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(TokenType.EQL, prev + this.currentLiteral);
        } else {
          token = this.newToken(TokenType.ASSIGN, this.currentLiteral);
        }
        break;
      case '+':
        if (this.peekChar() === '=') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(
            TokenType.ADD_ASSIGN,
            prev + this.currentLiteral,
          );
        } else if (this.peekChar() === '+') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(TokenType.INC, prev + this.currentLiteral);
        } else {
          token = this.newToken(TokenType.ADD, this.currentLiteral);
        }
        break;
      case '-':
        if (this.peekChar() === '=') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(
            TokenType.SUB_ASSIGN,
            prev + this.currentLiteral,
          );
        } else if (this.peekChar() === '-') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(TokenType.DEC, prev + this.currentLiteral);
        } else {
          token = this.newToken(TokenType.SUB, this.currentLiteral);
        }
        break;
      case '*':
        if (this.peekChar() === '=') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(
            TokenType.MUL_ASSIGN,
            prev + this.currentLiteral,
          );
        } else {
          token = this.newToken(TokenType.MUL, this.currentLiteral);
        }
        break;
      case '/':
        if (this.peekChar() === '=') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(
            TokenType.QUO_ASSIGN,
            prev + this.currentLiteral,
          );
        } else {
          token = this.newToken(TokenType.QUO, this.currentLiteral);
        }
        break;
      case '%':
        if (this.peekChar() === '=') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(
            TokenType.REM_ASSIGN,
            prev + this.currentLiteral,
          );
        } else {
          token = this.newToken(TokenType.REM, this.currentLiteral);
        }
        break;
      case '!':
        if (this.peekChar() === '=') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(TokenType.NEQ, prev + this.currentLiteral);
        } else {
          token = this.newToken(TokenType.NOT, this.currentLiteral);
        }
        break;
      case '&':
        if (this.peekChar() === '=') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(
            TokenType.AND_ASSIGN,
            prev + this.currentLiteral,
          );
        } else if (this.peekChar() === '&') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(TokenType.LAND, prev + this.currentLiteral);
        } else {
          token = this.newToken(TokenType.AND, this.currentLiteral);
        }
        break;
      case '|':
        if (this.peekChar() === '=') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(
            TokenType.OR_ASSIGN,
            prev + this.currentLiteral,
          );
        } else if (this.peekChar() === '|') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(TokenType.LOR, prev + this.currentLiteral);
        } else {
          token = this.newToken(TokenType.OR, this.currentLiteral);
        }
        break;
      case '^':
        if (this.peekChar() === '=') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(
            TokenType.XOR_ASSIGN,
            prev + this.currentLiteral,
          );
        } else {
          token = this.newToken(TokenType.XOR, this.currentLiteral);
        }
        break;
      case '<':
        if (this.peekChar() === '<') {
          const prev = this.currentLiteral;
          this.readChar();
          if (this.peekChar() === '=') {
            const end = this.currentLiteral;
            this.readChar();
            token = this.newToken(
              TokenType.SHL_ASSIGN,
              prev + end + this.currentLiteral,
            );
          } else {
            token = this.newToken(TokenType.SHL, prev + this.currentLiteral);
          }
        } else if (this.peekChar() === '=') {
          const prev = this.currentLiteral;
          this.readChar();
          token = this.newToken(TokenType.LEQ, prev + this.currentLiteral);
        } else {
          token = this.newToken(TokenType.LSS, this.currentLiteral);
        }
        break;
      case '>':
        token = this.newToken(TokenType.GTR, this.currentLiteral);
        break;
      case '(':
        token = this.newToken(TokenType.LPAREN, this.currentLiteral);
        break;
      case ')':
        token = this.newToken(TokenType.RPAREN, this.currentLiteral);
        break;
      case '{':
        token = this.newToken(TokenType.LBRACE, this.currentLiteral);
        break;
      case '}':
        token = this.newToken(TokenType.RBRACE, this.currentLiteral);
        break;
      case '[':
        token = this.newToken(TokenType.LBRACK, this.currentLiteral);
        break;
      case ']':
        token = this.newToken(TokenType.RBRACK, this.currentLiteral);
        break;
      case ';':
        token = this.newToken(TokenType.SEMICOLON, this.currentLiteral);
        break;
      case ',':
        token = this.newToken(TokenType.COMMA, this.currentLiteral);
        break;
      case '\0':
        token = this.newToken(TokenType.EOF, '');
        break;
      default:
        if (this.isCharacter(this.currentLiteral)) {
          const ident = this.readIdentifier();
          return this.newToken(lookUpIdent(ident), ident);
        } else if (this.isDigit(this.currentLiteral)) {
          return this.newToken(TokenType.INT, this.readNumber());
        } else {
          token = this.newToken(TokenType.ILLEGAL, this.currentLiteral);
        }
    }

    this.readChar();

    return token;
  }

  private newToken(type: TokenType, literal: string): Token {
    return { type: type, literal: literal };
  }

  private readIdentifier(): string {
    const start = this.position;
    while (this.isCharacter(this.currentLiteral)) {
      this.readChar();
    }
    return this.source.slice(start, this.position);
  }

  private readNumber(): string {
    const start = this.position;
    while (this.isDigit(this.currentLiteral)) {
      this.readChar();
    }
    return this.source.slice(start, this.position);
  }

  private skipWhitespace(): void {
    while (
      this.currentLiteral === ' ' ||
      this.currentLiteral === '\t' ||
      this.currentLiteral === '\n' ||
      this.currentLiteral === '\r'
    ) {
      this.readChar();
    }
  }

  private readChar(): void {
    if (this.readPosition >= this.source.length) {
      this.currentLiteral = '\0';
    } else {
      this.currentLiteral = this.source[this.readPosition];
      this.position = this.readPosition;
      this.readPosition += 1;
    }
  }

  private peekChar(): string {
    if (this.readPosition >= this.source.length) return '\0';
    return this.source[this.readPosition];
  }

  private isCharacter(char: string): boolean {
    return (
      ('a' <= char && char <= 'z') ||
      ('A' <= char && char <= 'Z') ||
      char == '_'
    );
  }

  private isDigit(char: string): boolean {
    return '0' <= char && char <= '9';
  }
}
