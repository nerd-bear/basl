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

// TODO: Consider not forcing file extensions in the future.
bool checkFileExtension(std::string absoluteFilePath)
{
    /*
    The following is a Regex expression to handle this entire function.
    The expression "should" be able to handle both Windows and Unix paths.
    Additionaly included is handling of every possible edge case and bypass.
    This is NOT a production solution and SHOULD NEVER replace the current
    implementation, however after further discussion a change to regex for
    bidirectionalcompatability should be strongly considered and is
    recommended for implementation in the future.

    ^(?:\/|(?:(?:[A-Za-z]:[\\\/])|(?:\\\\[^\\\/:]+[\\\/]+[^\\\/:]+[\\\/]+)|(?:\\\\\?\\[A-Za-z]:[\\\/])|(?:\\\\\?\\UNC\\(?:[^\\\/:]+)[\\\/]+(?:[^\\\/:]+)[\\\/]+)))(?:[^\\\/]*[\\\/])*[^\\\/]*\.[bB][aA][sS][lL]$
    */

    if (absoluteFilePath.find(".") == std::string::npos)
    {
        return false;
    }

    std::vector<std::string> splittedString = splitString(absoluteFilePath, ".");

    if (!splittedString.empty() && splittedString.back() == "basl")
    {
        return false;
    }
    else
    {
        return true;
    }
}

file readFile(std::string absoluteReadPath)
{

    if (absoluteReadPath == "" || std::filesystem::is_directory(absoluteReadPath))
    {
        return file{false, ""};
    }

    const std::filesystem::path path = absoluteReadPath;

    if (checkFileAccess(path, READ_OK) != 0 || checkFileExtension(absoluteReadPath) == false)
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