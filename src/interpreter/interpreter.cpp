#include "interpreter/interpreter.h"

void Interpreter::visit(const LiteralExpr& expr){
    _lastValue = expr.value;
}

void Interpreter::visit(const VariableExpr& expr){
    auto it = _ctx.variables.find(expr.nameId);
    if (it != _ctx.variables.end()) {
        _lastValue = it->second;
    } else {
        _lastValue = Value::Float(0.0f);
    }
}

void Interpreter::visit(const BinaryExpr& expr){
    Value left = evaluate(expr.left);
    Value right = evaluate(expr.right);

    switch (expr.op){
        case TokenType::Plus:
            if (isAnyString(left) && isAnyString(right)){
                // Fetch copies of strings, regardless of current allocated arena
                const char* s1 = getString(left);
                const char* s2 = getString(right);
                size_t len = left.length + right.length + 1;

                // Write into transient arena
                char* buf = (char*)_ctx.transientArena.alloc(len);
                if (buf){
                    memcpy(buf, s1, left.length);
                    memcpy(buf + left.length, s2, right.length + 1);
                    _lastValue = Value::TransientStr(buf, len);
                }
            } else {
                _lastValue = Value::Float(left.number + right.number);
            }
            break;

        case TokenType::Minus: _lastValue = Value::Float(left.number - right.number); break;
        case TokenType::Star:  _lastValue = Value::Float(left.number * right.number); break;
        case TokenType::Slash: _lastValue = Value::Float(left.number / right.number); break;

        case TokenType::Greater:      _lastValue = Value::Bool(checkGreater(left, right)); break;
        case TokenType::GreaterEqual: _lastValue = Value::Bool(checkGreater(left, right) || checkEqual(left, right)); break;
        case TokenType::Less:         _lastValue = Value::Bool(!checkGreater(left, right) && !checkEqual(left, right)); break;
        case TokenType::LessEqual:    _lastValue = Value::Bool(!checkGreater(left, right)); break;
        case TokenType::Equal:        _lastValue = Value::Bool(checkEqual(left, right)); break;
        case TokenType::NotEqual:     _lastValue = Value::Bool(!checkEqual(left, right)); break;

        case TokenType::And: _lastValue = Value::Bool(left.isTruthy() && right.isTruthy()); break;
        case TokenType::Or:  _lastValue = Value::Bool(left.isTruthy() || right.isTruthy()); break;
    }
}

void Interpreter::visit(const UnaryExpr& expr){
    Value val = evaluate(expr.val);
    switch (expr.op) {
        case TokenType::Minus: _lastValue = Value::Float(-val.number); break;
        case TokenType::Not:   _lastValue = Value::Bool(!val.isTruthy()); break;
    }
}

void Interpreter::visit(const PrintStmt& stmt){
    for (const auto& item : stmt.items) {
        Value val = evaluate(item.expression);
        
        if (isAnyString(val)){
            _io.print(getString(val));
        } else if (val.type == ValueType::Number) {
            _io.print(std::to_string(val.number));
        } else if (val.type == ValueType::Boolean) {
            _io.print(val.boolean ? "true" : "false");
        }

        if (item.separator == PrintSeparator::Comma) _io.print("\t");
    }
    _io.print("\n");
}

void Interpreter::visit(const LetStmt& stmt){
    _ctx.variables[stmt.variable] = _ctx.promoteValue(evaluate(stmt.value));
}

void Interpreter::visit(const AssignmentStmt& stmt){
    _ctx.variables[stmt.variable] = _ctx.promoteValue(evaluate(stmt.value));
}

void Interpreter::visit(const InputStmt& stmt){
    if (stmt.hasPrompt) {
        _io.print(_registry.getString(stmt.promptId));
        _io.print("\n");
    }
    
    std::string input = _io.readLine();
    if (!stmt.variables.empty()) {
        // Store input string in persistent memory directly (mimmicks assign without additional calls)
        char* buf = (char*)_ctx.persistentArena.alloc(input.length() + 1);
        if (buf){
            memcpy(buf, input.c_str(), input.length() + 1);
            _ctx.variables[stmt.variables[0]] = Value::PersistentStr(buf, input.length() + 1);
        }
    }
}

void Interpreter::visit(const GotoStmt& stmt){
    _ctx.jumpTarget = (int)evaluate(stmt.target).number;
    _ctx.jumpRequested = true;
}

void Interpreter::visit(const IfStmt& stmt){
    if (evaluate(stmt.condition).isTruthy()) {
        stmt.thenBranch->accept(*this);
    } else if (stmt.elseBranch) {
        stmt.elseBranch->accept(*this);
    }
}

void Interpreter::visit(const ForStmt& stmt) {
    bool isReEntry = !_ctx.forLoopStack.empty() && 
                      _ctx.forLoopStack.back().variable == stmt.variable;

    if (!isReEntry) {
        float startVal = evaluate(stmt.start).number;
        float endVal = evaluate(stmt.end).number;
        float stepVal = stmt.step ? evaluate(stmt.step).number : 1.0f;
        
        _ctx.variables[stmt.variable] = Value::Float(startVal);
        _ctx.forLoopStack.push_back({stmt.variable, endVal, stepVal, _ctx.currentLine});
    }
}

void Interpreter::visit(const NextStmt& stmt){
    if (_ctx.forLoopStack.empty()) return; // Error handling

    ForLoopFrame& frame = _ctx.forLoopStack.back();
    
    // Pointer comparison (Symbol) is enough here!
    if (stmt.variable != INVALID_SYMBOL && stmt.variable != frame.variable){ 
        return; 
    }

    float current = _ctx.variables[frame.variable].number;
    current += frame.stepValue;
    _ctx.variables[frame.variable] = Value::Float(current);

    bool loopAgain = (frame.stepValue >= 0) ? (current <= frame.endValue) : (current >= frame.endValue);
    
    if (loopAgain){
        _ctx.jumpTarget = frame.bodyStartLine;
        _ctx.jumpRequested = true;
    } else{
        _ctx.forLoopStack.pop_back();
    }
}

Value Interpreter::handleCall(Symbol mod, Symbol func, const std::vector<Value>& args) {
    // Convert IDs back to strings only when calling native modules
    const char* modName = _registry.getString(mod);
    const char* funcName = _registry.getString(func);

    if (strcmp(modName, "MATH") == 0) return math::call(funcName, args);
    if (strcmp(modName, "GPIO") == 0) return gpio::call(funcName, args);

    return Value::Float(0.0f);
}

void Interpreter::visit(const CallStmt& stmt) {
    std::vector<Value> argValues;
    for (const auto& a : stmt.args) argValues.push_back(evaluate(a));
    handleCall(stmt.module, stmt.function, argValues);
}

void Interpreter::visit(const CallExpr& expr) {
    std::vector<Value> argValues;
    for (const auto& a : expr.args) argValues.push_back(evaluate(a));
    _lastValue = handleCall(expr.module, expr.function, argValues);
}

void Interpreter::visit(const GroupingExpr& expr){
    expr.expression->accept(*this);
}

void Interpreter::visit(const BlockStmt& stmt) {
    for (const auto& s : stmt.statements) {
        if (s) s->accept(*this);
    }
}

void Interpreter::visit(const EndStmt& expr){

}

void Interpreter::visit(const ReturnStmt& expr){

}

