#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>

/**
 * @brief Enum of all Token Types
 */
enum class TokenType
{
    // literals
    Identifier,
    Integer,
    Float,
    String,

    // arithmetic
    Plus,
    Minus,
    Star,
    Slash,
    Caret,
    
    // comparison
    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    
    // punctuation
    LeftParen,
    RightParen,
    Comma,
    Semicolon,
    
    // keywords
    Print,
    Let,
    If,
    Then,
    Goto,
    Input,
    For,
    To,
    Step,
    Next,
    Run,
    List,
    New,
    Return,
    End,
    And,
    Or,
    Not,
    Mod,
    
    EndOfLine,
    Eof,
    Error
};

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
}