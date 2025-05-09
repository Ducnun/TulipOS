#include "parser.h"

void Parser::parse(void){
    size_t i = 0;
    size_t tokenSize = _tokens.size();

    while(i < tokenSize){
        Token& token = _tokens[i];

        // PRINT KEYWORD
        if (token.type == TokenType::Keyword && token.value == "PRINT"){
            i++;
            // Print value if next token is of type string
            if (i < tokenSize && _tokens[i].type == TokenType::String){
                _io.print(_tokens[i].value);
                _io.print("\n");
            }
            // Retrieve value of variable if tokentype is identifier
            if (i < tokenSize && _tokens[i].type == TokenType::Identifier){
                if (_variables.count(_tokens[i].value)){
                    _io.print(std::to_string(_variables[_tokens[i++].value]));
                    _io.print("\n");
                }
                else {
                    _io.print("?UNDEFINED VARIABLE ERROR\n");
                }
            }
        }

        // LET KEYWORD
        if (token.type == TokenType::Keyword && token.value == "LET"){
            i++;
            // Check that next token is a valid identifier and it is properly assigned
            if (i < tokenSize && _tokens[i].type == TokenType::Identifier) {
                std::string varName = _tokens[i++].value;

                if (i < tokenSize && _tokens[i].value == "="){
                    i++;
                    
                    // Store value in variable map
                    if (i < tokenSize && _tokens[i].type == TokenType::Number) {
                        int value = std::stoi(_tokens[i].value);
                        _variables[varName] = value;
                    }
                }
            }
        }

        i++;
    }
}