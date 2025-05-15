#pragma once

#include <string>
#include <unordered_set>

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

    // Static keyword list
    static const std::unordered_set<std::string>& keywordList();

    // Static checker
    // TODO: Overload to check for specific tokens
    static bool isKeyword(const std::string& word);
};