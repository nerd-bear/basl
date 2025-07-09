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

std::vector<Token> lex(const std::string &inputString)
{
    std::vector<Token> tokens;

    std::unordered_map<std::string, TokenKind> keywords = {
        {"int", TK_TypeInteger},
        {"char", TK_TypeChar},
        {"float", TK_TypeFloat},
        {"string", TK_TypeString}};

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
        {"!", TK_LogicalOperator}};

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

        if (std::isdigit(currentChar))
        {
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
                break;
            }

            pos++;
            column++;
            std::string str = inputString.substr(startPos, pos - startPos);
            addToken(TK_String, str, startColumn);
            continue;
        }

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
    }

    if (!tokens.empty() && tokens.back().type != TK_EOF)
    {
        tokens.push_back({TK_EOF, "", line, column, column});
    }

    else if (tokens.empty())
    {
        tokens.push_back({TK_EOF, "", 1, 1, 1});
    }

    if (!tokens.empty() && tokens.back().type != TK_EOF)
    {
        tokens.push_back({TK_EOF, "", line, column, column});
    }
    else if (tokens.empty())
    {
        tokens.push_back({TK_EOF, "", 1, 1, 1});
    }

    std::cout << "Lexer produced " << tokens.size() << " tokens\n";
    return tokens;
}