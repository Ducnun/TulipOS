// #include "interpreter/pretty_printer.h"

// // --- Expressions ---

// void PrettyPrinter::visit(const LiteralExpr& expr) {
//     result += pad() + "Literal(" + expr.value.asString() + ")\n";
// }

// void PrettyPrinter::visit(const VariableExpr& expr) {
//     result += pad() + "Variable(" + expr.name.value + ")\n";
// }

// void PrettyPrinter::visit(const BinaryExpr& expr) {
//     result += pad() + "Binary(" + TokenUtils::typeToString(expr.op) + ")\n";
//     indent++;
//     expr.left->accept(*this);
//     expr.right->accept(*this);
//     indent--;
// }

// void PrettyPrinter::visit(const UnaryExpr& expr) {
//     result += pad() + "Unary(" + TokenUtils::typeToString(expr.op) + ")\n";
//     indent++;
//     expr.val->accept(*this);
//     indent--;
// }

// void PrettyPrinter::visit(const GroupingExpr& expr) {
//     result += pad() + "Grouping\n";
//     indent++;
//     expr.expression->accept(*this);
//     indent--;
// }

// void PrettyPrinter::visit(const CallExpr& expr){
//     result += pad() + "CallStmt(" + expr.module + "::" + expr.function + ")\n";
//     indent++;
//     for (const auto& arg : expr.args) {
//         arg->accept(*this);
//     }
//     indent--;
// }

// // --- Statements ---

// void PrettyPrinter::visit(const PrintStmt& stmt) {
//     result += pad() + "PrintStmt\n";
//     indent++;
//     for (const auto& item : stmt.items) {
//         item.expression->accept(*this);
//     }
//     indent--;
// }

// void PrettyPrinter::visit(const LetStmt& stmt) {
//     result += pad() + "LetStmt(" + stmt.variable + ")\n";
//     indent++;
//     stmt.value->accept(*this);
//     indent--;
// }

// void PrettyPrinter::visit(const AssignmentStmt& stmt) {
//     result += pad() + "AssignmentStmt(" + stmt.variable + ")\n";
//     indent++;
//     stmt.value->accept(*this);
//     indent--;
// }

// void PrettyPrinter::visit(const InputStmt& stmt) {
//     result += pad() + "InputStmt";
//     if (stmt.hasPrompt) result += " (Prompt: \"" + stmt.prompt + "\")";
//     result += "\n";

//     indent++;
//     for (const auto& var : stmt.variables) {
//         result += pad() + "Target: " + var + "\n";
//     }
//     indent--;
// }

// void PrettyPrinter::visit(const GotoStmt& stmt) {
//     result += pad() + "GotoStmt\n";
//     indent++;
//     stmt.target->accept(*this);
//     indent--;
// }

// void PrettyPrinter::visit(const IfStmt& stmt) {
//     result += pad() + "IfStmt\n";
//     indent++;
    
//     result += pad() + "Condition:\n";
//     indent++;
//     stmt.condition->accept(*this);
//     indent--;

//     result += pad() + "Then:\n";
//     indent++;
//     stmt.thenBranch->accept(*this);
//     indent--;

//     if (stmt.elseBranch) {
//         result += pad() + "Else:\n";
//         indent++;
//         stmt.elseBranch->accept(*this);
//         indent--;
//     }
//     indent--;
// }

// void PrettyPrinter::visit(const ForStmt& stmt) {
//     // result += pad() + "ForStmt(" + stmt.variable + ")\n";
//     // indent++;

//     // result += pad() + "Start:\n";
//     // indent++;
//     // stmt.start->accept(*this);
//     // indent--;

//     // result += pad() + "End:\n";
//     // indent++;
//     // stmt.end->accept(*this);
//     // indent--;

//     // if (stmt.step) {
//     //     result += pad() + "Step:\n";
//     //     indent++;
//     //     stmt.step->accept(*this);
//     //     indent--;
//     // }

//     // if (stmt.body) {
//     //     result += pad() + "Body:\n";
//     //     indent++;
//     //     stmt.body->accept(*this);
//     //     indent--;
//     // }
//     // indent--;
// }

// void PrettyPrinter::visit(const NextStmt& stmt){
    
// }

// void PrettyPrinter::visit(const EndStmt& stmt) {
//     result += pad() + "EndStmt\n";
// }

// void PrettyPrinter::visit(const ReturnStmt& stmt) {
//     result += pad() + "ReturnStmt\n";
//     if (stmt.value) {
//         indent++;
//         stmt.value->accept(*this);
//         indent--;
//     }
// }

// void PrettyPrinter::visit(const CallStmt& stmt) {
//     result += pad() + "CallStmt(" + stmt.module + "::" + stmt.function + ")\n";
//     indent++;
//     for (const auto& arg : stmt.args) {
//         arg->accept(*this);
//     }
//     indent--;
// }

// void PrettyPrinter::visit(const BlockStmt& stmt) {
//     result += pad() + "BlockStmt\n";
//     indent++;
//     for (const auto& s : stmt.statements) {
//         if (s) s->accept(*this);
//     }
//     indent--;
// }