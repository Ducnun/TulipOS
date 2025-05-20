#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>

enum class TokenType{
    Number,
    String,
    Identifier,
    Keyword,
    Symbol,
    EndOfLine,
    Unknown
};

enum class KeywordType{
    Unknown,
    Print,
    Let,
    If,
    Then,
    Goto,
    Input,
};

class Token{
public:
    ~Token() = default;
    TokenType type;
    std::string value;

    std::string toString() const;

    // Static token lists
    static const std::unordered_set<std::string>& keywordList();
    static const std::unordered_set<std::string>& symbolList();

    // Static checkers
    static bool isKeyword(const std::string& word);
    static bool isKeyword(const std::string& word, const std::string& target);
    static bool isSymbol(const std::string& word);
    static KeywordType getKeywordType(const std::string& word);
};