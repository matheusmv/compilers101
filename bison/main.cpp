#include <iostream>

#include "interpreter.hpp"

int main(int argc, char *argv[])
{
    int result = 0;
    Interpreter interpreter;

    for (int i = 1; i < argc; ++i) {
        if (argv[i] == std::string("-p")) {
            interpreter.trace_parsing = true;
        } else if (argv[i] == std::string("-s")) {
            interpreter.trace_scanning = true;
        } else {
            interpreter.parse(argv[i]);
            result = interpreter.result;
        }
    }

    return result;
}
