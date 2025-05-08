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

class Token {
public:
    ~Token() = default;
    TokenType type;
    std::string value;
    std::string toString() const;
};
