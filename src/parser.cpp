#include "parser.h"
#include <iostream>

ParsedStatement Parser::parse(){
    ParsedStatement stmt;
    if(_tokens.empty()) return stmt;

    switch (Token::getKeywordType(_tokens[0].value)){
        case KeywordType::Print: return parsePrint();
        case KeywordType::Let: return parseLet();
        case KeywordType::If: return parseIfThen();
        case KeywordType::Goto: return parseGoto();
        case KeywordType::Input: return parseInput();
        default:
            throw std::runtime_error("Unknown statement type");
    }
}

ParsedStatement Parser::parsePrint(){
    ParsedStatement stmt;
    stmt.type = StatementType::Print;
    size_t i = 1; // Skip PRINT
    
    while (i < _tokens.size() && _tokens[i].type != TokenType::EndOfLine) {
        stmt.printTokens.push_back(_tokens[i++]);
    }
    return stmt;
}

ParsedStatement Parser::parseLet() {
    ParsedStatement stmt;
    stmt.type = StatementType::Assignment;
    size_t i = 1; // Skip LET
    
    if (i < _tokens.size() && _tokens[i].type == TokenType::Identifier) {
        stmt.identifier = _tokens[i++].value;
        if (Token::isKeyword(stmt.identifier)) {
            throw std::runtime_error("Ident can't shadow keyword");
        }
        
        if (i < _tokens.size() && _tokens[i].value == "=") i++;
        
        while (i < _tokens.size() && _tokens[i].type != TokenType::EndOfLine) {
            stmt.expressionTokens.push_back(_tokens[i++]);
        }
    }
    return stmt;
}

ParsedStatement Parser::parseIfThen() {
    ParsedStatement stmt;
    stmt.type = StatementType::IfThen;
    size_t i = 1; // Skip IF
    
    while (i < _tokens.size() && _tokens[i].value != "THEN" 
           && _tokens[i].type != TokenType::EndOfLine) {
        stmt.conditionTokens.push_back(_tokens[i++]);
    }
    
    if (i >= _tokens.size() || _tokens[i].value != "THEN") {
        throw std::runtime_error("IF conditional not followed by THEN");
    }
    i++; // Skip THEN
    
    while (i < _tokens.size() && _tokens[i].type != TokenType::EndOfLine) {
        stmt.thenTokens.push_back(_tokens[i++]);
    }
    return stmt;
}

ParsedStatement Parser::parseGoto() {
    ParsedStatement stmt;
    stmt.type = StatementType::Goto;
    size_t i = 1; // Skip GOTO
    
    if (i >= _tokens.size()) {
        throw std::runtime_error("GOTO missing target line");
    }
    
    while (i < _tokens.size() && _tokens[i].type != TokenType::EndOfLine) {
        stmt.expressionTokens.push_back(_tokens[i++]);
    }
    return stmt;
}