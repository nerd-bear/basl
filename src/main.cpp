#include "file.hpp"
#include "lexer.hpp"
#include <string>
#include <iostream>

int main() {
    std::string content = readFile("C:/Users/nerdb/Desktop/coding-projects/bassil/input/main.basl").fileContent;

    save_tokens(lex(content), "C:/Users/nerdb/Desktop/coding-projects/bassil/output/output.json");

    return 0;
}