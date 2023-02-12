import { Token, TokenType } from './token.js';

export class Scanner {
  private readonly source: string;
  private readonly keywords: Map<string, TokenType>;

  private readonly tokens: Token[] = [];

  private start = 0;
  private current = 0;
  private line = 1;

  constructor(source: string) {
    this.source = source;
    this.keywords = new Map([
      ['int', TokenType.TYPE_INT],
      ['float', TokenType.TYPE_FLOAT],
      ['char', TokenType.TYPE_CHAR],
      ['string', TokenType.TYPE_STRING],
      ['bool', TokenType.TYPE_BOOL],
      ['class', TokenType.CLASS],
      ['else', TokenType.ELSE],
      ['false', TokenType.BOOL],
      ['for', TokenType.FOR],
      ['fun', TokenType.FUN],
      ['if', TokenType.IF],
      ['len', TokenType.LEN],
      ['nil', TokenType.NIL],
      ['println', TokenType.PRINTLN],
      ['return', TokenType.RETURN],
      ['super', TokenType.SUPER],
      ['this', TokenType.THIS],
      ['true', TokenType.BOOL],
      ['var', TokenType.VAR],
      ['while', TokenType.WHILE],
    ]);
  }

  tokenize(): Token[] {
    while (!this.isAtEnd()) {
      this.start = this.current;
      this.scanToken();
    }

    this.tokens.push({
      type: TokenType.EOF,
      lexeme: '',
      literal: null,
      line: this.line,
    });

    return this.tokens;
  }

  private isAtEnd(): boolean {
    return this.current >= this.source.length;
  }

  private scanToken(): void {
    const token: string = this.advance();

    if (/[ \r\t]/.test(token)) return;

    if (token === '\n') {
      this.line += 1;
      return;
    }

    switch (token) {
      case TokenType.ADD:
        if (this.match('=')) {
          this.addToken(TokenType.ADD_ASSIGN);
        } else if (this.match('+')) {
          this.addToken(TokenType.INC);
        } else {
          this.addToken(TokenType.ADD);
        }
        break;
      case TokenType.SUB:
        if (this.match('=')) {
          this.addToken(TokenType.SUB_ASSIGN);
        } else if (this.match('-')) {
          this.addToken(TokenType.DEC);
        } else {
          this.addToken(TokenType.SUB);
        }
        break;
      case TokenType.MUL:
        this.addToken(this.match('=') ? TokenType.MUL_ASSIGN : TokenType.MUL);
        break;
      case TokenType.QUO:
        if (this.match('/')) {
          this.comment();
        } else if (this.match('=')) {
          this.addToken(TokenType.QUO_ASSIGN);
        } else {
          this.addToken(TokenType.QUO);
        }
        break;
      case TokenType.REM:
        this.addToken(this.match('=') ? TokenType.REM_ASSIGN : TokenType.REM);
        break;
      case TokenType.CPLMT:
        this.addToken(TokenType.CPLMT);
        break;
      case TokenType.AND:
        if (this.match('=')) {
          this.addToken(TokenType.ADD_ASSIGN);
        } else if (this.match('&')) {
          this.addToken(TokenType.LAND);
        } else {
          this.addToken(TokenType.AND);
        }
        break;
      case TokenType.OR:
        if (this.match('=')) {
          this.addToken(TokenType.OR_ASSIGN);
        } else if (this.match('&')) {
          this.addToken(TokenType.LOR);
        } else {
          this.addToken(TokenType.OR);
        }
        break;
      case TokenType.XOR:
        if (this.match('=')) {
          this.addToken(TokenType.XOR_ASSIGN);
        } else {
          this.addToken(TokenType.XOR);
        }
        break;
      case TokenType.LSS:
        if (this.match('<') && this.match('=')) {
          this.addToken(TokenType.SHL_ASSIGN);
        } else if (this.match('<')) {
          this.addToken(TokenType.SHL);
        } else if (this.match('=')) {
          this.addToken(TokenType.LEQ);
        } else {
          this.addToken(TokenType.LSS);
        }
        break;
      case TokenType.GTR:
        if (this.match('>') && this.match('=')) {
          this.addToken(TokenType.SHR_ASSIGN);
        } else if (this.match('>')) {
          this.addToken(TokenType.SHR);
        } else if (this.match('=')) {
          this.addToken(TokenType.GEQ);
        } else {
          this.addToken(TokenType.GTR);
        }
        break;
      case TokenType.NOT:
        if (this.match('=')) {
          this.addToken(TokenType.NEQ);
        } else {
          this.addToken(TokenType.NOT);
        }
        break;
      case TokenType.ASSIGN:
        if (this.match('=')) {
          this.addToken(TokenType.EQL);
        } else {
          this.addToken(TokenType.ASSIGN);
        }
        break;
      case TokenType.COLON:
        if (this.match('=')) {
          this.addToken(TokenType.DEFINE);
        } else {
          this.addToken(TokenType.COLON);
        }
        break;
      case TokenType.PERIOD:
        if (this.match('.')) {
          const type = this.match('.') ? TokenType.ELLIPSIS : TokenType.ILLEGAL;
          if (type === TokenType.ILLEGAL) {
            console.error(
              `line: ${this.line}, Error: Unexpected character '${token}'.`,
            );
          }
          this.addToken(type);
        } else {
          this.addToken(TokenType.PERIOD);
        }
        break;
      case TokenType.LPAREN:
        this.addToken(TokenType.LPAREN);
        break;
      case TokenType.LBRACK:
        this.addToken(TokenType.LBRACK);
        break;
      case TokenType.LBRACE:
        this.addToken(TokenType.LBRACE);
        break;
      case TokenType.COMMA:
        this.addToken(TokenType.COMMA);
        break;
      case TokenType.RPAREN:
        this.addToken(TokenType.RPAREN);
        break;
      case TokenType.RBRACK:
        this.addToken(TokenType.RBRACK);
        break;
      case TokenType.RBRACE:
        this.addToken(TokenType.RBRACE);
        break;
      case TokenType.SEMICOLON:
        this.addToken(TokenType.SEMICOLON);
        break;
      case "'":
        this.character();
        break;
      case '"':
        this.string();
        break;
      default:
        if (this.isDigit(token)) {
          this.number();
        } else if (this.isAlpha(token)) {
          this.identifier();
        } else {
          this.addTokenWithLiteral(TokenType.ILLEGAL, token);
          console.error(
            `line: ${this.line}, Error: Unexpected character '${token}'.`,
          );
        }
    }
  }

