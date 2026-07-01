#include "interpreter/interpreter.h"

void Interpreter::visit(const LiteralExpr& expr){
    _lastValue = expr.value;
}

void Interpreter::visit(const VariableExpr& expr){
    _lastValue = _ctx.get(expr.name.value);
}

void Interpreter::visit(const BinaryExpr& expr){
    Value left = evaluate(expr.left);
    Value right = evaluate(expr.right);

    switch (expr.op){
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

    switch (expr.op) {
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
    _ctx.jumpTarget = (int)evaluate(stmt.target).asNumber();
    _ctx.jumpRequested = true;
}

void Interpreter::visit(const IfStmt& stmt){
    Value condition = evaluate(stmt.condition);
    if (condition.asNumber() != 0.0) stmt.thenBranch->accept(*this);
    else if (stmt.elseBranch) stmt.elseBranch->accept(*this);
}

void Interpreter::visit(const ForStmt& stmt) {
    // Check if we are already in this specific loop
    bool isReEntry = !_ctx.forLoopStack.empty() && 
                      _ctx.forLoopStack.back().variable == stmt.variable;

    if (!isReEntry) {
        // Normal initialization
        float startVal = evaluate(stmt.start).asNumber();
        float endVal = evaluate(stmt.end).asNumber();
        float stepVal = stmt.step ? evaluate(stmt.step).asNumber() : 1.0f;
        
        _ctx.set(stmt.variable, Value(startVal));
        _ctx.forLoopStack.push_back({stmt.variable, endVal, stepVal, _ctx.currentLine});
    }
}

void Interpreter::visit(const NextStmt& stmt){
    if (_ctx.forLoopStack.empty()) throw std::runtime_error("NEXT without FOR");

    ForLoopFrame& frame = _ctx.forLoopStack.back();
    if (!stmt.variable.empty() && stmt.variable != frame.variable){ 
        throw std::runtime_error("NEXT var doesnt match: " + stmt.variable);
    }

    float current = _ctx.get(frame.variable).asNumber();
    current += frame.stepValue;
    _ctx.set(frame.variable, Value(current));

    bool loopAgain = (frame.stepValue >= 0) ? (current <= frame.endValue) : (current >= frame.endValue);
    
    if (loopAgain){
        _ctx.jumpTarget = frame.bodyStartLine;
        _ctx.jumpRequested = true;
    } else{
        _ctx.forLoopStack.pop_back();
    }
}

void Interpreter::visit(const BlockStmt& stmt){
    for (const auto& s : stmt.statements){
        s->accept(*this);
        if (_ctx.jumpRequested || _ctx.halted) return;
    }
}

Value Interpreter::handleCall(const std::string& mod, const std::string& func, const std::vector<Value>& args) {
    if (mod == "MATH") {
        return math::call(func, args);
    }
    
    if (mod == "GPIO") {
        return gpio::call(func, args);
    }

    return Value(0.0f); // Default return
}

// Then your visitors are simple:
void Interpreter::visit(const CallStmt& stmt) {
    std::vector<Value> argValues;
    for (const auto& a : stmt.args) argValues.push_back(evaluate(a));
    _lastValue = handleCall(stmt.module, stmt.function, argValues);
}

void Interpreter::visit(const CallExpr& expr) {
    std::vector<Value> argValues;
    for (const auto& a : expr.args) argValues.push_back(evaluate(a));
    _lastValue = handleCall(expr.module, expr.function, argValues);
}

void Interpreter::visit(const AssignmentStmt& stmt){
    _ctx.set(stmt.variable, evaluate(stmt.value));
}

void Interpreter::visit(const EndStmt& stmt){
    throw EndSignal();
}

void Interpreter::visit(const ReturnStmt& stmt){
    if (stmt.value) _ctx.returnValue = evaluate(stmt.value);
    throw ReturnSignal();
}
