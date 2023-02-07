import { readFileSync, writeFileSync } from "fs";
import { dirname, resolve } from "path";

const parserDir = `./echo.js`;
const __dirname = dirname(parserDir);

let parserDirContent = readFileSync(resolve(__dirname, parserDir), "utf-8");

parserDirContent = parserDirContent.replace(
  /if \(typeof require(.|\n)*/,
  "export default echo"
);

writeFileSync(resolve(__dirname, parserDir), parserDirContent);