  private advance(): string {
    this.current += 1;
    return this.source.charAt(this.current - 1);
  }

  private addToken(type: TokenType): void {
    this.addTokenWithLiteral(type, null);
  }

  private addTokenWithLiteral(type: TokenType, literal: any): void {
    const lexeme = this.source.substring(this.start, this.current);
    this.tokens.push({
      type: type,
      lexeme: lexeme,
      literal: literal,
      line: this.line,
    });
  }

  private match(expected: string): boolean {
    if (this.isAtEnd()) return false;
    if (this.source.charAt(this.current) !== expected) return false;

    this.current += 1;

    return true;
  }

  private peek(): string {
    if (this.isAtEnd()) return '\0';

    return this.source.charAt(this.current);
  }

  private peekNext(): string {
    if (this.current + 1 >= this.source.length) return '\0';

    return this.source.charAt(this.current + 1);
  }

  private isDigit(token: string): boolean {
    return token >= '0' && token <= '9';
  }

  private isAlpha(token: string): boolean {
    return (
      (token >= 'a' && token <= 'z') ||
      (token >= 'A' && token <= 'Z') ||
      token == '_'
    );
  }

  private isAlphaNumeric(token: string): boolean {
    return this.isAlpha(token) || this.isDigit(token);
  }

  private comment(): void {
    let comment = '';

    while (this.peek() !== '\n' && !this.isAtEnd()) {
      comment += this.peek();
      this.advance();
    }

    this.addTokenWithLiteral(TokenType.COMMENT, comment.trim());
  }

  private identifier(): void {
    while (this.isAlphaNumeric(this.peek())) this.advance();

    const identifier = this.source.substring(this.start, this.current);
    const type = this.keywords.get(identifier) ?? TokenType.IDENT;

    if (type === TokenType.BOOL)
      this.addTokenWithLiteral(type, identifier === 'true' ? true : false);
    else this.addToken(type);
  }

  private number(): void {
    while (this.isDigit(this.peek())) this.advance();

    let type: TokenType = TokenType.INT;

    if (this.peek() === '.' && this.isDigit(this.peekNext())) {
      type = TokenType.FLOAT;

      this.advance();

      while (this.isDigit(this.peek())) this.advance();
    }

    const literal = this.source.substring(this.start, this.current);
    this.addTokenWithLiteral(type, Number(literal));
  }

  private character(): void {
    if (this.peek() === "'") this.advance();

    let length = 0;
    while (this.peek() !== "'" && !this.isAtEnd()) {
      length += 1;
      this.advance();
    }

    if (length > 1 || this.isAtEnd()) {
      console.error(`line: ${this.line}, Error: unclosed character literal`);
      return;
    }

    this.advance();

    const literal = this.source.substring(this.start + 1, this.current - 1);
    this.addTokenWithLiteral(TokenType.CHAR, literal);
  }

  private string(): void {
    while (this.peek() !== '"' && !this.isAtEnd()) {
      if (this.peek() === '\n') this.line += 1;
      this.advance();
    }

    if (this.isAtEnd()) {
      console.error(`line: ${this.line}, Error: Unterminated string.`);
      return;
    }

    this.advance();

    const literal = this.source.substring(this.start + 1, this.current - 1);
    this.addTokenWithLiteral(TokenType.STRING, literal);
  }
}
