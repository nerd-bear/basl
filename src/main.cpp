// /*

// Copyright 2025-latest I. Mitterfellner

//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at

//        http://www.apache.org/licenses/LICENSE-2.0

//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

// */

// #include "file.hpp"
// #include "lexer.hpp"
// #include "parser.hpp"
// #include "argparse.hpp"
// #include <string>
// #include <iostream>

// int main(int argc, char *argv[])
// {
//     std::string absoluteFileReadPath = "C:/Users/nerdb/Desktop/coding-projects/bassil/input/secondary.basl";

//     try
//     {
//         std::cout << "Starting File Read" << std::endl;
//         std::string content = readFile(absoluteFileReadPath).fileContent;
//         std::cout << "File Read" << std::endl;

//         std::cout << "Starting Lex" << std::endl;
//         auto tokens = lex(content, absoluteFileReadPath);
//         std::cout << "Ended Lex" << std::endl;

//         std::cout << "Initing parser" << std::endl;
//         Parser parser(tokens);
//         std::cout << "Inited parser" << std::endl;
//         try
//         {
//             std::cout << "Starting parser..." << std::endl;
//             auto statements = parser.parse();
//             std::cout << "Parsing completed successfully! Found "
//                       << statements.size() << " statements" << std::endl;

//             std::cout << parser.printAST(statements) << std::endl;
//         }
//         catch (const std::runtime_error &e)
//         {
//             std::cerr << "Parsing failed: " << e.what() << std::endl;
//             return 1;
//         }
//     }
//     catch (...)
//     {
//         std::cerr << "Unexpected error occurred" << std::endl;
//         return 1;
//     }
//     return 0;
// }

#include <argparse.hpp>

int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("program_name");

    program.add_argument("square")
        .help("display the square of a given integer")
        .scan<'i', int>();

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    auto input = program.get<int>("square");
    std::cout << (input * input) << std::endl;

    return 0;
}