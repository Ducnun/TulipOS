#pragma once

#include "ast/expression_visitor.h"
#include "ast/statement_visitor.h"
#include "io/io_interface.h"
#include "runtime/core/runtime_context.h"
#include "runtime/modules/gpio_module.h"
#include "runtime/modules/math_module.h"
#include "signals.h"

class Interpreter : public StmtVisitor, public ExprVisitor {    
public:
    ~Interpreter() = default;
    Interpreter(RuntimeContext& ctx, IOInterface& io, StringRegistry& registry, Arena& staticArena) 
        : _ctx(ctx), _io(io), _registry(registry), _staticArena(staticArena) {
        }

    void execute(const std::vector<StmtPtr>& statements) {
        for (const auto& stmt : statements) {
            if (stmt) stmt->accept(*this);
        }
    }
    
private:
RuntimeContext& _ctx;
    IOInterface& _io;
    StringRegistry& _registry;
    Arena& _staticArena;
    Value _lastValue;

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
        if (a.type != b.type) return false;

        if (a.type == ValueType::Number) return a.number == b.number;
        if (a.type == ValueType::Boolean) return a.boolean == b.boolean;
        if (a.type == ValueType::PermanentString) return a.stringId == b.stringId;
        return false;
    }

    bool checkGreater(const Value& a, const Value& b) {
        // String comparison (lexicographical)
        if (a.type == ValueType::PermanentString && b.type == ValueType::PermanentString) {
            return _registry.getString(a.stringId) > _registry.getString(b.stringId);
        }
        // Numeric comparison
        if (a.type == ValueType::Number && b.type == ValueType::Number){
            return a.number > b.number;
        }
        if (a.type == ValueType::Boolean && b.type == ValueType::Boolean){
            return a.boolean > b.boolean;
        }
    }

    Value handleCall(Symbol mod, Symbol func, const std::vector<Value>& args);

    const char* getString(const Value& val) const {
        if (val.type == ValueType::PermanentString) return _registry.getString(val.stringId);
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