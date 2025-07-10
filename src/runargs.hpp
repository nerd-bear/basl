#pragma once

#include <string>

struct flagsStruct
{
    std::string inputPath = "";
    std::string outputPath = "a.exe";
    bool warningIgnore = false;
    bool consoleOutput = true;
    bool consoleColor = false;
    bool generalProccessLogs = false;
    bool advancedProccessLogs = false;
};