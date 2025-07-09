#pragma once
#include <string>

enum TokenKind
{
    TK_Identifier,
    TK_Argument,
    TK_Integer,
    TK_Float,
    TK_String,

    TK_TypeInteger,
    TK_TypeFloat,
    TK_TypeChar,
    TK_TypeString,

    TK_MathOperator,       //F - * / %
    TK_ComparisonOperator, // == != < > <= >=
    TK_LogicalOperator,    // && || !
    TK_EqualsSign,         // =

    TK_Semicolon,  // ;
    TK_OpenParen,  // (
    TK_CloseParen, // )
    TK_OpenBrace,  // {
    TK_CloseBrace, // }
    TK_Comma,      // ,

    // special dont ask
    TK_Unknown,
    TK_EOF // End of file
};

struct Token
{
    TokenKind type;
    std::string value;
    int line;
    int start_column;
    int end_column;
};