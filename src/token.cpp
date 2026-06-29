#include "tokenizer/token.h"

std::string TokenUtils::typeToString(TokenType type)
{
    switch (type)
    {
        case TokenType::Identifier: return "Identifier";
        case TokenType::Integer: return "Integer";
        case TokenType::Float: return "Float";
        case TokenType::String: return "String";

        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Star: return "Star";
        case TokenType::Slash: return "Slash";
        case TokenType::Caret: return "Caret";

        case TokenType::Equal: return "Equal";
        case TokenType::NotEqual: return "NotEqual";
        case TokenType::Less: return "Less";
        case TokenType::LessEqual: return "LessEqual";
        case TokenType::Greater: return "Greater";
        case TokenType::GreaterEqual: return "GreaterEqual";
        
        case TokenType::LeftParen: return "LeftParen";
        case TokenType::RightParen: return "RightParen";
        case TokenType::Comma: return "Comma";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::LeftBrace: return "LeftBrace";
        case TokenType::RightBrace: return "RightBrace";
        
        case TokenType::Print: return "Print";
        case TokenType::Let: return "Let";
        case TokenType::If: return "If";
        case TokenType::Then: return "Then";
        case TokenType::Else: return "Else";
        case TokenType::Goto: return "Goto";
        case TokenType::Input: return "Input";
        case TokenType::For: return "For";
        case TokenType::To: return "To";
        case TokenType::Step: return "Step";
        case TokenType::Next: return "Next";
        case TokenType::Run: return "Run";
        case TokenType::List: return "List";
        case TokenType::New: return "New";
        case TokenType::Return: return "Return";
        case TokenType::End: return "End";
        case TokenType::And: return "And";
        case TokenType::Or: return "Or";
        case TokenType::Not: return "Not";
        case TokenType::Mod: return "Mod";
        
        case TokenType::EndOfLine: return "EOL";
        case TokenType::Eof: return "EOF";
        case TokenType::Error: return "Error";

        default: return "Unknown";
    }
}

std::string TokenUtils::toString(const Token& token){
    return "Token(type: " + TokenUtils::typeToString(token.type) + ", value: \"" + token.value + "\"" +  " at: " + std::to_string(token.line) + ")";
}