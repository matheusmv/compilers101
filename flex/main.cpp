#include <FlexLexer.h>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>

#include "token.h"

extern std::size_t line_number;

Token new_token(const TokenType& token_type, const std::string& literal, const std::size_t& line) {
    return Token{token_type, literal, line};
}

std::vector<Token> tokenize(const std::string& file_path) {
    std::ifstream file;
    file.open(file_path);

    std::unique_ptr<FlexLexer> l = std::make_unique<yyFlexLexer>(&file);

    std::vector<Token> tokens;

    auto tt = static_cast<TokenType>(l->yylex());
    while (tt != TokenType::TT_EOF) {
        tokens.emplace_back(new_token(tt, l->YYText(), line_number));
        tt = static_cast<TokenType>(l->yylex());
    }
    tokens.emplace_back(new_token(tt, l->YYText(), line_number));

    file.close();

    return tokens;
}

int main() {
    std::vector<Token> tokens = tokenize("example.txt");

    for (const auto& token : tokens) {
        std::cout << token << std::endl;
    }
}
