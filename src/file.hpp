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

#include <filesystem>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <cstdint>
#include <clocale>
#include <codecvt>
#include <locale>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>

enum class FileType
{
    BINARY,
    TEXT_ASCII,
    TEXT_UTF8,
    TEXT_UTF16_LE,
    TEXT_UTF16_BE,
    TEXT_UTF32_LE,
    TEXT_UTF32_BE,
    UNKNOWN
};

struct fileStruct
{
    bool fileReadSuccess;
    std::string fileContent;
};

int checkFileAccess(const std::filesystem::path &path, int amode);

fileStruct readFile(std::string absoluteReadPath = "");

std::string pathToAbsolutePath(std::string pathStr);    