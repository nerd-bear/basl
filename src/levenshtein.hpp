#pragma once

#include <algorithm>
#include <cctype>
#include <locale>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>

namespace Levenshtein
{
    int computeDistance(std::string inputA, std::string inputB);
}