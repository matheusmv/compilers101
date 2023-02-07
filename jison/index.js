import parser from "./echo.js";

function runParser(expr) {
  return parser.parse(expr);
}

runParser("1 + 1 * 2");
runParser("(1 + 1) * 2");
runParser("1 + -1");
runParser("5!");
runParser("PI");
runParser("E");
