#include <FlexLexer.h>
#include <fstream>
#include <iostream>
#include <memory>

#include "token.h"

int main() {
    std::ifstream file;
    file.open("example.txt");

    std::unique_ptr<FlexLexer> l = std::make_unique<yyFlexLexer>(&file);
    l->yylex();

    file.close();
}
