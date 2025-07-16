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

/*

As of writing this, we are using https://github.com/p-ranav/argparse for our
runtime argument parser, however in the future we are planning to rewrite
the argument parser for our own specific needs. This will get us closer to
our goal of zero thrid party libs by 1.0.0 full release. When the argument
parser will be rewritten, we wish to add a feature so that if an incorrect
argument is passed that does not exist, Levenshtein will be used to locate
the most similar argument if one exists. This is similar to git or the memo
discord bots command handler.

*/

#include "levenshtein.hpp"

namespace Levenshtein
{
    namespace helper
    {
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

        inline std::string ltrim_copy(std::string s)
        {
            ltrim(s);
            return s;
        }

        inline std::string rtrim_copy(std::string s)
        {
            rtrim(s);
            return s;
        }

        inline std::string trim_copy(std::string s)
        {
            trim(s);
            return s;
        }
    }

    int computeDistance(std::string inputA, std::string inputB)
    {
        helper::trim(inputA);
        helper::trim(inputB);

        int sizeA = inputA.size();
        int sizeB = inputB.size();

        std::vector<std::vector<int>> matrix(sizeA + 1, std::vector<int>(sizeB + 1));

        if (sizeA == 0)
            return sizeB;
        if (sizeB == 0)
            return sizeA;

        for (int i = 0; i <= sizeA; i++)
            matrix[i][0] = i;
        for (int j = 0; j <= sizeB; j++)
            matrix[0][j] = j;

        for (int i = 1; i <= sizeA; i++)
        {
            for (int j = 1; j <= sizeB; j++)
            {
                int score = (inputB[j - 1] == inputA[i - 1]) ? 0 : 1;

                matrix[i][j] = std::min(
                    std::min(matrix[i - 1][j] + 1, matrix[i][j - 1] + 1),
                    matrix[i - 1][j - 1] + score);
            }
        }

        return matrix[sizeA][sizeB];
    }
}
