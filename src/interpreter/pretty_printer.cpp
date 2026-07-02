#include "interpreter/pretty_printer.h"

// Helper to get string from registry
std::string PrettyPrinter::sym(Symbol id) const {
    if (id == INVALID_SYMBOL) return "INVALID";
    return _registry.getString(id);
}

// Helper to convert Value to string for display
std::string PrettyPrinter::valStr(const Value& v) const {
    switch (v.type) {
        case ValueType::Number:  return std::to_string(v.number);
        case ValueType::Boolean: return v.boolean ? "true" : "false";
        case ValueType::String:  return "\"" + sym(v.stringId) + "\"";
        default: return "unknown";
    }
}

// --- Expressions ---

void PrettyPrinter::visit(const LiteralExpr& expr) {
    result += pad() + "Literal(" + valStr(expr.value) + ")\n";
}

void PrettyPrinter::visit(const VariableExpr& expr) {
    result += pad() + "Variable(" + sym(expr.nameId) + ", ID: " + std::to_string(expr.nameId) +  ")\n";
}

void PrettyPrinter::visit(const BinaryExpr& expr) {
    result += pad() + "Binary(" + TokenUtils::typeToString(expr.op) + ")\n";
    indent++;
    expr.left->accept(*this);
    expr.right->accept(*this);
    indent--;
}

void PrettyPrinter::visit(const UnaryExpr& expr) {
    result += pad() + "Unary(" + TokenUtils::typeToString(expr.op) + ")\n";
    indent++;
    expr.val->accept(*this);
    indent--;
}

void PrettyPrinter::visit(const GroupingExpr& expr) {
    result += pad() + "Grouping\n";
    indent++;
    expr.expression->accept(*this);
    indent--;
}

void PrettyPrinter::visit(const CallExpr& expr){
    result += pad() + "CallExpr(" + sym(expr.module) + "::" + sym(expr.function) + ")\n";
    indent++;
    for (const auto& arg : expr.args) {
        arg->accept(*this);
    }
    indent--;
}

// --- Statements ---

void PrettyPrinter::visit(const PrintStmt& stmt) {
    result += pad() + "PrintStmt\n";
    indent++;
    for (const auto& item : stmt.items) {
        item.expression->accept(*this);
    }
    indent--;
}

void PrettyPrinter::visit(const LetStmt& stmt) {
    result += pad() + "LetStmt(" + sym(stmt.variable) + ")\n";
    indent++;
    stmt.value->accept(*this);
    indent--;
}

void PrettyPrinter::visit(const AssignmentStmt& stmt) {
    result += pad() + "AssignmentStmt(" + sym(stmt.variable) + ")\n";
    indent++;
    stmt.value->accept(*this);
    indent--;
}

void PrettyPrinter::visit(const InputStmt& stmt) {
    result += pad() + "InputStmt";
    if (stmt.hasPrompt) result += " (Prompt: \"" + sym(stmt.promptId) + "\")";
    result += "\n";

    indent++;
    for (const auto& var : stmt.variables) {
        result += pad() + "Target: " + sym(var) + "\n";
    }
    indent--;
}

void PrettyPrinter::visit(const GotoStmt& stmt) {
    result += pad() + "GotoStmt\n";
    indent++;
    stmt.target->accept(*this);
    indent--;
}

void PrettyPrinter::visit(const IfStmt& stmt) {
    result += pad() + "IfStmt\n";
    indent++;
    
    result += pad() + "Condition:\n";
    indent++;
    stmt.condition->accept(*this);
    indent--;

    result += pad() + "Then:\n";
    indent++;
    stmt.thenBranch->accept(*this);
    indent--;

    if (stmt.elseBranch) {
        result += pad() + "Else:\n";
        indent++;
        stmt.elseBranch->accept(*this);
        indent--;
    }
    indent--;
}

void PrettyPrinter::visit(const ForStmt& stmt) {
    result += pad() + "ForStmt(" + sym(stmt.variable) + ")\n";
    indent++;

    result += pad() + "Start:\n";
    indent++;
    stmt.start->accept(*this);
    indent--;

    result += pad() + "End:\n";
    indent++;
    stmt.end->accept(*this);
    indent--;

    if (stmt.step) {
        result += pad() + "Step:\n";
        indent++;
        stmt.step->accept(*this);
        indent--;
    }
    indent--;
}

void PrettyPrinter::visit(const NextStmt& stmt){
    result += pad() + "NextStmt(" + sym(stmt.variable) + ")\n";
}

void PrettyPrinter::visit(const EndStmt& stmt) {
    result += pad() + "EndStmt\n";
}

void PrettyPrinter::visit(const ReturnStmt& stmt) {
    result += pad() + "ReturnStmt\n";
    if (stmt.value) {
        indent++;
        stmt.value->accept(*this);
        indent--;
    }
}

void PrettyPrinter::visit(const CallStmt& stmt) {
    result += pad() + "CallStmt(" + sym(stmt.module) + "::" + sym(stmt.function) + ")\n";
    indent++;
    for (const auto& arg : stmt.args) {
        arg->accept(*this);
    }
    indent--;
}

void PrettyPrinter::visit(const BlockStmt& stmt) {
    result += pad() + "BlockStmt\n";
    indent++;
    for (const auto& s : stmt.statements) {
        if (s) s->accept(*this);
    }
    indent--;
}