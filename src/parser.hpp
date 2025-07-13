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

#include "tokens.hpp"
#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <string>
#include <iomanip>

struct ASTNode;
struct Expr;
struct Stmt;

struct ASTNode {
    virtual ~ASTNode() = default;
};

struct Expr : public ASTNode {
    virtual ~Expr() = default;
};

struct Stmt : public ASTNode {
    virtual ~Stmt() = default;
};

struct BinaryExpr : public Expr {
    Token op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    BinaryExpr(Token op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
};

struct UnaryExpr : public Expr {
    Token op;
    std::unique_ptr<Expr> right;

    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}
};

struct Literal : public Expr {
    Token value;

    explicit Literal(Token value) : value(value) {}
};

struct Identifier : public Expr {
    Token name;

    explicit Identifier(Token name) : name(name) {}
};

struct VarDeclaration : public Stmt {
    Token type;
    Token name;
    std::unique_ptr<Expr> initializer;

    VarDeclaration(Token type, Token name, std::unique_ptr<Expr> initializer)
        : type(type), name(name), initializer(std::move(initializer)) {}
};

struct ExprStmt : public Stmt {
    std::unique_ptr<Expr> expr;
    explicit ExprStmt(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {}
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    
    std::vector<std::unique_ptr<Stmt>> parse();
    std::string printAST(const std::vector<std::unique_ptr<Stmt>>& statements);
    
private:
    bool isAtEnd() const;
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool check(TokenKind type) const;
    bool match(TokenKind type);
    bool match(const std::vector<TokenKind>& types);
    
    Token consume(TokenKind type, const std::string& message);
    void error(const Token& token, const std::string& message);
    void synchronize();
    
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
    
    std::vector<Token> tokens;
    size_t current = 0;
};

class ASTPrinter {
public:
    std::string print(const std::vector<std::unique_ptr<Stmt>>& statements);
    
private:
    std::string printStmt(const Stmt* stmt);
    std::string printExpr(const Expr* expr);
    std::string parenthesize(const std::string& name, const std::vector<const Expr*>& exprs);
    
    int indentLevel = 0;
    
    std::string makeIndent() {
        return std::string(indentLevel * 2, ' ');
    }
};