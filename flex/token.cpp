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

    {TokenType::TT_ADD, "ADD"},
    {TokenType::TT_SUB, "SUB"},
    {TokenType::TT_MUL, "MUL"},
    {TokenType::TT_QUO, "QUO"},
    {TokenType::TT_REM, "REM"},

    {TokenType::TT_TILDE, "TILDE"},
    {TokenType::TT_AND, "AND"},
    {TokenType::TT_OR, "OR"},
    {TokenType::TT_XOR, "XOR"},
    {TokenType::TT_SHL, "SHL"},
    {TokenType::TT_SHR, "SHR"},

    {TokenType::TT_ADD_ASSIGN, "ADD_ASSIGN"},
    {TokenType::TT_SUB_ASSIGN, "SUB_ASSIGN"},
    {TokenType::TT_MUL_ASSIGN, "MUL_ASSIGN"},
    {TokenType::TT_QUO_ASSIGN, "QUO_ASSIGN"},
    {TokenType::TT_REM_ASSIGN, "REM_ASSIGN"},

    {TokenType::TT_AND_ASSIGN, "AND_ASSIGN"},
    {TokenType::TT_OR_ASSIGN, "OR_ASSIGN"},
    {TokenType::TT_XOR_ASSIGN, "XOR_ASSIGN"},
    {TokenType::TT_SHL_ASSIGN, "SHL_ASSIGN"},
    {TokenType::TT_SHR_ASSIGN, "SHR_ASSIGN"},

    {TokenType::TT_LAND, "LAND"},
    {TokenType::TT_LOR, "LOR"},
    {TokenType::TT_INC, "INC"},
    {TokenType::TT_DEC, "DEC"},

    {TokenType::TT_EQL, "EQL"},
    {TokenType::TT_LSS, "LSS"},
    {TokenType::TT_GTR, "GTR"},
    {TokenType::TT_ASSIGN, "ASSIGN"},
    {TokenType::TT_NOT, "NOT"},

    {TokenType::TT_NEQ, "NEQ"},
    {TokenType::TT_LEQ, "LEQ"},
    {TokenType::TT_GEQ, "GEQ"},
    {TokenType::TT_DEFINE, "DEFINE"},
    {TokenType::TT_ELLIPSIS, "ELLIPSIS"},

    {TokenType::TT_LPAREN, "LPAREN"},
    {TokenType::TT_LBRACK, "LBRACK"},
    {TokenType::TT_LBRACE, "LBRACE"},
    {TokenType::TT_COMMA, "COMMA"},
    {TokenType::TT_PERIOD, "PERIOD"},

    {TokenType::TT_RPAREN, "RPAREN"},
    {TokenType::TT_RBRACK, "RBRACK"},
    {TokenType::TT_RBRACE, "RBRACE"},
    {TokenType::TT_SEMICOLON, "SEMICOLON"},
    {TokenType::TT_COLON, "COLON"},

    {TokenType::FUNCTION, "FUNCTION"},
    {TokenType::LET, "LET"},
    {TokenType::TRUE, "TRUE"},
    {TokenType::FALSE, "FALSE"},
    {TokenType::IF, "IF"},
    {TokenType::ELSE, "ELSE"},
    {TokenType::RETURN, "RETURN"},
    {TokenType::BREAK, "BREAK"},
    {TokenType::CONTINUE, "CONTINUE"},
    {TokenType::WHILE, "WHILE"},
    {TokenType::FOR, "FOR"},
    {TokenType::NIL, "NIL"},
};

static const std::unordered_map<std::string, TokenType> keywords = {
    {"func", TokenType::FUNCTION},
    {"let", TokenType::LET},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"return", TokenType::RETURN},
    {"break", TokenType::BREAK},
    {"continue", TokenType::CONTINUE},
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
