#include "interpreter/interpreter.h"

void Interpreter::visit(const LiteralExpr& expr){
    switch (expr.literal.type){
        case TokenType::Float: _lastValue = Value(expr.literal.value); break;
        case TokenType::String: _lastValue = Value(std::stod(expr.literal.value)); break;
        case TokenType::True: _lastValue = Value(true); break;
        case TokenType::False: _lastValue = Value(false); break;
    }
}

void Interpreter::visit(const VariableExpr& expr){
    _lastValue = _ctx.get(expr.name.value);
}

void Interpreter::visit(const BinaryExpr& expr){
    Value left = evaluate(expr.left);
    Value right = evaluate(expr.right);

    switch (expr.op.type){
        // --- Basic Operators ---
        case TokenType::Plus:
            if (left.isString() || right.isString()) _lastValue = Value(left.asString() + right.asString());
            else _lastValue = Value(left.asNumber() + right.asNumber());
            break;
        case TokenType::Minus: _lastValue = Value(left.asNumber() - right.asNumber()); break;
        case TokenType::Star:  _lastValue = Value(left.asNumber() * right.asNumber()); break;
        case TokenType::Slash: _lastValue = Value(left.asNumber() / right.asNumber()); break;

        // --- Comparison Operators ---
        case TokenType::Greater: _lastValue = Value(checkGreater(left, right)); break;
        case TokenType::GreaterEqual: _lastValue = Value(checkGreater(left, right) || checkEqual(left, right)); break;
        case TokenType::Less: _lastValue = Value(!checkGreater(left, right) && !checkEqual(left, right)); break;
        case TokenType::LessEqual: _lastValue = Value(!checkGreater(left, right)); break;
        case TokenType::Equal: _lastValue = Value(checkEqual(left, right)); break;
        case TokenType::NotEqual: _lastValue = Value(!checkEqual(left, right)); break;

        // --- Logic Operators ---
        case TokenType::And: _lastValue = Value(left.isTruthy() && right.isTruthy()); break;
        case TokenType::Or: _lastValue = Value(left.isTruthy() || right.isTruthy()); break;
    }
}

void Interpreter::visit(const UnaryExpr& expr){
    Value val = evaluate(expr.val);
}

void Interpreter::visit(const GroupingExpr& expr){

}

void Interpreter::visit(const PrintStmt& stmt){

}

void Interpreter::visit(const LetStmt& stmt){

}

void Interpreter::visit(const InputStmt& stmt){

}

void Interpreter::visit(const GotoStmt& stmt){

}

void Interpreter::visit(const IfStmt& stmt){

}

void Interpreter::visit(const ForStmt& stmt){

}

void Interpreter::visit(const BlockStmt& stmt){

}

void Interpreter::visit(const CallStmt& stmt){

}

void Interpreter::visit(const AssignmentStmt& stmt){

}
