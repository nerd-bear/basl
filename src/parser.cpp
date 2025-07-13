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

#include "parser.hpp"
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse()
{
    std::cout << "[Parser] Starting parse with " << tokens.size() << " tokens\n";
    std::vector<std::unique_ptr<Stmt>> statements;
    int loopCount = 0;
    const int MAX_LOOPS = 100;

    while (!isAtEnd() && loopCount < MAX_LOOPS)
    {
        loopCount++;
        std::cout << "[Parser] Token #" << current << ": '"
                  << peek().value << "' (type: "
                  << static_cast<int>(peek().type) << ")\n";

        try
        {
            auto stmt = declaration();
            if (stmt)
            {
                statements.push_back(std::move(stmt));
            }
        }
        catch (const std::runtime_error &e)
        {
            std::cerr << "[Parser] Error: " << e.what() << std::endl;
            synchronize();
        }

        if (current >= tokens.size())
        {
            std::cout << "[Parser] Reached end of token stream\n";
            break;
        }
    }

    if (loopCount >= MAX_LOOPS)
    {
        std::cerr << "[Parser] WARNING: Loop limit reached. Possible infinite loop\n";
    }

    std::cout << "[Parser] Finished parsing. Found "
              << statements.size() << " statements\n";
    return statements;
}

bool Parser::isAtEnd() const
{
    return current >= tokens.size() || tokens[current].type == TK_EOF;
}

const Token &Parser::peek() const
{
    return tokens[current];
}

const Token &Parser::previous() const
{
    return tokens[current - 1];
}

const Token &Parser::advance()
{
    if (!isAtEnd())
    {
        std::cout << "[Parser] Advancing from token #" << current << ": '"
                  << tokens[current].value << "'\n";
        current++;
    }
    return previous();
}

bool Parser::check(TokenKind type) const
{
    if (isAtEnd())
        return false;
    return peek().type == type;
}

