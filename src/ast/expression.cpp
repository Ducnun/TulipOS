#include "ast/expression.h"
#include "ast/expression_visitor.h"

void LiteralExpr::accept(ExprVisitor& v) const {
    v.visit(*this);
}

void VariableExpr::accept(ExprVisitor& v) const {
    v.visit(*this);
}

void UnaryExpr::accept(ExprVisitor& v) const {
    v.visit(*this);
}

void BinaryExpr::accept(ExprVisitor& v) const {
    v.visit(*this);
}

void GroupingExpr::accept(ExprVisitor& v) const {
    v.visit(*this);
}