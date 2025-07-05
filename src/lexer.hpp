#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>

typedef enum
{
    TK_Identifier,         ///< Identifiers (variable names, function names, etc.)
    TK_Argument,           ///< Command arguments
    TK_String,             ///< String literals
    TK_Semicolon,          ///< Semicolon (;)
    TK_Integer,            ///< Integer literals
    TK_Float,              ///< Floating-point literals
    TK_MathOperator,       ///< Mathematical operators (+, -, *, /, %)
    TK_EqualsSign,         ///< Equals sign (=)
    TK_TypeInteger,        ///< Integer type keyword
    TK_TypeChar,           ///< Character type keyword
    TK_TypeFloat,          ///< Float type keyword
    TK_TypeString,         ///< String type keyword
    TK_OpenParen,          ///< Opening parenthesis (
    TK_CloseParen,         ///< Closing parenthesis )
    TK_OpenBrace,          ///< Opening brace {
    TK_CloseBrace,         ///< Closing brace }
    TK_Comma,              ///< Comma (,)
    TK_LogicalOperator,    ///< Logical operators (&&, ||, !)
    TK_ComparisonOperator, ///< Comparison operators (==, !=, <, >, <=, >=)
    TK_Unknown             ///< Unknown token type
} TokenKind;

typedef struct
{
    TokenKind type;    ///< Type of the token
    std::string value; ///< Value of the token
    int line;          ///< Line number where the token is found
    int start_column;  ///< Start column of the token
    int end_column;    ///< End column of the token
} Token;

void save_tokens(const std::vector<Token> &tokens, const std::string &filename);
// void display_tokens(const std::vector<Token> &tokens);
std::vector<Token> lex(const std::string &inputString);