#pragma once

#include "ast/expression_visitor.h"
#include "ast/statement_visitor.h"
#include "runtime_context.h"
#include "io_interface.h"

class Interpreter : public StmtVisitor, public ExprVisitor {    
public:
    ~Interpreter() = default;
    Interpreter(RuntimeContext& ctx, IOInterface& io) : _ctx(ctx), _io(io) {}

    void execute(const std::vector<StmtPtr>& statements) {
        for (const auto& stmt : statements) {
            if (stmt) stmt->accept(*this);
        }
    }
    
private:
    RuntimeContext& _ctx;
    IOInterface& _io;
    Value _lastValue;

    Value evaluate(const ExprPtr& expr) {
        expr->accept(*this);
        return _lastValue;
    }

    void visit(const LiteralExpr& expr) override;
    void visit(const VariableExpr& expr) override;
    void visit(const BinaryExpr& expr) override;
    void visit(const UnaryExpr& expr) override;
    void visit(const GroupingExpr& expr) override;

    // --- Statement Execution ---
    void visit(const PrintStmt& stmt) override;
    void visit(const LetStmt& stmt) override ;
    void visit(const InputStmt& stmt) override;
    void visit(const GotoStmt& stmt) override;
    void visit(const IfStmt& stmt) override;
    void visit(const ForStmt& stmt) override;
    void visit(const BlockStmt& stmt) override;
    void visit(const CallStmt& stmt) override;
    void visit(const AssignmentStmt& stmt) override;
    void visit(const EndStmt& stmt) override;
    void visit(const ReturnStmt& stmt) override;

    bool checkEqual(const Value& a, const Value& b) {
        if (a.type != b.type) return false;

        if (a.isNumber()) return a.numberValue == b.numberValue;
        if (a.isString()) return a.stringValue == b.stringValue;
        if (a.type == ValueType::Boolean) return a.boolValue == b.boolValue;
        return false;
    }

    bool checkGreater(const Value& a, const Value& b) {
        // String comparison (lexicographical)
        if (a.isString() && b.isString()) {
            return a.stringValue > b.stringValue;
        }
        // Numeric comparison
        return a.asNumber() > b.asNumber();
    }
};