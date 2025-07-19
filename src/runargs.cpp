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

#include "runargs.hpp"

flagsStruct handleRunArgs(int argc, char *argv[], std::string version)
{
    argparse::ArgumentParser program("bassil", version);

    // ========= ARGS =========
    program.add_argument("-o", "--output", "--outputFile")
        .help("If you are compiling, this is the directory path where the executable will be exported too")
        .default_value(std::string{"./a.exe"});

    program.add_argument("-i", "--input", "--inputFile")
        .help("This is the file path where your source code is")
        .required();

    // ========= FLAGS =========
    program.add_argument("-w", "--warning", "--waringIgnore")
        .help("Flag to ignore all warnings")
        .flag();

    program.add_argument("-cc", "--color", "--consoleColor")
        .help("Flag to enable color for logs")
        .flag();

    program.add_argument("-log", "--logs", "--generalProccessLogs")
        .help("Flag to enable general compile logs")
        .flag();

    program.add_argument("-alog", "--alogs", "--advancedProccessLogs")
        .help("Flag to enable advanced compile logs")
        .flag();

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        exit(1);
    }

    flagsStruct returnFlagsStruct = flagsStruct{
        program.get<std::string>("-i"),
        program.get<std::string>("-o"),
        program.get<bool>("-w"),
        program.get<bool>("-cc"),
        program.get<bool>("-log"),
        program.get<bool>("-alog")};

    return flagsStruct{};
}