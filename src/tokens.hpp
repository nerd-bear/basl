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

    TK_MathOperator,       // F - * / %
    TK_ComparisonOperator, // == != < > <= >=
    TK_LogicalOperator,    // && || !
    TK_EqualsSign,         // =

    TK_Semicolon,  // ;
    TK_OpenParen,  // (
    TK_CloseParen, // )
    TK_OpenBrace,  // {
    TK_CloseBrace, // }
    TK_Comma,      // ,
    TK_TypeBool,

    TK_KeywordIf,
    TK_KeywordElse,
    TK_KeywordFor,
    TK_KeywordFunction,
    TK_KeywordReturn,
    TK_KeywordPrint,

    // special
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