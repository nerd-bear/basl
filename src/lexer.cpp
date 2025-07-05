#include "lexer.hpp"
#include <iostream>

namespace Utils
{
    int general_log(const std::string &str, bool isPrintTrue)
    {
        if (!isPrintTrue)
        {
            return 0;
        }

        std::ofstream outputFile("C:/Users/nerdb/Desktop/coding-projects/bassil/output/logs.log", std::ios::app);
        if (!outputFile.is_open())
        {
            std::cerr << "[general_log] Failed to open file." << std::endl;
            return 1;
        }
        outputFile << str << "\n";
        outputFile.close();
        return 0;
    }
}

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
    Utils::general_log("[lex] Starting lexical analysis.", true);
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
        Utils::general_log("[lex] Added token: " + value + " at line " +
                           std::to_string(line) + ", columns " +
                           std::to_string(startColumn) + "-" +
                           std::to_string(column - 1), true);
    };

    while (pos < inputString.length())
    {
        char currentChar = inputString[pos];
        Utils::general_log("[lex] Reading char: '" + std::string(1, currentChar) +
                           "' at pos " + std::to_string(pos), true);

        if (std::isspace(currentChar))
        {
            if (currentChar == '\n')
            {
                Utils::general_log("[lex] Newline detected.", true);
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
            Utils::general_log("[lex] Starting identifier at pos " + std::to_string(pos), true);

            while (pos < inputString.length() && isIdentifierContinuation(inputString[pos]))
            {
                pos++;
                column++;
            }

            std::string identifier = inputString.substr(startPos, pos - startPos);
            Utils::general_log("[lex] Parsed identifier: " + identifier, true);

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
            Utils::general_log("[lex] Starting number at pos " + std::to_string(pos), true);

            while (pos < inputString.length() && (std::isdigit(inputString[pos]) || inputString[pos] == '.'))
            {
                if (inputString[pos] == '.')
                {
                    if (isFloat)
                    {
                        Utils::general_log("[lex] Second decimal point detected, breaking float parse.", true);
                        break;
                    }
                    isFloat = true;
                }
                pos++;
                column++;
            }

            std::string number = inputString.substr(startPos, pos - startPos);
            Utils::general_log("[lex] Parsed number: " + number, true);
            addToken(isFloat ? TK_Float : TK_Integer, number, startColumn);
            continue;
        }

        if (currentChar == '"')
        {
            size_t startPos = pos;
            int startColumn = column;
            Utils::general_log("[lex] Starting string literal.", true);
            pos++;
            column++;

            while (pos < inputString.length() && inputString[pos] != '"')
            {
                if (inputString[pos] == '\\' && pos + 1 < inputString.length())
                {
                    Utils::general_log("[lex] Escape sequence found.", true);
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
                Utils::general_log("[lex] Unterminated string literal!", true);
                break;
            }

            pos++;
            column++;
            std::string str = inputString.substr(startPos, pos - startPos);
            Utils::general_log("[lex] Parsed string: " + str, true);
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
                Utils::general_log("[lex] Parsed double operator: " + doubleOp, true);
                addToken(it->second, doubleOp, column);
                pos += 2;
                column += 2;
                continue;
            }
        }

        auto it = operators.find(op);
        if (it != operators.end())
        {
            Utils::general_log("[lex] Parsed single operator: " + op, true);
            addToken(it->second, op, column);
            pos++;
            column++;
            continue;
        }

        switch (currentChar)
        {
        case ';':
            Utils::general_log("[lex] Parsed semicolon.", true);
            addToken(TK_Semicolon, ";", column);
            break;
        case '(':
            Utils::general_log("[lex] Parsed open parenthesis.", true);
            addToken(TK_OpenParen, "(", column);
            break;
        case ')':
            Utils::general_log("[lex] Parsed close parenthesis.", true);
            addToken(TK_CloseParen, ")", column);
            break;
        case '{':
            Utils::general_log("[lex] Parsed open brace.", true);
            addToken(TK_OpenBrace, "{", column);
            break;
        case '}':
            Utils::general_log("[lex] Parsed close brace.", true);
            addToken(TK_CloseBrace, "}", column);
            break;
        case ',':
            Utils::general_log("[lex] Parsed comma.", true);
            addToken(TK_Comma, ",", column);
            break;
        default:
            Utils::general_log("[lex] Unknown character encountered: " + std::string(1, currentChar), true);
            addToken(TK_Unknown, std::string(1, currentChar), column);
            break;
        }
        pos++;
        column++;
    }

    Utils::general_log("[lex] Lexical analysis complete. Total tokens: " + std::to_string(tokens.size()), true);
    return tokens;
}


