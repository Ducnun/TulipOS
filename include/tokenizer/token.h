#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "tokentype.h"

struct Token{
    TokenType type;     ///< The type of the token
    std::string value;  ///< The string value of the token
    int line;

    /// @brief Constructor for Token struct
    Token(TokenType type, const std::string& value, int line) : type(type), value(value), line(line) {}
};

namespace TokenUtils{
    /// @brief Converts a Token to a string representation for debugging purposes
    std::string toString(const Token& token);
    std::string typeToString(TokenType type);
}
