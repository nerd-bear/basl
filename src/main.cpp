#include "file.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <string>
#include <iostream>

int main()
{
    try
    {

        std::string content = readFile("C:/Users/nerdb/Desktop/coding-projects/bassil/input/main.basl").fileContent;

        auto tokens = lex(content);

        Parser parser(tokens);
        try
        {
            std::cout << "Starting parser..." << std::endl;
            auto statements = parser.parse();
            std::cout << "Parsing completed successfully! Found "
                      << statements.size() << " statements" << std::endl;

            std::cout << parser.printAST(statements) << std::endl;
        }
        catch (const std::runtime_error &e)
        {
            std::cerr << "Parsing failed: " << e.what() << std::endl;
            return 1;
        }
    }
    catch (...)
    {
        std::cerr << "Unexpected error occurred" << std::endl;
        return 1;
    }
    return 0;
}