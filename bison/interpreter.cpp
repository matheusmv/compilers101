#include "interpreter.hpp"
#include "parser.tab.hpp"

Interpreter::Interpreter()
    : trace_parsing(false),
      trace_scanning(false) {}

int Interpreter::parse(const std::string &file_path) {
    file = file_path;
    location.initialize(&file);
    scan_begin();
    yy::parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();
    return res;
}
