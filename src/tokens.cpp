#include "tokens.h"

std::string Token::toString() const {
    switch (type) {
        case TokenType::Number:    return "Token Type: NUMBER, Token Value: " + value + "\n";
        case TokenType::String:    return "Token Type: STRING, Token Value: " + value + "\n";
        case TokenType::Identifier:return "Token Type: IDENTIFIER, Token Value: " + value + "\n";
        case TokenType::Keyword:   return "Token Type: KEYWORD, Token Value: " + value + "\n";
        case TokenType::Symbol:    return "Token Type: SYMBOL, Token Value: " + value + "\n";
        case TokenType::EndOfLine: return "Token Type: ENDOFLINE\n";
        case TokenType::Unknown:   return "Token Type: UNKNOWN\n";
        default:                   return "Token Type: INVALID\n";
    }
}
