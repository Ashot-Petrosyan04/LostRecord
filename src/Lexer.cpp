#include "Lexer.h"
#include <iostream>
#include <unordered_map>

std::string to_string(TokenType type) {
    static const std::unordered_map<TokenType, std::string> typeStrings = {
        {TokenType::KEYWORD, "KEYWORD"}, 
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::INT_LITERAL, "INT_LITERAL"},
        {TokenType::FLOAT_LITERAL, "FLOAT_LITERAL"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"}, 
        {TokenType::BOOL_LITERAL, "BOOL_LITERAL"},
        {TokenType::PERIOD, "PERIOD"}, 
        {TokenType::COMMA, "COMMA"}, 
        {TokenType::COLON, "COLON"},
        {TokenType::SINGLE_QUOTE, "SINGLE_QUOTE"}, 
        {TokenType::L_PAREN, "L_PAREN"},
        {TokenType::R_PAREN, "R_PAREN"}, 
        {TokenType::END_OF_FILE, "EOF"}, 
        {TokenType::UNKNOWN, "UNKNOWN"}
    };
    auto it = typeStrings.find(type);
    return it == typeStrings.end() ? "UNKNOWN" : it->second;
}

Lexer::Lexer(const std::string& source) : m_source(source) {}

std::vector<Token> Lexer::scanTokens() 
{
    while (!isAtEnd()) 
    {
        m_start = m_current;
        scanToken();
    }
    m_tokens.push_back({TokenType::END_OF_FILE, "", "", m_line});
    return m_tokens;
}

bool Lexer::isAtEnd() 
{ 
    return m_current >= m_source.length(); 
}
char Lexer::advance() 
{
    return m_source[m_current++]; 
}
char Lexer::peek() 
{ 
    if (isAtEnd())
    { 
        return '\0';
    }

    return m_source[m_current]; 
}
char Lexer::peekNext() 
{ 
    if (m_current + 1 >= m_source.length())
    { 
        return '\0'; 
    }

    return m_source[m_current + 1]; 
}

void Lexer::addToken(TokenType type) 
{
    addToken(type, "");
}

void Lexer::addToken(TokenType type, const std::string& literal) 
{
    std::string text = m_source.substr(m_start, m_current - m_start);
    m_tokens.push_back({type, text, literal, m_line});
}

void Lexer::handleIdentifier() 
{
    while (isalnum(peek()) || peek() == '_') 
    {
        advance();
    }
    
    std::string text = m_source.substr(m_start, m_current - m_start);
    
    if (text == "true") 
    {
        addToken(TokenType::BOOL_LITERAL, "true");
    } 
    else if (text == "false") 
    {
        addToken(TokenType::BOOL_LITERAL, "false");
    } 
    else 
    {
        addToken(TokenType::KEYWORD); 
    }
}

void Lexer::handleNumber()
{
    while (isdigit(peek()))
    { 
        advance();
    }

    if (peek() == '.' && isdigit(peekNext()))
    {
        advance();
        while (isdigit(peek()))
        {
            advance();
        }
    }

    std::string num_text = m_source.substr(m_start, m_current - m_start);

    if (num_text.find('.') != std::string::npos) 
    {
        addToken(TokenType::FLOAT_LITERAL, num_text);
    } 
    else 
    {
        addToken(TokenType::INT_LITERAL, num_text);
    }
}

void Lexer::handleString() 
{
    while (peek() != '"' && !isAtEnd()) 
    {
        if (peek() == '\n')
        {
            m_line++;
        }

        advance();
    }

    if (isAtEnd()) 
    {
        std::cerr << "Line " << m_line << ": Error: Unterminated string." << std::endl;
        return;
    }

    advance();
    
    std::string value = m_source.substr(m_start + 1, m_current - m_start - 2);
    addToken(TokenType::STRING_LITERAL, value);
}

void Lexer::scanToken() 
{
    char c = advance();
    switch (c) 
    {
        case '(': 
            addToken(TokenType::L_PAREN); 
            break;
        case ')': 
            addToken(TokenType::R_PAREN); 
            break;
        case '.': 
            addToken(TokenType::PERIOD); 
            break;
        case ',': 
            addToken(TokenType::COMMA); 
            break;
        case ':': 
            addToken(TokenType::COLON);
            break;
        case '\'': 
            addToken(TokenType::SINGLE_QUOTE); 
            break;
        case ' ': 
        case '\r': 
        case '\t': 
            break;
        case '\n': 
            m_line++; 
            break;
        case '"': 
            handleString();
            break;
        case '/':
            if (peek() == '/') 
            {
                while (peek() != '\n' && !isAtEnd()) 
                { 
                    advance();
                }
            }
            else
            {
                addToken(TokenType::UNKNOWN);
            }
            break;
        default:
            if (isalpha(c)) 
            {
                handleIdentifier();
            } 
            else if (isdigit(c)) 
            {
                handleNumber();
            } 
            else 
            {
                std::cerr << "Line " << m_line << ": Error: Unexpected character '" << c << "'" << std::endl;
                addToken(TokenType::UNKNOWN);
            }
            break;
    }
}