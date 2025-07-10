#include "file.hpp"

#define FILE_OK 0
#define READ_OK 1
#define WRITE_OK 2
#define XX_OK 3

int checkFileAccess(const std::filesystem::path &path, int amode)
{
    auto a = std::filesystem::status(path).permissions();
    switch (amode)
    {
    case READ_OK:
        return (std::filesystem::perms::owner_read & a) == std::filesystem::perms::owner_read ? 0 : -1;
    case FILE_OK:
        return std::filesystem::perms::unknown != a ? 0 : -1;
    case WRITE_OK:
        return (std::filesystem::perms::owner_read & a) == std::filesystem::perms::owner_read && (std::filesystem::perms::owner_write & a) == std::filesystem::perms::owner_write ? 0 : -1;
    case XX_OK:
        return (std::filesystem::perms::owner_read & a) == std::filesystem::perms::owner_read && (std::filesystem::perms::owner_exec & a) == std::filesystem::perms::owner_exec ? 0 : -1;
    default:
        return -1;
    }
}

std::vector<std::string> splitString(const std::string &s, const std::string &delimiter = " ")
{
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;

    while ((end = s.find(delimiter, start)) != std::string::npos)
    {
        tokens.push_back(s.substr(start, end - start));
        start = end + delimiter.length();
    }

    tokens.push_back(s.substr(start));
    return tokens;
}

// THIS FUNCTION IS REMOVED

// // TODO: Consider not forcing file extensions in the future.
// bool checkFileExtension(std::string absoluteFilePath)
// {
//     /*
//     The following is a Regex expression to handle this entire function.
//     The expression "should" be able to handle both Windows and Unix paths.
//     Additionaly included is handling of every possible edge case and bypass.
//     This is NOT a production solution and SHOULD NEVER replace the current
//     implementation, however after further discussion a change to regex for
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

FileType detectFileType(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    
    if (!file)
    {
        return FileType::UNKNOWN;
    }

    // read first 4 bytes to check for BOM (Byte Order Mark)
    std::vector<uint8_t> header(4);
    file.read(reinterpret_cast<char *>(header.data()), 4);
    size_t bytes_read = file.gcount();

    // BOM markers
    if (bytes_read >= 2)
    {
        // UTF-16 Little Endian
        if (header[0] == 0xFF && header[1] == 0xFE)
        {
            if (bytes_read >= 4 && header[2] == 0x00 && header[3] == 0x00)
            {
                return FileType::TEXT_UTF32_LE;
            }
            return FileType::TEXT_UTF16_LE;
        }

        // UTF-16 Big Endian
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

    // no BOM found, need to analyze content
    file.seekg(0);
    bool has_null = false;
    bool is_ascii = true;
    bool is_utf8 = true;
    size_t utf8_continuation = 0;
    size_t total_bytes = 0;
    const size_t max_bytes_to_check = 4096;

    uint8_t byte;
    while (file.read(reinterpret_cast<char *>(&byte), 1) && total_bytes < max_bytes_to_check)
    {
        total_bytes++;

        // check for null byte
        if (byte == 0x00)
        {
            has_null = true;
        }

        if (byte > 127)
        {
            is_ascii = false;
        }

        // UTF-8 auth
        if (utf8_continuation > 0)
        {
            // continuation byte must start with 10xxxxxx
            if ((byte & 0xC0) != 0x80)
            {
                is_utf8 = false;
            }
            utf8_continuation--;
        }
        else
        {
            // check UTF-8 lead byte
            if ((byte & 0x80) != 0)
            {
                // check array length
                if ((byte & 0xE0) == 0xC0)
                {
                    utf8_continuation = 1;
                }
                else if ((byte & 0xF0) == 0xE0)
                {
                    utf8_continuation = 2;
                }
                else if ((byte & 0xF8) == 0xF0)
                {
                    utf8_continuation = 3;
                }
                else
                {
                    is_utf8 = false;
                }
            }
        }
    }

    if (has_null)
    {
        return FileType::BINARY;
    }

    if (is_ascii)
    {
        return FileType::TEXT_ASCII;
    }

    if (is_utf8 && utf8_continuation == 0)
    {
        return FileType::TEXT_UTF8;
    }

    // could be UTF-16/32 without BOM but harder to detect reliably
    return FileType::BINARY;
}

file readFile(std::string absoluteReadPath)
{

    if (absoluteReadPath == "" || std::filesystem::is_directory(absoluteReadPath))
    {
        return file{false, ""};
    }

    const std::filesystem::path path = absoluteReadPath;

    if (checkFileAccess(path, READ_OK) != 0)
    {
        return file{false, ""};
    }

    FileType type = detectFileType(absoluteReadPath);

    if (type == FileType::UNKNOWN || type == FileType::BINARY)
    {
        return file{false, ""};
    }

    std::string outputFileString;

    std::ifstream inputFileStream(absoluteReadPath);

    std::string returnOutputFileString;

    while (std::getline(inputFileStream, outputFileString))
    {
        returnOutputFileString = returnOutputFileString + outputFileString + "\n";
    }

    return file{true, returnOutputFileString};
}