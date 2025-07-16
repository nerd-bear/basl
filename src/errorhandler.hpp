#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

int reportError(const std::string &filePath, int lineNumber, int startColumn, int endColumn, const std::string &msg);