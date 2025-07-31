#pragma once
#include <string>

enum class TokenType {
    KEYWORD,
    IDENTIFIER,

    INT_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    BOOL_LITERAL,

    PERIOD,
    COMMA,
    COLON,
    SINGLE_QUOTE,
    L_PAREN,
    R_PAREN,

    END_OF_FILE,

    UNKNOWN
};

std::string to_string(TokenType type);

struct Token {
    TokenType type;
    std::string text;
    std::string literal_value;
    int line;
};