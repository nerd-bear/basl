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

#include "lexer.hpp"
#include <iostream>

inline bool isIdentifierStart(char c)
{
    return std::isalpha(c) || c == '_';
}

inline bool isIdentifierContinuation(char c)
{
    return std::isalnum(c) || c == '_';
}

std::vector<Token> lex(const std::string &inputString, const std::string filePath)
{
    std::vector<Token> tokens;

    std::unordered_map<std::string, TokenKind> keywords = {
        {"int", TK_TypeInteger},
        {"char", TK_TypeChar},
        {"float", TK_TypeFloat},
        {"string", TK_TypeString},
        {"bool", TK_TypeBool}, // Add this
        {"if", TK_KeywordIf},
        {"else", TK_KeywordElse},
        {"for", TK_KeywordFor},
        {"function", TK_KeywordFunction},
        {"return", TK_KeywordReturn},
        {"print", TK_KeywordPrint}};

    std::unordered_map<std::string, TokenKind> operators = {
        {"+", TK_MathOperator},
        {"-", TK_MathOperator},
        {"*", TK_MathOperator},
        {"/", TK_MathOperator},
        {"%", TK_MathOperator},
        {"=", TK_EqualsSign},
        {"==", TK_ComparisonOperator},
        {"!=", TK_ComparisonOperator},
        {"<", TK_ComparisonOperator},
        {">", TK_ComparisonOperator},
        {"<=", TK_ComparisonOperator},
        {">=", TK_ComparisonOperator},
        {"&&", TK_LogicalOperator},
        {"||", TK_LogicalOperator},
        {"!", TK_LogicalOperator},
        {";", TK_Semicolon},
        {"(", TK_OpenParen},
        {")", TK_CloseParen},
        {"{", TK_OpenBrace},
        {"}", TK_CloseBrace},
        {",", TK_Comma}};

    size_t pos = 0;
    int line = 1;
    int column = 1;

    auto addToken = [&](TokenKind type, const std::string &value, int startColumn)
    {
        tokens.push_back({type, value, line, startColumn, column - 1});
    };

    while (pos < inputString.length())
    {
        char currentChar = inputString[pos];

        if (std::isspace(currentChar))
        {
            if (currentChar == '\n')
            {
                line++;
                column = 1;
            }
            else
            {
                column++;
            }
            pos++;
            continue;
        }

        if (isIdentifierStart(currentChar))
        {
            size_t startPos = pos;
            int startColumn = column;

            while (pos < inputString.length() && isIdentifierContinuation(inputString[pos]))
            {
                pos++;
                column++;
            }

            std::string identifier = inputString.substr(startPos, pos - startPos);

            auto it = keywords.find(identifier);
            if (it != keywords.end())
            {
                addToken(it->second, identifier, startColumn);
            }
            else
            {
                addToken(TK_Identifier, identifier, startColumn);
            }
            continue;
        }

        if (currentChar == '-' && std::isdigit(inputString[pos + 1]))
        {
            pos++;
            column++;
            size_t startPos = pos;
            int startColumn = column;
            bool isFloat = false;

            while (pos < inputString.length() && (std::isdigit(inputString[pos]) || inputString[pos] == '.'))
            {
                if (inputString[pos] == '.')
                {
                    if (isFloat)
                    {
                        break;
                    }
                    isFloat = true;
                }
                pos++;
                column++;
            }

            std::string number = inputString.substr(startPos, pos - startPos);
            addToken(isFloat ? TK_Float : TK_Integer, number, startColumn);
            continue;
        }

        // Handle character literals (single quotes)
        if (currentChar == '\'')
        {
            size_t startPos = pos;
            int startColumn = column;
            pos++;
            column++;

            // Handle escape sequences
            if (pos < inputString.length() && inputString[pos] == '\\')
            {
                pos++;
                column++;
            }

            if (pos >= inputString.length() || inputString[pos] == '\'')
            {
                // Invalid character literal (empty or unterminated)
                pos++;
                column++;
                std::string charLit = inputString.substr(startPos, pos - startPos);
                addToken(TK_String, charLit, startColumn);
                continue;
            }

            // Get the actual character
            pos++;
            column++;

            // Expect closing quote
            if (pos >= inputString.length() || inputString[pos] != '\'')
            {
                // Unterminated character literal
                std::string charLit = inputString.substr(startPos, pos - startPos);
                addToken(TK_String, charLit, startColumn);
                continue;
            }

            pos++;
            column++;
            std::string charLit = inputString.substr(startPos, pos - startPos);
            addToken(TK_String, charLit, startColumn);
            continue;
        }

        // Handle string literals (double quotes)
        if (currentChar == '"')
        {
            size_t startPos = pos;
            int startColumn = column;
            pos++;
            column++;

            while (pos < inputString.length() && inputString[pos] != '"')
            {
                if (inputString[pos] == '\\' && pos + 1 < inputString.length())
                {
                    pos += 2;
                    column += 2;
                }
                else
                {
                    pos++;
                    column++;
                }
            }

            if (pos >= inputString.length())
            {
                // Unterminated string
                std::string str = inputString.substr(startPos, pos - startPos);
                addToken(TK_String, str, startColumn);
                continue;
            }

            pos++;
            column++;
            std::string str = inputString.substr(startPos, pos - startPos);
            addToken(TK_String, str, startColumn);
            continue;
        }

        // Handle operators
        std::string op(1, currentChar);
        if (pos + 1 < inputString.length())
        {
            std::string doubleOp = op + inputString[pos + 1];
            auto it = operators.find(doubleOp);
            if (it != operators.end())
            {
                addToken(it->second, doubleOp, column);
                pos += 2;
                column += 2;
                continue;
            }
        }

        auto it = operators.find(op);
        if (it != operators.end())
        {
            addToken(it->second, op, column);
            pos++;
            column++;
            continue;
        }

        // TODO: Calculate the proper end column
        // reportError(filePath, line, column, column+5, "Unkown token");

        pos++;
        column++;
    }

    // Add EOF token (only once)
    if (tokens.empty() || tokens.back().type != TK_EOF)
    {
        tokens.push_back({TK_EOF, "", line, column, column});
    }

    return tokens;
}