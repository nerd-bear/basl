#include "file.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <string>
#include <iostream>

int main()
{
    try
    {
        std::cout << "Starting File Read" << std::endl;
        std::string content = readFile("C:/Users/nerdb/Desktop/coding-projects/bassil/input/main.basl").fileContent;
        std::cout << "File Read" << std::endl;

        std::cout << "Starting Lex" << std::endl;
        auto tokens = lex(content);
        std::cout << "Ended Lex" << std::endl;

        std::cout << "Initing parser" << std::endl;
        Parser parser(tokens);
        std::cout << "Inited parser" << std::endl;
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