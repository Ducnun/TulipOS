#include "parser.h"
#include <iostream>

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
                    _io.print(_variables[_tokens[i].value].toString());
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

                // Skip "=" sign
                if (i < tokenSize && _tokens[i].value == "=") i++;

                Token valueToken = _tokens[i];

                // Store value in variable map
                if (i < tokenSize) {
                    std::vector<Token> expressionTokens;
                    while (i < tokenSize && _tokens[i].type != TokenType::EndOfLine){
                        expressionTokens.push_back(_tokens[i++]);
                    }

                    ExpressionEvaluator evaluator(_variables);
                    VariableValue result = evaluator.evaluate(expressionTokens);
                    _variables[varName] = result;
                }
            }
        }

        i++;
    }
}