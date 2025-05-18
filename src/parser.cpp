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
        // Build print tokens
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
            // Check validity
            if (Token::isKeyword(stmt.identifier)){
                throw std::runtime_error("Ident can't shadow keyword");
            }
            // Skip equal sign if needed, build tokens
            if (i < _tokens.size() && _tokens[i].value == "=") i++;
            while (i < _tokens.size() && _tokens[i].type != TokenType::EndOfLine){
                stmt.expressionTokens.push_back(_tokens[i++]);
            }
        }
        return stmt;
    }

    // Build IF .. THEN statement
    if (Token::isKeyword(token.value, "IF")){
        stmt.type = StatementType::IfThen;
        i++;
        // Build if tokens
        while (i < _tokens.size() && _tokens[i].value != "THEN" && _tokens[i].type != TokenType::EndOfLine){
            stmt.conditionTokens.push_back(_tokens[i++]);
        }
        // Check if its followed by a THEN statement
        if (_tokens[i].value != "THEN") throw std::runtime_error("IF conditional not followed by THEN statement");
        else i++;
        // Construct then tokens
        while (i < _tokens.size() && _tokens[i].type != TokenType::EndOfLine){
            stmt.thenTokens.push_back(_tokens[i++]);
        }
        return stmt;
    }

    if (Token::isKeyword(token.value, "GOTO")){
        stmt.type = StatementType::Goto;
        i++;
        while (_tokens[i].type == TokenType::EndOfLine){
            stmt.expressionTokens.push_back(_tokens[i++]);
            std::cout << "Tokens: " << _tokens[i++].toString() << "\n";
        }
        return stmt;
    }

    return stmt;
}