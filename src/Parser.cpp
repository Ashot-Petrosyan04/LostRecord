#include "Parser.h"
#include <stdexcept>
#include <iostream>
#include <memory>
#include <algorithm>

bool is_in(const std::string& token_text, const std::vector<std::string>& texts)
{
    return std::find(texts.begin(), texts.end(), token_text) != texts.end();
}

Parser::Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse()
{
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!isAtEnd()) 
    {
        try 
        {
            statements.push_back(statement());
        } 
        catch (const std::runtime_error& e) 
        {
            std::cerr << "Line " << peek().line << ": Parse Error: " << e.what() << std::endl;
            synchronize();
        }
    }

    return statements;
}

std::unique_ptr<Stmt> Parser::statement() 
{
    if (peek().text == "a" && m_tokens[m_current + 1].text == "value") 
    { 
        return declaration(); 
    }
    if (peek().text == "for" && m_tokens.size() > 1 && m_tokens[m_current + 1].text == "procedure") 
    { 
        return procedureDeclaration(); 
    }
    if (peek().text == "perform") 
    { 
        return procedureCallStatement(); 
    }
    if (peek().text == "the" && m_tokens.size() > 1 && m_tokens[m_current + 1].text == "result") 
    { 
        return returnStatement(); 
    }
    if (peek().text == "the" && m_tokens.size() > 1 && m_tokens[m_current + 1].text == "story" && m_tokens[m_current + 2].text == "ends") 
    {
        if (m_tokens.size() > 3 && m_tokens[m_current + 3].text == "at") 
        { 
            return breakStatement();
        }

        return printStatement();
    }
    if (peek().text == "the" && m_tokens.size() > 1 && m_tokens[m_current + 1].text == "story") 
    { 
        return printStatement(); 
    }
    if (peek().text == "if") 
    { 
        return ifStatement();
    }
    if (peek().text == "while") 
    { 
        return whileStatement(); 
    }

    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::breakStatement() 
{
    consume("the", "Expected 'the'.");
    consume("story", "Expected 'story'.");
    consume("ends", "Expected 'ends'.");
    consume("at", "Expected 'at'.");
    consume("this", "Expected 'this'.");
    consume("moment", "Expected 'moment'.");
    consume(".", "Expected '.' after 'the story ends at this moment'.");

    return std::make_unique<BreakStmt>();
}

std::unique_ptr<Stmt> Parser::returnStatement() 
{
    consume("the", "Expected 'the'.");
    consume("result", "Expected 'result'.");
    consume("shall", "Expected 'shall'.");
    consume("be", "Expected 'be'.");
    auto value = expression();
    consume(".", "Expected '.' after return value.");

    return std::make_unique<ReturnStmt>(std::move(value));
}

std::unique_ptr<Stmt> Parser::printStatement() 
{
    consume("the", "Expected 'the'.");
    consume("story", "Expected 'story'.");

    if (peek().text == "tells") 
    {
        advance();
        consume(":", "Expected ':' after 'tells'.");
        auto value = expression();
        consume(".", "Expected '.' after print expression.");
        return std::make_unique<PrintStmt>(std::move(value));
    }

    if (peek().text == "ends") 
    {
        advance();
        consume("a", "Expected 'a'.");
        consume("line", "Expected 'line'.");
        consume(".", "Expected '.' after 'ends a line'.");
        return std::make_unique<NewlineStmt>();
    }

    throw std::runtime_error("Unrecognized story action. Expected 'tells:' or 'ends a line'.");
}

std::unique_ptr<Stmt> Parser::ifStatement() 
{
    consume("if", "Expected 'if'.");
    auto condition = expression();
    
    consume("is", "Expected 'is' after the condition.");
    consume("met", "Expected 'met' after 'is'.");
    consume(",", "Expected ',' after 'met'.");
    consume("tell", "Expected 'tell'.");
    consume("the", "Expected 'the'.");
    consume("following", "Expected 'following'.");
    consume("story", "Expected 'story'.");
    consume(":", "Expected ':' after 'story'.");
    auto thenBranch = block();

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch));
}

