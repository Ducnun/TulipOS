#include "parser.h"
#include <iostream>

void Parser::parse(void){
    size_t i = 0;
    size_t tokenSize = _tokens.size();

    while(i < tokenSize){
        Token& token = _tokens[i];

        // PRINT KEYWORD
        if (Token::isKeyword(token.value, "PRINT")){
            i++;
            
            std::vector<Token> printTokens;
            while (i < tokenSize && _tokens[i].type != TokenType::EndOfLine){
                printTokens.push_back(_tokens[i]);
                i++;
            }

            ExpressionEvaluator evaluator(_variables);
            VariableValue result = evaluator.evaluate(printTokens);
            _io.print(result.toString() + "\n");
        }

        // LET KEYWORD
        if (Token::isKeyword(token.value, "LET")){
            i++;
            // Check that next token is a valid identifier and it is properly assigned
            if (i < tokenSize && _tokens[i].type == TokenType::Identifier) {
                std::string varName = _tokens[i++].value;

                std::cout << "Token being parsed: " << _tokens[i].toString();

                // Check that varname doesnt shadow keyword
                if (Token::isKeyword(varName)){
                    throw std::runtime_error("Ident can't shadow keyword");
                }

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