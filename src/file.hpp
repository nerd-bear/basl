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
#include <sys/types.h>
#include <sys/stat.h>

enum class FileType {
    BINARY,
    TEXT_ASCII,
    TEXT_UTF8,
    TEXT_UTF16_LE,
    TEXT_UTF16_BE,
    TEXT_UTF32_LE,
    TEXT_UTF32_BE,
    UNKNOWN
};

struct file
{
    bool fileReadSuccess;
    std::string fileContent;
};

int checkFileAccess(const std::filesystem::path &path, int amode);
file readFile(std::string absoluteReadPath = "");