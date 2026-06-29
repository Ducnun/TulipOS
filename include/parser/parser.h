#pragma once

#include <stdexcept>

#include "ast/expression.h"
#include "ast/statement.h"
#include "utils/ptrs.h"

class Parser{
public:
    ~Parser() = default;

    std::vector<StmtPtr> parse(std::vector<Token> tokens);

private:
    std::vector<Token> _tokens;
    size_t _current = 0;

    // Helper functions
    Token& peek() {
        return _tokens[_current];
    }

    bool isAtEnd() {
        return peek().type == TokenType::Eof;
    }

    Token& previous(){
        return _tokens[_current - 1];
    }

    Token advance(){
        if (!isAtEnd()) _current++;
        return previous();
    }

    bool check(TokenType expected){
        if (isAtEnd()) return false;
        return peek().type == expected;
    }

    bool checkNext(TokenType type) {
        if (isAtEnd()) return false;
        if (_tokens[_current + 1].type == TokenType::Eof) return false;
        return _tokens[_current + 1].type == type;
    }

    bool match(std::initializer_list<TokenType> expected){
        for (const auto& type : expected){
            if (check(type)){
                advance();
                return true;
            }
        }
        return false;
    }

    Token consume(TokenType type, const std::string& msg){
        if (check(type)) return advance();
        throw std::runtime_error(msg);
    }

    // statements
    StmtPtr statement();
    StmtPtr printStatement();
    StmtPtr letStatement();
    StmtPtr assignmentStatement();
    StmtPtr ifStatement();
    StmtPtr forStatement();
    StmtPtr nextStatement();
    StmtPtr gotoStatement();
    StmtPtr inputStatement();
    StmtPtr returnStatement();
    StmtPtr callStatement();
    StmtPtr blockStatement();

    // expressions
    ExprPtr expression();
    ExprPtr orExpr();
    ExprPtr andExpr();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr primary();
};