std::unique_ptr<Stmt> Parser::whileStatement() 
{
    consume("while", "Expected 'while'.");
    auto condition = expression();
    consume("holds", "Expected 'holds' after condition.");
    consume(",", "Expected ',' after 'holds'.");
    consume("tell", "Expected 'tell'.");
    consume("the", "Expected 'the'.");
    consume("following", "Expected 'following'.");
    consume("story", "Expected 'story'.");
    consume(":", "Expected ':' after 'story'.");
    auto body = block();

    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::block() 
{
    consume("beginning", "Expected 'beginning'.");
    consume("of", "Expected 'of'.");
    consume("the", "Expected 'the'.");
    consume("story", "Expected 'story'.");
    std::vector<std::unique_ptr<Stmt>> statements;

    while (peek().text != "end") 
    {
        statements.push_back(statement());

        if(isAtEnd()) 
        {
            throw std::runtime_error("Unterminated block statement, missing 'end of the story'.");
        }
    }
    
    consume("end", "Expected 'end'.");
    consume("of", "Expected 'of'.");
    consume("the", "Expected 'the'.");
    consume("story", "Expected 'story'.");
    consume(".", "Expected '.' after 'end of the story'.");

    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::declaration() 
{
    advance();
    advance();
    Token name = consume("KEYWORD", "Expected variable name.");
    consume(",", "Expected ','.");
    consume("type", "Expected 'type'.");
    Token type = consume("KEYWORD", "Expected type name.");
    consume(",", "Expected ','.");
    bool is_mutable = false;

    if (match({"begins", "at"})) 
    {
        is_mutable = true;
    } 
    else if (match({"is", "revealed", "as"})) 
    {
        is_mutable = false;
    } 
    else 
    {
        throw std::runtime_error("Expected 'begins at' or 'is revealed as'.");
    }

    auto initializer = expression();
    consume(".", "Expected '.' after declaration.");

    return std::make_unique<DeclarationStmt>(name, type, std::move(initializer), is_mutable);
}

std::unique_ptr<Stmt> Parser::procedureDeclaration() 
{
    consume("for", "Expected 'for'.");
    consume("procedure", "Expected 'procedure'.");
    consume("named", "Expected 'named'.");
    consume("'", "Expected single quote before procedure name.");
    Token name = consume("KEYWORD", "Expected procedure name.");
    consume("'", "Expected single quote after procedure name.");
    std::vector<Param> params;
    consume("accepting", "Expected 'accepting'.");
    consume("(", "Expected '(' for parameter list.");

    if (peek().text != ")") 
    {
        do 
        {
            Token param_name = consume("KEYWORD", "Expected parameter name.");
            consume("as", "Expected 'as'.");
            Token param_type = consume("KEYWORD", "Expected parameter type.");
            params.push_back({param_name, param_type});
        } 
        while (match({","}));
    }

    consume(")", "Expected ')' after parameter list.");
    Token returnType;

    if(match({"and", "yielding"})) 
    {
        returnType = consume("KEYWORD", "Expected return type.");
    }
    
    consume(",", "Expected ',' after procedure header.");
    consume("tell", "Expected 'tell'.");
    consume("the", "Expected 'the'.");
    consume("following", "Expected 'following'.");
    consume("story", "Expected 'story'.");
    consume(":", "Expected ':' after 'story'.");
    auto body = block();

    return std::make_unique<ProcedureDeclStmt>(name, std::move(params), returnType, std::move(body));
}

std::unique_ptr<Stmt> Parser::procedureCallStatement() 
{
    consume("perform", "Expected 'perform'.");
    consume("the", "Expected 'the'.");
    consume("story", "Expected 'story'.");
    consume("of", "Expected 'of'.");
    consume("'", "Expected single quote before procedure name.");
    Token name = consume("KEYWORD", "Expected procedure name to call.");
    consume("'", "Expected single quote after procedure name.");
    std::vector<std::unique_ptr<Expr>> arguments;
    consume("using", "Expected 'using'.");
    consume("(", "Expected '(' for arguments.");

    if (peek().text != ")") 
    {
        do 
        {
            arguments.push_back(expression());
        } 
        while(match({","}));
    }

    consume(")", "Expected ')' after arguments.");
    consume(".", "Expected '.' after procedure call.");

    return std::make_unique<ProcedureCallStmt>(name, std::move(arguments));
}

std::unique_ptr<Stmt> Parser::expressionStatement() 
{
    auto expr = expression();
    consume(".", "Expected '.' after a statement.");
    
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::unique_ptr<Expr> Parser::expression() 
{
    if (peek().text == "the" && m_tokens.size() > m_current + 4 && m_tokens[m_current + 1].text == "value" &&
        m_tokens[m_current + 3].text == "continues" && m_tokens[m_current + 4].text == "as") 
    {
        advance();
        advance();
        Token name = consume("KEYWORD", "Expected variable name in assignment.");
        consume("continues", "Expected 'continues as'.");
        consume("as", "Expected 'continues as'.");
        auto value = expression();

        return std::make_unique<AssignExpr>(name, std::move(value));
    }

    return logic_or();
}

std::unique_ptr<Expr> Parser::logic_or() 
{
    auto expr = logic_and();

    while (match({"or"})) 
    {
        Token op = previous();
        auto right = logic_and();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logic_and() 
{
    auto expr = comparison();

    while (match({"and"})) 
    {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison() 
{
    auto expr = addition();
    while (peek().text == "is") 
    {
        if (m_tokens.size() > m_current + 1 && m_tokens[m_current + 1].text == "met") 
        {
            break; 
        }
        
        Token op = advance();

        if (is_in(peek().text, {"greater", "less", "equal"})) 
        {
            Token op_part2 = advance();
            op.text += " " + op_part2.text;

            if (is_in(op_part2.text, {"greater", "less"})) 
            {
                consume("than", "Expected 'than'.");
                op.text += " than";
            } 
            else 
            {
                consume("to", "Expected 'to'.");
                op.text += " to";
            }

            auto right = addition();
            expr = std::make_unique<ComparisonExpr>(std::move(expr), op, std::move(right));
        } 
        else 
        {
            m_current--; 
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expr> Parser::addition() 
{
    auto expr = multiplication();

    while (peek().text == "plus" || peek().text == "minus") 
    {
        Token op = advance();
        auto right = multiplication();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::multiplication() 
{
    auto expr = unary();

    while (peek().text == "multiplied" || peek().text == "divided") 
    {
        Token op = advance();
        consume("by", "Expected 'by'.");
        auto right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() 
{
    if (match({"not"})) 
    {
        Token op = previous();
        auto right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }

    return primary();
}

std::unique_ptr<Expr> Parser::primary() 
{
    if (peek().type == TokenType::INT_LITERAL || peek().type == TokenType::FLOAT_LITERAL ||
        peek().type == TokenType::STRING_LITERAL || peek().type == TokenType::BOOL_LITERAL) 
    {
        return std::make_unique<LiteralExpr>(advance());
    }
    if (peek().text == "the" && m_tokens.size() > 1 && m_tokens[m_current + 1].text == "story") 
    {
        return functionCallExpression();
    }
    if (peek().type == TokenType::KEYWORD) 
    {
        return std::make_unique<VariableExpr>(advance());
    }

    throw std::runtime_error("Expected an expression, got '" + peek().text + "'.");
}

std::unique_ptr<Expr> Parser::functionCallExpression() 
{
    consume("the", "Expected 'the'.");
    consume("story", "Expected 'story'.");
    consume("of", "Expected 'of'.");
    consume("'", "Expected ' before function name.");
    Token name = consume("KEYWORD", "Expected function name.");
    consume("'", "Expected ' after function name.");
    std::vector<std::unique_ptr<Expr>> arguments;
    consume("using", "Expected 'using'.");
    consume("(", "Expected '(' for arguments.");

    if (peek().text != ")") 
    {
        do 
        {
            arguments.push_back(expression());
        } 
        while(match({","}));
    }

    consume(")", "Expected ')' after arguments.");

    return std::make_unique<FunctionCallExpr>(name, std::move(arguments));
}

bool Parser::match(const std::vector<std::string>& texts) 
{
    for (size_t i = 0; i < texts.size(); ++i) 
    {
        if (m_current + i >= m_tokens.size() || m_tokens[m_current + i].text != texts[i]) 
        {
            return false;
        }
    }

    m_current += texts.size();

    return true;
}

Token Parser::consume(const std::string& expected, const std::string& error_message) 
{
    if (isAtEnd()) 
    {
        throw std::runtime_error("Unexpected end of file. " + error_message);
    }
    if ((expected == "KEYWORD" || expected == "IDENTIFIER") && peek().type == TokenType::KEYWORD) 
    {
        return advance();
    }
    if (peek().text == expected) 
    {
        return advance();
    }

    throw std::runtime_error(error_message + " Got '" + peek().text + "' instead of '" + expected + "'.");
}

void Parser::synchronize() 
{
    advance();

    while (!isAtEnd()) 
    {
        if (previous().text == ".") 
        {
            return;
        }

        const std::string& text = peek().text;
        
        if (is_in(text, {"a", "for", "if", "while", "perform", "the"})) 
        {
            return;
        }

        advance();
    }
}

Token Parser::advance() 
{ 
    if (!isAtEnd())
    { 
        m_current++;
    } 
    
    return previous(); 
}

Token Parser::previous() 
{ 
    return m_tokens[m_current - 1]; 
}

Token Parser::peek() 
{ 
    return m_tokens[m_current]; 
}

bool Parser::isAtEnd() 
{ 
    return peek().type == TokenType::END_OF_FILE; 
}