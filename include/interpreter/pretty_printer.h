#pragma once

#include "ast/expression_visitor.h"
#include "ast/statement_visitor.h"

class PrettyPrinter : public ExprVisitor, public StmtVisitor{
public:
    std::string result;

    void print(const std::vector<StmtPtr>& statements) {
        for (const auto& stmt : statements) {
            stmt->accept(*this);
        }
    }

    // ExprVisitor
    void visit(const LiteralExpr& expr) override;
    void visit(const VariableExpr& expr) override;
    void visit(const UnaryExpr& expr) override;
    void visit(const BinaryExpr& expr) override;
    void visit(const GroupingExpr& expr) override;

    // StmtVisitor
    void visit(const PrintStmt& stmt) override;
    void visit(const LetStmt& stmt) override;
    void visit(const AssignmentStmt& stmt) override;
    void visit(const InputStmt& stmt) override;
    void visit(const GotoStmt& stmt) override;
    void visit(const IfStmt& stmt) override;
    void visit(const ForStmt& stmt) override;
    void visit(const EndStmt& stmt) override;
    void visit(const ReturnStmt& stmt) override;
    void visit(const CallStmt& stmt) override;
    void visit(const BlockStmt& stmt) override;

private:
    int indent = 0;

    std::string pad(){
        return std::string(indent * 2, ' ');
    }
};