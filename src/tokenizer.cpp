#include "tokenizer.h"
#include <iostream>

std::vector<Token> Tokenizer::tokenize(const std::string& input){
    std::vector<Token> tokens;
    size_t i = 0;
    size_t inputSize = input.size();

    auto isAlnumUnderscore = [](char c){
        return std::isalnum(c) || c == '_' || c == '$' || c == '%';
    };

    while (i < inputSize){
        // Whitespace
        if (std::isspace(input[i])){
            i++;
            continue;
        }

        // Number, default to float so support one "."
        if (std::isdigit(input[i]) || (input[i] == '.' && i + 1 < inputSize && std::isdigit(input[i + 1]))) {
            std::string number;
            bool hasDot = false;

            while (i < inputSize && (std::isdigit(input[i]) || (input[i] == '.' && !hasDot))) {
                if (input[i] == '.') hasDot = true;
                number += input[i++];
            }

            tokens.push_back({TokenType::Number, number});
            continue;
        }

        // String
        if (input[i] == '"'){
            std::string str;
            i++;    // Skip opening quotes
            
            while (i < inputSize && input[i] != '"'){
                str += input[i++];
            }
            if (input[i++] != '"'){
                tokens.push_back({TokenType::UnknownTokenType, str});
            }
            tokens.push_back({TokenType::String, str});
            continue;
        }

        // Check if input is symbol, from large to small char counts
        int symbolCharCounter = 3;
        bool symbolFound = false;
        while (symbolCharCounter > 0 && !symbolFound){
            if (i + symbolCharCounter <= inputSize && Token::isSymbol(input.substr(i, symbolCharCounter))){
                tokens.push_back({TokenType::Symbol, input.substr(i, symbolCharCounter)});
                i += symbolCharCounter;
                symbolFound = true;
            }
            symbolCharCounter--;
        }
        // If symbol was found, stop checking other inputs
        if (symbolFound) continue;

        // Check identifiers, keywords, special characters
        if (std::isalpha(input[i])){
            std::string ident;
            while (i < inputSize && std::isalnum(input[i])){
                ident += input[i++];
            }
            
            if (Token::isKeyword(ident)){
                tokens.push_back({TokenType::Keyword, ident});
            }
            else{
                if (ident.length() > 2){
                    ident = ident.substr(0,2);
                }
                tokens.push_back({TokenType::Identifier, ident});
            }

            continue;
        }

        // Unrecognized token
        std::string unknown(1, input[i]);
        tokens.push_back({TokenType::UnknownTokenType, unknown});
        i++;
    }

    tokens.push_back({TokenType::EndOfLine, ""});

    // DEBUGGING OUTPUT
    for (const auto& token : tokens) {
        std::cout << "[Token] Type: ";
    
        switch (token.type) {
            case TokenType::Keyword: std::cout << "Keyword"; break;
            case TokenType::Identifier: std::cout << "Identifier"; break;
            case TokenType::Number: std::cout << "Number"; break;
            case TokenType::String: std::cout << "String"; break;
            case TokenType::Symbol: std::cout << "Symbol"; break;
            case TokenType::EndOfLine: std::cout << "EndOfLine"; break;
            case TokenType::UnknownTokenType: std::cout << "Unknown"; break;
        }
    
        std::cout << ", Value: \"" << token.value << "\"\n";
    }

    return tokens;
}