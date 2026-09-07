#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>
#include "tokentype.h"
#include "utils/stringview.h"

struct Token{
    TokenType type;     ///< The type of the token
    util::StringView value;
    int line;

    /// @brief Constructor for Token struct
    Token(TokenType type, util::StringView value, int line) : type(type), value(value), line(line) {}
};

namespace TokenUtils{
    /// @brief Converts a Token to a string representation for debugging purposes
    std::string toString(const Token& token);
    std::string typeToString(TokenType type);
}
