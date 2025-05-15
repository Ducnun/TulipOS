#include "parser.h"
#include <iostream>

ParsedStatement Parser::parse(void){
    // Make new stmt struct, return early if tokens is empty
    ParsedStatement stmt;
    if(_tokens.empty()) return stmt;

    size_t i = 0;
    Token token = _tokens[i];

    // Build PRINT statement
    if (Token::isKeyword(token.value, "PRINT")){
        stmt.type = StatementType::Print;
        i++;
        while (i < _tokens.size() && _tokens[i].type != TokenType::EndOfLine){
            stmt.printTokens.push_back(_tokens[i++]);
        }
        return stmt;
    }

    // Build LET statement
    if (Token::isKeyword(token.value, "LET")){
        stmt.type = StatementType::Assignment;
        i++;
        if (i < _tokens.size() && _tokens[i].type == TokenType::Identifier){
            stmt.identifier = _tokens[i++].value;
            if (Token::isKeyword(stmt.identifier)){
                throw std::runtime_error("Ident can't shadow keyword");
            }
            if (i < _tokens.size() && _tokens[i].value == "=") i++;
            while (i < _tokens.size() && _tokens[i].type != TokenType::EndOfLine){
                stmt.expressionTokens.push_back(_tokens[i++]);
            }
        }
        return stmt;
    }
}