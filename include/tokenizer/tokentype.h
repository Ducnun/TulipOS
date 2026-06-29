#pragma once

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
    LeftBrace,
    RightBrace,
    Dot,
    
    // keywords
    Print,
    Let,
    If,
    Then,
    Else,
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
    Call,
    
    EndOfLine,
    Eof,
    Error
};