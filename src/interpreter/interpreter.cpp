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
    Value left, right;
    switch (expr.op.type){
        case TokenType::Plus:
        case TokenType::Minus:
        case TokenType::Star:
        case TokenType::Slash:
        case TokenType::Equal:
        case TokenType::NotEqual: 

    }
}

void Interpreter::visit(const UnaryExpr& expr){

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
