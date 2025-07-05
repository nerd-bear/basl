#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>

struct file
{
    bool fileReadSuccess;
    std::string fileContent;
};

int checkFileAccess(const std::filesystem::path &path, int amode);
file readFile(std::string absoluteReadPath = "");