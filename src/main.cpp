/*

Copyright 2025-latest I. Mitterfellner

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

#include "file.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "runargs.hpp"
#include "spinner.hpp"
#include <string>
#include <iostream>
#include <csignal>
#include <cstdlib>

#define version "A1.0.0"

void exitSignalHandler(int signalIndex)
{
    std::cout << "Interrupt signal (" << signalIndex << ") received.\n";

    // clean up will be added later

    exit(signalIndex);
}

int main(int argc, char *argv[])
{
    // Abnormal termination of the program, such as a call to abort.
    signal(SIGABRT, exitSignalHandler);
    // An erroneous arithmetic operation, such as a divide by zero or an operation resulting in overflow.
    signal(SIGFPE, exitSignalHandler);
    // Detection of an illegal instruction.
    signal(SIGILL, exitSignalHandler);
    // Receipt of an interactive attention signal.
    signal(SIGINT, exitSignalHandler);
    // An invalid access to storage.
    signal(SIGSEGV, exitSignalHandler);
    // A termination request sent to the program.
    signal(SIGTERM, exitSignalHandler);

    flagsStruct runArgs = handleRunArgs(argc, argv, version);

    std::string inputPath = runArgs.inputPath;
    std::string outputPath = runArgs.outputPath;
    bool warningIgnore = runArgs.warningIgnore;
    bool consoleColor = runArgs.consoleColor;
    bool generalProccessLogs = runArgs.generalProccessLogs;
    bool advancedProccessLogs = runArgs.advancedProccessLogs;

    // AAHHH 

    if (inputPath.length() == 0)
    {
        std::cout << "File input path Invalid\n";
        exit(1);
    }

    try
    {
        std::cout << "Starting File Read" << std::endl;
        std::string content = readFile(inputPath).fileContent;
        std::cout << "File Read" << std::endl;

        std::cout << "Starting Lex" << std::endl;
        auto tokens = lex(content, inputPath);
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

// #include "spinner.hpp"

// int main()
// {
//     spinner::Spinner spinner = spinner::Spinner(brail);

//     spinner.setText("Loading");

//     spinner.setTimeSep(100);

//     spinner.start();

//     std::this_thread::sleep_for(std::chrono::milliseconds(3000));

//     spinner.end();

//     return 0;
// }