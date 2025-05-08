#pragma once

#include <string>

enum class TokenType {
    Number,
    String,
    Identifier,
    Keyword,
    Symbol,
    EndOfLine,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
};
