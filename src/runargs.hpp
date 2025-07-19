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

#pragma once

#include <string>
#include <argparse.hpp>

struct flagsStruct
{
    std::string inputPath = "";
    std::string outputPath = "a.exe";
    bool warningIgnore = false;
    bool consoleColor = false;
    bool generalProccessLogs = false;
    bool advancedProccessLogs = false;
};

flagsStruct handleRunArgs(int argc, char *argv[], std::string version);