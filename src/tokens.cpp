#include "tokens.h"

std::string Token::toString() const {
    switch (type) {
        case TokenType::Float:    return "Token Type: FLOAT, Token Value: " + value + "\n";
        case TokenType::Integer:    return "Token Type: INTEGER, Token Value: " + value + "\n";
        case TokenType::String:    return "Token Type: STRING, Token Value: " + value + "\n";
        case TokenType::Identifier:return "Token Type: IDENTIFIER, Token Value: " + value + "\n";
        case TokenType::Keyword:   return "Token Type: KEYWORD, Token Value: " + value + "\n";
        case TokenType::Symbol:    return "Token Type: SYMBOL, Token Value: " + value + "\n";
        case TokenType::EndOfLine: return "Token Type: ENDOFLINE\n";
        case TokenType::UnknownTokenType:   return "Token Type: UNKNOWN\n";
        default:                   return "Token Type: INVALID\n";
    }
}

const std::unordered_set<std::string>& Token::keywordList(){
    static const std::unordered_set<std::string> keywords = {
        "PRINT", "RUN", "LET", "NEW", "LIST", "INPUT", "GOTO", "IF", "THEN", "FOR", "TO", "NEXT", "STEP", "RETURN", 
        "END",
    };
    return keywords;
}

const std::unordered_set<std::string>& Token::symbolList(){
    static const std::unordered_set<std::string> symbols = {
        "+", "-", "*", "/", "=", "<", ">", "<=", ">=", "<>", "(", ")",
        "AND", "OR",
        ";", ","
    };
    return symbols;
}


bool Token::isKeyword(const std::string& word){
    return keywordList().count(word) > 0;
}

bool Token::isKeyword(const std::string& word, const std::string& target){
    return keywordList().count(word) > 0 && word == target;
}

bool Token::isSymbol(const std::string& symbol){
    return symbolList().count(symbol) > 0;
}

KeywordType Token::getKeywordType(const std::string& word){
    static const std::unordered_map<std::string, KeywordType> keywordMap = {
        {"PRINT", KeywordType::Print},
        {"LET", KeywordType::Let},
        {"IF", KeywordType::If},
        {"THEN", KeywordType::Then},
        {"GOTO", KeywordType::Goto},
        {"INPUT", KeywordType::Input},
        {"FOR", KeywordType::For},
        {"TO", KeywordType::To},
        {"NEXT", KeywordType::Next},
        {"STEP", KeywordType::Step}
    };
    auto it = keywordMap.find(word);
    return (it != keywordMap.end()) ? it->second : KeywordType::UnknownKeywordType;
}