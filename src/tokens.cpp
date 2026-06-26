#include "tokens.h"

std::string TokenUtils::toString(const Token& token){
    return "Token(type: " + std::to_string(static_cast<int>(token.type)) + ", value: \"" + token.value + "\"" +  " at: " + std::to_string(token.line) + ")";
}