#pragma once

#include <vector>
#include <memory>
#include "Token.h"
#include "AST.h"

class Parser 
{
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    const std::vector<Token>& m_tokens;
    size_t m_current = 0;

    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> whileStatement();
    std::unique_ptr<Stmt> block();
    std::unique_ptr<Stmt> expressionStatement();
    std::unique_ptr<Stmt> printStatement();
    std::unique_ptr<Stmt> procedureDeclaration();
    std::unique_ptr<Stmt> procedureCallStatement();
    std::unique_ptr<Stmt> returnStatement();
    std::unique_ptr<Stmt> breakStatement();
    
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> logic_or();
    std::unique_ptr<Expr> logic_and();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> addition();
    std::unique_ptr<Expr> multiplication();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
    std::unique_ptr<Expr> functionCallExpression();

    bool match(const std::vector<std::string>& texts);
    Token consume(const std::string& expected_text, const std::string& error_message);
    void synchronize();
    Token advance();
    Token previous();
    Token peek();
    bool isAtEnd();
};