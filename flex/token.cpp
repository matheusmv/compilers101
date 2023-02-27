#include "token.h"

#include <functional>
#include <unordered_map>

static const std::unordered_map<TokenType, std::string> tokens = {
    {TokenType::TT_ILLEGAL, "ILLEGAL"},
    {TokenType::TT_EOF, "EOF"},
    {TokenType::TT_COMMENT, "COMMENT"},

    {TokenType::TT_IDENT, "IDENT"},
    {TokenType::TT_INT, "INT"},
    {TokenType::TT_FLOAT, "FLOAT"},
    {TokenType::TT_CHAR, "CHAR"},
    {TokenType::TT_STRING, "STRING"},

    {TokenType::TT_ADD, "+"},
    {TokenType::TT_SUB, "-"},
    {TokenType::TT_MUL, "*"},
    {TokenType::TT_QUO, "/"},
    {TokenType::TT_REM, "%"},

    {TokenType::TT_TILDE, "~"},
    {TokenType::TT_AND, "&"},
    {TokenType::TT_OR, "|"},
    {TokenType::TT_XOR, "^"},
    {TokenType::TT_SHL, "<<"},
    {TokenType::TT_SHR, ">>"},

    {TokenType::TT_ADD_ASSIGN, "+="},
    {TokenType::TT_SUB_ASSIGN, "-="},
    {TokenType::TT_MUL_ASSIGN, "*="},
    {TokenType::TT_QUO_ASSIGN, "/="},
    {TokenType::TT_REM_ASSIGN, "%="},

    {TokenType::TT_AND_ASSIGN, "&="},
    {TokenType::TT_OR_ASSIGN, "|="},
    {TokenType::TT_XOR_ASSIGN, "^="},
    {TokenType::TT_SHL_ASSIGN, "<<="},
    {TokenType::TT_SHR_ASSIGN, ">>="},

    {TokenType::TT_LAND, "&&"},
    {TokenType::TT_LOR, "||"},
    {TokenType::TT_INC, "++"},
    {TokenType::TT_DEC, "--"},

    {TokenType::TT_EQL, "=="},
    {TokenType::TT_LSS, "<"},
    {TokenType::TT_GTR, ">"},
    {TokenType::TT_ASSIGN, "="},
    {TokenType::TT_NOT, "!"},

    {TokenType::TT_NEQ, "!="},
    {TokenType::TT_LEQ, "<="},
    {TokenType::TT_GEQ, ">="},
    {TokenType::TT_DEFINE, ":="},

    {TokenType::TT_LPAREN, "("},
    {TokenType::TT_LBRACK, "["},
    {TokenType::TT_LBRACE, "{"},
    {TokenType::TT_COMMA, ","},
    {TokenType::TT_PERIOD, "."},

    {TokenType::TT_RPAREN, ")"},
    {TokenType::TT_RBRACK, "]"},
    {TokenType::TT_RBRACE, "}"},
    {TokenType::TT_SEMICOLON, ";"},
    {TokenType::TT_COLON, ":"},

    {TokenType::FUNCTION, "func"},
    {TokenType::LET, "let"},
    {TokenType::TRUE, "true"},
    {TokenType::FALSE, "false"},
    {TokenType::IF, "if"},
    {TokenType::ELSE, "else"},
    {TokenType::RETURN, "return"},
    {TokenType::WHILE, "while"},
    {TokenType::FOR, "for"},
    {TokenType::NIL, "nil"},
};

static const std::unordered_map<std::string, TokenType> keywords = {
    {"func", TokenType::FUNCTION},
    {"let", TokenType::LET},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"return", TokenType::RETURN},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"nil", TokenType::NIL},
};

const std::string& to_string(const TokenType& token_type) {
    if (tokens.find(token_type) != tokens.end()) {
        return tokens.at(token_type);
    }

    static const std::string blank;

    return blank;
}

TokenType lookup(const std::string& literal) {
    if (keywords.find(literal) != keywords.end()) {
        return keywords.at(literal);
    }

    return TokenType::TT_IDENT;
}

bool is_literal(const TokenType& token_type) {
    return TokenType::literal_start < token_type && token_type < TokenType::literal_end;
}

bool is_operator(const TokenType& token_type) {
    return TokenType::operator_start < token_type && token_type < TokenType::operator_end;
}

bool is_keyword(const TokenType& token_type) {
    return TokenType::keyword_start < token_type && token_type < TokenType::keyword_end;
}

bool is_keyword(const std::string& literal) {
    return keywords.find(literal) != keywords.end();
}

bool is_identifier(const std::string& literal) {
    if (literal.empty() || is_keyword(literal)) {
        return false;
    }

    if (std::isdigit(literal[0])) {
        return false;
    }

    return std::all_of(literal.cbegin(), literal.cend(), [&](const auto& character) {
        return std::isalpha(character) || std::isdigit(character) || character == '_';
    });
}
