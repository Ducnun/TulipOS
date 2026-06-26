#include "parser.h"
#include <iostream>

ParsedStatement Parser::parse(){
    ParsedStatement stmt;
    if(_tokens.empty()) return stmt;

    switch (_tokens[0].type){
        case TokenType::Print: return parsePrint();
        case TokenType::Let: return parseLet();
        case TokenType::If: return parseIfThen();
        case TokenType::Goto: return parseGoto();
        case TokenType::Input: return parseInput();
        case TokenType::For: return parseFor();
        case TokenType::Next: return parseNext();
        default:
            throw std::runtime_error("Unknown statement type");
    }
}

ParsedStatement Parser::parsePrint(){
    ParsedStatement stmt;
    stmt.type = StatementType::Print;
    size_t i = 1; // Skip PRINT
    
    while (i < _tokens.size() && _tokens[i].type != TokenType::EndOfLine){
        stmt.printTokens.push_back(_tokens[i++]);
    }
    return stmt;
}

ParsedStatement Parser::parseLet() {
    ParsedStatement stmt;
    stmt.type = StatementType::Assignment;

    size_t i = 1; // Skip LET

    if (i >= _tokens.size() || _tokens[i].type != TokenType::Identifier)
        throw std::runtime_error("Expected identifier after LET");

    stmt.identifier = _tokens[i++].value;

    if (i >= _tokens.size() || _tokens[i].type != TokenType::Equal)
        throw std::runtime_error("Expected '=' after identifier");

    ++i; // consume '='

    while (i < _tokens.size()) {
        stmt.expressionTokens.push_back(_tokens[i++]);
    }

    return stmt;
}

ParsedStatement Parser::parseIfThen() {
    ParsedStatement stmt;
    stmt.type = StatementType::IfThen;
    size_t i = 1; // Skip IF
    
    while (i < _tokens.size() && _tokens[i].type != TokenType::Then 
           && _tokens[i].type != TokenType::EndOfLine){
        stmt.conditionTokens.push_back(_tokens[i++]);
    }
    
    if (i >= _tokens.size() || _tokens[i].type != TokenType::Then){
        throw std::runtime_error("IF conditional not followed by THEN");
    }
    i++; // Skip THEN
    
    while (i < _tokens.size() && _tokens[i].type != TokenType::EndOfLine){
        stmt.thenTokens.push_back(_tokens[i++]);
    }
    return stmt;
}

ParsedStatement Parser::parseGoto(){
    ParsedStatement stmt;
    stmt.type = StatementType::Goto;
    size_t i = 1; // Skip GOTO
    
    if (i >= _tokens.size()){
        throw std::runtime_error("GOTO missing target line");
    }
    
    while (i < _tokens.size() && _tokens[i].type != TokenType::EndOfLine){
        stmt.expressionTokens.push_back(_tokens[i++]);
    }
    return stmt;
}

ParsedStatement Parser::parseInput(){
    ParsedStatement stmt;
    stmt.type = StatementType::Input;
    size_t i = 1;   // Skip Input

    if (i < _tokens.size() && _tokens[i].type == TokenType::String){
        stmt.printTokens.push_back(_tokens[i++]);

        if (i < _tokens.size()){
            if (_tokens[i].type == TokenType::Semicolon || _tokens[i].type == TokenType::Comma) i++;
            else throw std::runtime_error("MISSING ; OR , AFTER PROMPT");
        }
    }

    bool firstVar = true;
    while (i < _tokens.size() && _tokens[i].type != TokenType::EndOfLine){
        if (!firstVar){
            if (_tokens[i].type != TokenType::Comma){
                throw std::runtime_error("EXPECTED , BETWEEN VARIABLES");
            }
            i++;
        }
        if (i >= _tokens.size() || _tokens[i].type != TokenType::Identifier){
            throw std::runtime_error("EXPECTED VARIABLE NAME");
        }

        stmt.identifierTokens.push_back(_tokens[i++]);
        firstVar = false;
    }
    return stmt;
}

ParsedStatement Parser::parseFor(){
    ParsedStatement stmt;
    stmt.type = StatementType::For;
    size_t i = 1;   // Skip for token

    if (i >= _tokens.size() || _tokens[i].type != TokenType::Identifier) throw std::runtime_error("Expected Loop variable after FOR");
    stmt.identifier = _tokens[i++].value;

    if (i >= _tokens.size() || _tokens[i].type != TokenType::Equal) throw std::runtime_error("Expected '=' after loop variable");
    i++;

    while (i < _tokens.size() && _tokens[i].type != TokenType::To){
        stmt.forInitTokens.push_back(_tokens[i++]);
    }

    if (i >= _tokens.size() || _tokens[i].type != TokenType::To) throw std::runtime_error("Expected 'TO' in FOR statement");
    i++;

    while (i < _tokens.size() && _tokens[i].type != TokenType::Step){
        stmt.forToTokens.push_back(_tokens[i++]);
    }

    if (i < _tokens.size() && _tokens[i].type == TokenType::Step){
        i++;
        while (i < _tokens.size()){
            stmt.forStepTokens.push_back(_tokens[i++]);
        }
    }
    return stmt;
}

ParsedStatement Parser::parseNext(){
    ParsedStatement stmt;
    stmt.type = StatementType::Next;
    size_t i = 1;

    if (i >= _tokens.size() || _tokens[i].type != TokenType::Identifier) throw std::runtime_error("Expected Loop variable after NEXT");
    stmt.identifier = _tokens[i++].value;

    if (i != _tokens.size() && _tokens[i].type != TokenType::EndOfLine) {
        throw std::runtime_error("Unexpected token after NEXT statement");
    }

    return stmt;
}