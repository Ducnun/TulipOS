#pragma once

#include <string>
#include <memory>
#include <vector>
#include "tokenizer/token.h"
#include "runtime/core/value.h"

// Forward declaration
class ExprVisitor;

struct Expr{
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) const = 0;
};

using ExprPtr = Expr*;

struct LiteralExpr : Expr{
    Value value;

    LiteralExpr(Value value) :
        value(value) {}
    
    void accept(ExprVisitor&) const override;
};

struct VariableExpr : Expr{
    Symbol nameId;

    VariableExpr(Symbol nameId) :
        nameId(nameId) {}
    
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
    Symbol module;
    Symbol function;
    std::vector<ExprPtr> args;

    CallExpr(Symbol module, Symbol function, std::vector<ExprPtr> args) :
        module(module), function(function), args(std::move(args)) {}
    
    void accept(ExprVisitor&) const override;
};