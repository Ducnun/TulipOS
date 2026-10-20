#pragma once

#include "ast/expression_visitor.h"
#include "ast/statement_visitor.h"
#include "runtime/core/runtime_context.h"
#include "runtime/modules/gpio_module.h"
#include "runtime/modules/math_module.h"
#include "runtime/modules/system_module.h"
#include "signals.h"
#include <cstring>

class Interpreter : public StmtVisitor, public ExprVisitor {    
public:
    ~Interpreter() = default;
    Interpreter(RuntimeContext& ctx, SymbolTable& symTable, Arena& staticArena) : 
        _ctx(ctx), 
        _symTable(symTable), 
        _staticArena(staticArena),
        _gpio(_ctx),
        _sys(_ctx)
        {}

    void execute(const std::vector<StmtPtr>& statements) {
        for (const auto& stmt : statements) {
            if (stmt) stmt->accept(*this);
        }
    }
    
private:
    RuntimeContext& _ctx;
    SymbolTable& _symTable;
    Arena& _staticArena;
    Value _lastValue;
    GpioModule _gpio;
    MathModule _math;
    SystemModule _sys;

    Value evaluate(const ExprPtr& expr) {
        expr->accept(*this);
        return _lastValue;
    }

    void visit(const LiteralExpr& expr) override;
    void visit(const VariableExpr& expr) override;
    void visit(const BinaryExpr& expr) override;
    void visit(const UnaryExpr& expr) override;
    void visit(const CallExpr& expr) override;
    void visit(const GroupingExpr& expr) override;

    // --- Statement Execution ---
    void visit(const PrintStmt& stmt) override;
    void visit(const LetStmt& stmt) override ;
    void visit(const InputStmt& stmt) override;
    void visit(const GotoStmt& stmt) override;
    void visit(const IfStmt& stmt) override;
    void visit(const ForStmt& stmt) override;
    void visit(const NextStmt& stmt) override;
    void visit(const CallStmt& stmt) override;
    void visit(const AssignmentStmt& stmt) override;
    void visit(const BlockStmt& stmt) override;
    void visit(const EndStmt& stmt) override;
    void visit(const ReturnStmt& stmt) override;

    bool checkEqual(const Value& a, const Value& b) {
        if (isAnyString(a) && isAnyString(b)) {
            return std::strcmp(getString(a), getString(b)) == 0;
        }
        if (a.type != b.type) return false;

        if (a.type == ValueType::Number) return a.number == b.number;
        if (a.type == ValueType::Boolean) return a.boolean == b.boolean;
        return false;
    }

    bool checkGreater(const Value& a, const Value& b) {
        if (isAnyString(a) && isAnyString(b)) {
            return std::strcmp(getString(a), getString(b)) > 0;
        }
        if (a.type == ValueType::Number && b.type == ValueType::Number){
            return a.number > b.number;
        }
        if (a.type == ValueType::Boolean && b.type == ValueType::Boolean){
            return a.boolean > b.boolean;
        }
        return false;
    }

    Value handleCall(Symbol mod, Symbol func, util::span<const Value> args);

    const char* getString(const Value& val) const {
        if (val.type == ValueType::PermanentString) return _symTable.getString(val.stringId);
        else if (val.type == ValueType::PersistentString || val.type == ValueType::TransientString){
            return val.rawPtr ? val.rawPtr : "";
        }
        return "";
    }

    bool isAnyString(const Value& val) const {
        return  val.type == ValueType::PermanentString  ||
                val.type == ValueType::PersistentString ||
                val.type == ValueType::TransientString;
    }
};