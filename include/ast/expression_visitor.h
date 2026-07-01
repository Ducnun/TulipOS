#pragma once

#include "expression.h"

class ExprVisitor{
public:
    virtual ~ExprVisitor() = default;

    virtual void visit(const LiteralExpr&) = 0;
    virtual void visit(const VariableExpr&) = 0;
    virtual void visit(const UnaryExpr&) = 0;
    virtual void visit(const BinaryExpr&) = 0;
    virtual void visit(const GroupingExpr&) = 0;
    virtual void visit(const CallExpr&) = 0;
};