#pragma once

#include <string>
#include <memory>
#include <vector>
#include "tokenizer/token.h"
#include "interpreter/value.h"
#include "engine/symboltable.h"

// Forward declaration
class ExprVisitor;

struct Expr{
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) const = 0;
};

using ExprPtr = std::unique_ptr<Expr>;

struct LiteralExpr : Expr{
    Value value;

    LiteralExpr(Value value) :
        value(std::move(value)) {}
    
    void accept(ExprVisitor&) const override;
};

struct VariableExpr : Expr{
    Token name;

    VariableExpr(Token name) :
        name(std::move(name)) {}
    
    void accept(ExprVisitor&) const override;
};

struct UnaryExpr : Expr{
    TokenType op;
    ExprPtr val;

    UnaryExpr(TokenType op, ExprPtr val) :
        op(op), val(std::move(val)) {}
        
    void accept(ExprVisitor&) const override;
};

struct BinaryExpr : Expr{
    ExprPtr left;
    TokenType op;
    ExprPtr right;

    BinaryExpr(ExprPtr left, TokenType op, ExprPtr right) :
        left(std::move(left)), op(op), right(std::move(right)) {}
    
    void accept(ExprVisitor&) const override;
};

struct GroupingExpr : Expr{
    ExprPtr expression;
    
    GroupingExpr(ExprPtr expression) : 
        expression(std::move(expression)) {}

    void accept(ExprVisitor&) const override;
};

struct CallExpr : Expr{
    std::string module;
    std::string function;
    std::vector<ExprPtr> args;

    CallExpr(std::string module, std::string function, std::vector<ExprPtr> args) :
        module(std::move(module)), function(std::move(function)), args(std::move(args)) {}
    
    void accept(ExprVisitor&) const override;
};