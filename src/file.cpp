/*

Copyright 2025-latest I. Mitterfellner

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   limitations under the License.

*/

#include "file.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <system_error>
#include <iostream>

#define FILE_OK 0
#define READ_OK 1
#define WRITE_OK 2
#define XX_OK 3

inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                    { return !std::isspace(ch); }));
}

inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                         { return !std::isspace(ch); })
                .base(),
            s.end());
}

inline void trim(std::string &s)
{
    rtrim(s);
    ltrim(s);
}

// THIS FUNCTION IS REMOVED

// // TODO: Consider not forcing file extensions in the future.
// bool checkFileExtension(std::string absoluteFilePath)
// {
//     /*
//     The expression "should" be able to handle both Windows and Unix paths.
//     Additionaly included is handling of every possible edge case and bypass.
//     bidirectionalcompatability should be strongly considered and is
//     recommended for implementation in the future.

//     ^(?:\/|(?:(?:[A-Za-z]:[\\\/])|(?:\\\\[^\\\/:]+[\\\/]+[^\\\/:]+[\\\/]+)|(?:\\\\\?\\[A-Za-z]:[\\\/])|(?:\\\\\?\\UNC\\(?:[^\\\/:]+)[\\\/]+(?:[^\\\/:]+)[\\\/]+)))(?:[^\\\/]*[\\\/])*[^\\\/]*\.[bB][aA][sS][lL]$
//     */

//     if (absoluteFilePath.find(".") == std::string::npos)
//     {
//         return false;
//     }

//     std::vector<std::string> splittedString = splitString(absoluteFilePath, ".");

//     if (!splittedString.empty() && splittedString.back() == "basl")
//     {
//         return false;
//     }
//     else
//     {
//         return true;
//     }
// }

std::string normalizePath(std::string path)
{
    std::replace(path.begin(), path.end(), '\\', '/');

    if (!path.empty() && path.back() == '/')
    {
        path.pop_back();
    }
    return path;
}

int checkFileAccess(const std::filesystem::path &path, int amode)
{
    std::error_code ec;
    auto status = std::filesystem::status(path, ec);
    if (ec)
        return -1;

    auto perms = status.permissions();

    switch (amode)
    {
    case READ_OK:
        return (perms & std::filesystem::perms::owner_read) != std::filesystem::perms::none ? 0 : -1;
    case FILE_OK:
        return std::filesystem::exists(status) ? 0 : -1;
    case WRITE_OK:
        return (perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none ? 0 : -1;
    case XX_OK:
        return (perms & std::filesystem::perms::owner_exec) != std::filesystem::perms::none ? 0 : -1;
    default:
        return -1;
    }
}

FileType detectFileType(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        return FileType::UNKNOWN;

    // first 4 bytes to check for BOM
    std::vector<uint8_t> header(4);
    if (!file.read(reinterpret_cast<char *>(header.data()), 4))
    {
        // no 4 bytes reset
        file.clear();
        file.seekg(0);
    }
    size_t bytes_read = file.gcount();

    // BOM markers
    if (bytes_read >= 2)
    {
        // UTF-16 LE
        if (header[0] == 0xFF && header[1] == 0xFE)
        {
            if (bytes_read >= 4 && header[2] == 0x00 && header[3] == 0x00)
            {
                return FileType::TEXT_UTF32_LE;
            }
            return FileType::TEXT_UTF16_LE;
        }
        // UTF-16 BE
        if (header[0] == 0xFE && header[1] == 0xFF)
        {
            return FileType::TEXT_UTF16_BE;
        }
    }
    if (bytes_read >= 3 && header[0] == 0xEF && header[1] == 0xBB && header[2] == 0xBF)
    {
        return FileType::TEXT_UTF8;
    }
    if (bytes_read >= 4 && header[0] == 0x00 && header[1] == 0x00 &&
        header[2] == 0xFE && header[3] == 0xFF)
    {
        return FileType::TEXT_UTF32_BE;
    }

    // no BOM found so check content
    file.seekg(0);
    bool has_null = false;
    bool is_ascii = true;
    bool is_utf8 = true;
    size_t utf8_continuation = 0;
    const size_t max_bytes_to_check = 4096;
    size_t total_bytes = 0;

    uint8_t byte;
    while (total_bytes < max_bytes_to_check && file.read(reinterpret_cast<char *>(&byte), 1))
    {
        total_bytes++;

        // null byte
        if (byte == 0x00)
            has_null = true;

        // non-ASCII
        if (byte > 127)
            is_ascii = false;

        // UTF8 sequences
        if (utf8_continuation > 0)
        {
            if ((byte & 0xC0) != 0x80)
                is_utf8 = false;
            utf8_continuation--;
        }
        else if (byte > 127)
        {
            if ((byte & 0xE0) == 0xC0)
                utf8_continuation = 1;
            else if ((byte & 0xF0) == 0xE0)
                utf8_continuation = 2;
            else if ((byte & 0xF8) == 0xF0)
                utf8_continuation = 3;
            else
                is_utf8 = false;
        }
    }

    if (utf8_continuation > 0)
        is_utf8 = false;

    if (has_null)
        return FileType::BINARY;
    if (is_ascii)
        return FileType::TEXT_ASCII;
    if (is_utf8)
        return FileType::TEXT_UTF8;
    return FileType::BINARY;
}

fileStruct readFile(std::string absoluteReadPath)
{
    std::cout << absoluteReadPath << std::endl;

    std::string absPath = normalizePath(pathToAbsolutePath(absoluteReadPath));
    if (absPath.empty())
    {
        std::cerr << "Path resolution failed for: " << absoluteReadPath << std::endl;
        return {false, ""};
    }

    std::error_code ec;
    std::filesystem::path path(absPath);

    if (!std::filesystem::exists(path, ec) || ec)
        return {false, ""};
    if (!std::filesystem::is_regular_file(path, ec) || ec)
        return {false, ""};
    if (checkFileAccess(path, READ_OK) != 0)
        return {false, ""};
    ;

    FileType type = detectFileType(absoluteReadPath);
    if (type == FileType::UNKNOWN || type == FileType::BINARY)
    {
        return {false, ""};
    }

    std::ifstream inputFileStream(path, std::ios::binary);
    if (!inputFileStream)
        return {false, ""};

    std::string content;
    try
    {
        inputFileStream.seekg(0, std::ios::end);
        auto size = inputFileStream.tellg();
        if (size == -1)
            return {false, ""};

        content.resize(static_cast<size_t>(size));
        inputFileStream.seekg(0);
        inputFileStream.read(&content[0], size);

        if (!inputFileStream && !inputFileStream.eof())
        {
            return {false, ""};
        }
    }
    catch (const std::exception &)
    {
        return {false, ""};
    }

    return {true, content};
}

std::string pathToAbsolutePath(std::string pathStr)
{
    trim(pathStr);
    if (pathStr.empty())
    {
        std::cerr << "Empty path provided" << std::endl;
        return "";
    }

    try
    {
        std::filesystem::path pathObj(pathStr);
        std::cerr << "Original path: " << pathStr << std::endl;

        if (!pathObj.is_absolute())
        {
            std::filesystem::path absolute = std::filesystem::absolute(pathObj);
            std::cerr << "Converted to absolute: " << absolute << std::endl;
            return absolute.generic_string();
        }

        std::cerr << "Already absolute: " << pathObj << std::endl;
        return pathObj.generic_string();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Path conversion error: " << e.what() << std::endl;
        return "";
    }
}