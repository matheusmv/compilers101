#pragma once

#include <string>
#include <map>
#include "parser.tab.hpp"

#define YY_DECL yy::parser::symbol_type yylex(Interpreter &interpreter)

YY_DECL;

class Interpreter
{
public:
  Interpreter();

  std::map<std::string, double> variables;
  double result;

  int parse(const std::string &file_path);
  bool trace_parsing;

  void scan_begin();
  void scan_end();
  bool trace_scanning;
  yy::location location;

private:
  std::string file;
};
