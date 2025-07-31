#pragma once

#include <string>
#include <vector>
#include "Token.h"

class Lexer 
{
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    const std::string m_source;
    size_t m_start = 0;
    size_t m_current = 0;
    int m_line = 1;
    std::vector<Token> m_tokens;

    bool isAtEnd();
    void scanToken();
    char advance();
    char peek();
    char peekNext();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);
    void handleIdentifier();
    void handleNumber();
    void handleString();
};