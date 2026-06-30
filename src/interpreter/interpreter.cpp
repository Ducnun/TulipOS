#include "interpreter/interpreter.h"

void Interpreter::visit(const LiteralExpr& expr){
    switch (expr.literal.type){
        case TokenType::Float: _lastValue = Value(std::stod(expr.literal.value)); break;
        case TokenType::String: _lastValue = Value(expr.literal.value); break;
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

    switch (expr.op.type) {
        case TokenType::Minus: _lastValue = Value(-val.asNumber()); break;
        case TokenType::Not: _lastValue = Value(!val.isTruthy()); break;
    }
}

void Interpreter::visit(const GroupingExpr& expr){
    _lastValue = evaluate(expr.expression);
}

void Interpreter::visit(const PrintStmt& stmt){
    for (const auto& item : stmt.items) {
        Value val = evaluate(item.expression);
        _io.print(val.asString());
        if (item.separator == PrintSeparator::Comma) _io.print("\t");
    }
    _io.print("\n");
}

void Interpreter::visit(const LetStmt& stmt){
    _ctx.set(stmt.variable, evaluate(stmt.value));
}

void Interpreter::visit(const InputStmt& stmt){
    if (stmt.hasPrompt) _io.print(stmt.prompt + "\n");
    // Simple implementation: read one line, assign to first variable
    std::string input = _io.readLine();
    if (!stmt.variables.empty()) _ctx.set(stmt.variables[0], Value(input));
}

void Interpreter::visit(const GotoStmt& stmt){

}

void Interpreter::visit(const IfStmt& stmt){
    Value condition = evaluate(stmt.condition);
    if (condition.asNumber() != 0.0) stmt.thenBranch->accept(*this);
    else if (stmt.elseBranch) stmt.elseBranch->accept(*this);
}

void Interpreter::visit(const ForStmt& stmt){
    double startVal = evaluate(stmt.start).asNumber();
    double endVal = evaluate(stmt.end).asNumber();
    double step = stmt.step ? evaluate(stmt.step).asNumber() : 1.0;
    _ctx.set(stmt.variable, Value(startVal));

    while (true){
        double currentVal = _ctx.get(stmt.variable).asNumber();
        if (step >= 0 && currentVal > endVal) break;
        if (step < 0 && currentVal < endVal) break;

        if (stmt.body) stmt.body->accept(*this);

        if (_ctx.halted) break;

        currentVal += step;
        _ctx.set(stmt.variable, currentVal);
    }
}

void Interpreter::visit(const BlockStmt& stmt){
    for (const auto& s : stmt.statements) s->accept(*this);
}

void Interpreter::visit(const CallStmt& stmt){
    std::vector<Value> argList;
    for (const auto& arg : stmt.args){
        argList.push_back(evaluate(arg));
    }

    if (stmt.module == "GPIO"){
        _lastValue = gpio::call(stmt, argList);
    }
    else{
        _lastValue = Value(false);
    }
}

void Interpreter::visit(const AssignmentStmt& stmt){
    _ctx.set(stmt.variable, evaluate(stmt.value));
}

void Interpreter::visit(const EndStmt& stmt){
    _ctx.halted = true;
}

void Interpreter::visit(const ReturnStmt& stmt){
    if (stmt.value) _ctx.returnValue = evaluate(stmt.value);
    throw ReturnSignal();
}