bool Parser::match(TokenKind type)
{
    if (check(type))
    {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(const std::vector<TokenKind> &types)
{
    for (TokenKind type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenKind type, const std::string &message)
{
    if (check(type))
        return advance();
    error(peek(), message);
    throw std::runtime_error(message);
}

void Parser::error(const Token &token, const std::string &message)
{
    std::cerr << "[Line " << token.line << "] Error at '" << token.value << "': " << message << std::endl;
}

void Parser::synchronize()
{
    advance();

    while (!isAtEnd())
    {
        if (previous().type == TK_Semicolon)
            return;

        switch (peek().type)
        {
        case TK_TypeInteger:
        case TK_TypeFloat:
        case TK_TypeChar:
        case TK_TypeString:
        case TK_Identifier:
            return;
        default:
            advance();
        }
    }
}

std::unique_ptr<Stmt> Parser::declaration()
{
    try
    {
        if (match({TK_TypeInteger, TK_TypeFloat, TK_TypeChar, TK_TypeString}))
        {
            return varDeclaration();
        }
        return statement();
    }
    catch (const std::runtime_error &)
    {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::varDeclaration()
{
    Token type = previous();
    Token name = consume(TK_Identifier, "Expected variable name after type");

    std::unique_ptr<Expr> initializer = nullptr;
    if (match(TK_EqualsSign))
    {
        initializer = expression();
    }

    consume(TK_Semicolon, "Expected ';' after variable declaration");
    return std::make_unique<VarDeclaration>(type, name, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::statement()
{
    if (isAtEnd())
    {
        return nullptr;
    }

    if (match(TK_Semicolon))
    {
        return nullptr;
    }

    try
    {
        auto expr = expression();
        if (match(TK_Semicolon))
        {
            return std::make_unique<ExprStmt>(std::move(expr));
        }
        else
        {
            error(peek(), "Expected ';' after expression");
            synchronize();
            return nullptr;
        }
    }
    catch (const std::runtime_error &e)
    {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::expression()
{
    return equality();
}

std::unique_ptr<Expr> Parser::equality()
{
    auto expr = comparison();

    while (match({TK_ComparisonOperator}))
    {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison()
{
    auto expr = term();

    while (match({TK_ComparisonOperator}))
    {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term()
{
    auto expr = factor();

    while (match(TK_MathOperator))
    {
        std::string op_val = previous().value;
        if (op_val == "+" || op_val == "-")
        {
            Token op = previous();
            auto right = factor();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        else
        {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor()
{
    auto expr = unary();

    while (match(TK_MathOperator))
    {
        std::string op_val = previous().value;
        if (op_val == "*" || op_val == "/" || op_val == "%")
        {
            Token op = previous();
            auto right = unary();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        else
        {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary()
{
    if (match(TK_LogicalOperator))
    {
        Token op = previous();
        auto right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    return primary();
}

std::unique_ptr<Expr> Parser::primary()
{
    if (match({TK_Integer, TK_Float, TK_String}))
    {
        return std::make_unique<Literal>(previous());
    }

    if (match(TK_Identifier))
    {
        return std::make_unique<Identifier>(previous());
    }

    if (match(TK_OpenParen))
    {
        auto expr = expression();
        consume(TK_CloseParen, "Expected ')' after expression");
        return expr;
    }

    error(peek(), "Expected expression");
    throw std::runtime_error("Expected expression");
}

std::string Parser::printAST(const std::vector<std::unique_ptr<Stmt>> &statements)
{
    ASTPrinter printer;
    return printer.print(statements);
}

std::string ASTPrinter::print(const std::vector<std::unique_ptr<Stmt>> &statements)
{
    std::string result = "Abstract Syntax Tree:\n";
    for (const auto &stmt : statements)
    {
        result += printStmt(stmt.get()) + "\n";
    }
    return result;
}

std::string ASTPrinter::printStmt(const Stmt *stmt)
{
    if (const VarDeclaration *varDecl = dynamic_cast<const VarDeclaration *>(stmt))
    {
        std::string output = makeIndent() + "VarDeclaration: " + varDecl->type.value + " " + varDecl->name.value;

        if (varDecl->initializer)
        {
            indentLevel++;
            output += "\n" + makeIndent() + "Initializer: " + printExpr(varDecl->initializer.get());
            indentLevel--;
        }

        return output;
    }
    else if (const ExprStmt *exprStmt = dynamic_cast<const ExprStmt *>(stmt))
    {
        std::string output = makeIndent() + "ExpressionStatement:";
        if (exprStmt->expr)
        {
            indentLevel++;
            output += "\n" + makeIndent() + printExpr(exprStmt->expr.get());
            indentLevel--;
        }
        return output;
    }

    return makeIndent() + "Unknown Statement";
}

std::string ASTPrinter::printExpr(const Expr *expr)
{
    if (const BinaryExpr *binary = dynamic_cast<const BinaryExpr *>(expr))
    {
        std::vector<const Expr *> exprs = {binary->left.get(), binary->right.get()};
        return parenthesize(binary->op.value, exprs);
    }
    else if (const UnaryExpr *unary = dynamic_cast<const UnaryExpr *>(expr))
    {
        std::vector<const Expr *> exprs = {unary->right.get()};
        return parenthesize(unary->op.value, exprs);
    }
    else if (const Literal *literal = dynamic_cast<const Literal *>(expr))
    {
        return "(literal " + literal->value.value + ")";
    }
    else if (const Identifier *ident = dynamic_cast<const Identifier *>(expr))
    {
        return "(id " + ident->name.value + ")";
    }

    return "(unknown_expr)";
}

std::string ASTPrinter::parenthesize(const std::string &name, const std::vector<const Expr *> &exprs)
{
    std::string output = "(" + name;

    {
        indentLevel++;
        for (const Expr *expr : exprs)
        {
            output += "\n" + makeIndent() + printExpr(expr);
        }
        indentLevel--;
    }

    output += ")";
    return output;
}