void display_tokens(const std::vector<Token> &tokens)
{
    Utils::general_log("[display_tokens] Displaying tokens:", true);
    for (const auto &token : tokens)
    {
        std::string tokenType;
        switch (token.type)
        {
        case TK_Identifier:
            tokenType = "Identifier";
            break;
        case TK_Argument:
            tokenType = "Argument";
            break;
        case TK_String:
            tokenType = "String";
            break;
        case TK_Semicolon:
            tokenType = "Semicolon";
            break;
        case TK_Integer:
            tokenType = "Integer";
            break;
        case TK_Float:
            tokenType = "Float";
            break;
        case TK_MathOperator:
            tokenType = "MathOperator";
            break;
        case TK_EqualsSign:
            tokenType = "EqualsSign";
            break;
        case TK_TypeInteger:
            tokenType = "TypeInteger";
            break;
        case TK_TypeChar:
            tokenType = "TypeChar";
            break;
        case TK_TypeFloat:
            tokenType = "TypeFloat";
            break;
        case TK_TypeString:
            tokenType = "TypeString";
            break;
        case TK_OpenParen:
            tokenType = "OpenParen";
            break;
        case TK_CloseParen:
            tokenType = "CloseParen";
            break;
        case TK_OpenBrace:
            tokenType = "OpenBrace";
            break;
        case TK_CloseBrace:
            tokenType = "CloseBrace";
            break;
        case TK_Comma:
            tokenType = "Comma";
            break;
        case TK_LogicalOperator:
            tokenType = "LogicalOperator";
            break;
        case TK_ComparisonOperator:
            tokenType = "ComparisonOperator";
            break;
        case TK_Unknown:
            tokenType = "Unknown";
            break;
        }
        Utils::general_log("Token at line " + std::to_string(token.line) +
                           ", columns " + std::to_string(token.start_column) +
                           "-" + std::to_string(token.end_column) + ": " +
                           tokenType + ": " + token.value, true);
    }
}

/**
 * @brief Save the generated tokens to a file
 * @param tokens Vector of tokens to be saved
 */
void save_tokens(const std::vector<Token> &tokens, const std::string &filename)
{
    Utils::general_log("[save_tokens] Saving tokens:", true);
    std::ofstream outputFile(filename, std::ios::app);
    if (!outputFile.is_open())
    {
        Utils::general_log("[save_tokens] Failed to open file.", true);
        return;
    }
    Utils::general_log("[save_tokens] Successfully opened file.", true);
    outputFile << "[\n";
    for (const auto &token : tokens)
    {
        Utils::general_log("[save_tokens] Writing token to file:", true);
        outputFile << "  {\n";
        outputFile << "    \"line\": " << token.line << ",\n";
        outputFile << "    \"start_column\": " << token.start_column << ",\n";
        outputFile << "    \"end_column\": " << token.end_column << ",\n";
        outputFile << "    \"type\": \"" << static_cast<int>(token.type) << "\",\n";
        outputFile << "    \"value\": \"" << token.value << "\"\n";
        outputFile << "  },\n";
        Utils::general_log("[save_tokens] Finished writing token to file.", true);
    }
    outputFile << "]\n";
    outputFile.close();
    Utils::general_log("[save_tokens] Successfully closed the file.", true);
}