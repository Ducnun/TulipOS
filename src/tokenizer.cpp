#include "tokenizer.h"

std::vector<Token> Tokenizer::tokenize(const std::string& input){
    std::vector<Token> tokens;
    size_t i = 0;
    size_t inputSize = input.size();

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
            i++;    // Skip opening parentheses
            
            while (i < inputSize && input[i] != '"'){
                str += input[i++];
            }
            if (input[i++] != '"'){
                tokens.push_back({TokenType::Unknown, str});
            }
            tokens.push_back({TokenType::String, str});
            continue;
        }
        
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

        // Build compound symbols where applicable
        char currentChar = input[i];
        if (currentChar == '<' || currentChar == '>' || currentChar == '='){
            std::string op(1, currentChar);
            // Peek next for compound symbol
            if (i + 1 < inputSize){
                char nextChar = input[i + 1];
                if ((currentChar == '<' && (nextChar == '>' || nextChar == '=')) ||
                    currentChar == '>' && nextChar == '='){
                        op += nextChar;
                        i++;
                    }
            }
            if (Token::isSymbol(op)){
                tokens.push_back(Token{TokenType::Symbol, op});
                i++;
                continue;
            }
        }
        
        // Handle single char symbols
        if (Token::isSymbol(std::string(1, input[i]))){
            tokens.push_back({TokenType::Symbol, std::string(1, input[i++])});
            continue;
        }

        // Unrecognized token
        std::string unknown(1, input[i]);
        tokens.push_back({TokenType::Unknown, unknown});
        i++;
    }

    tokens.push_back({TokenType::EndOfLine, ""});

    // // DEBUGGING OUTPUT
    // for (const auto& token : tokens) {
    //     std::cout << "[Token] Type: ";
    
    //     switch (token.type) {
    //         case TokenType::Keyword: std::cout << "Keyword"; break;
    //         case TokenType::Identifier: std::cout << "Identifier"; break;
    //         case TokenType::Number: std::cout << "Number"; break;
    //         case TokenType::String: std::cout << "String"; break;
    //         case TokenType::Symbol: std::cout << "Symbol"; break;
    //         case TokenType::EndOfLine: std::cout << "EndOfLine"; break;
    //     }
    
    //     std::cout << ", Value: \"" << token.value << "\"\n";
    // }

    return tokens;
}