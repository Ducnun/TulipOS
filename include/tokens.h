#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>

/**
 * @brief Enum of all Token Types
 */
enum class TokenType{
    UnknownTokenType,
    Float,
    Integer,
    String,
    Identifier,
    Keyword,
    Symbol,
    EndOfLine,
};

/**
 * @brief Enum of all Keyword Types
 */
enum class KeywordType{
    UnknownKeywordType,
    Print,
    Let,
    If,
    Then,
    Goto,
    Input,
    For,
    To,
    Next,
    Step
};

class Token{
public:
    /**
     * @brief Destroy the Token object
     */
    ~Token() = default;

    TokenType type; ///< The exact type of token
    std::string value;  ///< The value of the token

    /**
     * @brief Returns a formatted string representing token type and value
     * @return std::string formatted string
     */
    std::string toString() const;

    // Static token lists
    static const std::unordered_set<std::string>& keywordList();    ///< Static list of keywords
    static const std::unordered_set<std::string>& symbolList();     ///< Static list of symbols

    // Static checkers

    /**
     * @brief Checks whether an input word is a keyword
     * @param word to be checked
     * @return true exists in the list of keywords
     * @return false does not exist in the list of keywords
     */
    static bool isKeyword(const std::string& word);

    /**
     * @brief Checks whether an input word is a specific keyword
     * @param word to be checked
     * @param target to be checked against
     * @return true if it matches
     * @return false if it doesn't match
     */
    static bool isKeyword(const std::string& word, const std::string& target);

    /**
     * @brief Checks whether an input is a symbol
     * @param word to be checked
     * @return true if it matches a predefined symbol
     * @return false if it does not match
     */
    static bool isSymbol(const std::string& word);

    /**
     * @brief Maps keyword string representations to their matching enum
     * @param word to be converted
     * @return KeywordType representation of the word
     */
    static KeywordType getKeywordType(const std::string& word);
};